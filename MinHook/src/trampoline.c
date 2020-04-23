/*
 *  MinHook - The Minimalistic API Hooking Library for x64/x86
 *  Copyright (C) 2009-2017 Tsuda Kageyu.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <windows.h>

#ifndef ARRAYSIZE
    #define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#endif

#if defined(_M_X64) || defined(__x86_64__)
    #include "./hde/hde64.h"
    typedef hde64s HDE;
    #define HDE_DISASM(code, hs) hde64_disasm(code, hs)
#else
    #include "./hde/hde32.h"
    typedef hde32s HDE;
    #define HDE_DISASM(code, hs) hde32_disasm(code, hs)
#endif

#include "trampoline.h"
#include "buffer.h"

// Maximum size of a trampoline function.
#if defined(_M_X64) || defined(__x86_64__)
    #define TRAMPOLINE_MAX_SIZE (MEMORY_SLOT_SIZE - sizeof(JMP_ABS))
#else
    #define TRAMPOLINE_MAX_SIZE MEMORY_SLOT_SIZE
#endif

//-------------------------------------------------------------------------
static BOOL IsCodePadding(LPBYTE pInst, UINT size)
{
    UINT i;

    if (pInst[0] != 0x00 && pInst[0] != 0x90 && pInst[0] != 0xCC)
        return FALSE;

    for (i = 1; i < size; ++i)
    {
        if (pInst[i] != pInst[0])
            return FALSE;
    }
    return TRUE;
}

//-------------------------------------------------------------------------
BOOL CreateTrampolineFunction(PTRAMPOLINE ct)
{
    JMP_ABS_32_BIT push = {
        0x68,                   // push
        0x00000000,             // absolute destination address
        0xC3                    // ret
    };

    UINT8     oldPos   = 0;
    UINT8     newPos   = 0;
    ULONG_PTR jmpDest  = 0;     // Destination address of an internal jump.
    BOOL      finished = FALSE; // Is the function completed?

    ct->patchAbove = FALSE;
    ct->nIP        = 0;

    do
    {
        HDE       hs;
        UINT      copySize;
        LPVOID    pCopySrc;
        ULONG_PTR pOldInst = (ULONG_PTR)ct->pTarget     + oldPos;
        ULONG_PTR pNewInst = (ULONG_PTR)ct->pTrampoline + newPos;

        copySize = HDE_DISASM((LPVOID)pOldInst, &hs);
        if (hs.flags & F_ERROR)
            return FALSE;

        pCopySrc = (LPVOID)pOldInst;

        if (oldPos >= sizeof(JMP_ABS_32_BIT))
        {
            push.address = (UINT32)pOldInst;
            pCopySrc = &push;
            copySize = sizeof(push);
            finished = TRUE;
        }

        // Can't alter the instruction length in a branch.
        if (pOldInst < jmpDest && copySize != hs.len)
            return FALSE;

        // Trampoline function is too large.
        if ((newPos + copySize) > TRAMPOLINE_MAX_SIZE)
            return FALSE;

        // Trampoline function has too many instructions.
        if (ct->nIP >= ARRAYSIZE(ct->oldIPs))
            return FALSE;

        ct->oldIPs[ct->nIP] = oldPos;
        ct->newIPs[ct->nIP] = newPos;
        ct->nIP++;

        // Avoid using memcpy to reduce the footprint.
#ifndef _MSC_VER
        memcpy((LPBYTE)ct->pTrampoline + newPos, pCopySrc, copySize);
#else
        __movsb((LPBYTE)ct->pTrampoline + newPos, pCopySrc, copySize);
#endif
        newPos += copySize;
        oldPos += hs.len;
    }
    while (!finished);

    // Is there enough place for a long jump?
    if (oldPos < sizeof(JMP_ABS_32_BIT)
        && !IsCodePadding((LPBYTE)ct->pTarget + oldPos, sizeof(JMP_ABS_32_BIT) - oldPos))
    {
        return FALSE;
    }

    return TRUE;
}
