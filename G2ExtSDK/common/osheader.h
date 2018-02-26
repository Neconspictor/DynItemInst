/*////////////////////////////////////////////////////////////////////////////

This file is part of the G2Ext SDK headers.

//////////////////////////////////////////////////////////////////////////////

The G2Ext SDK headers

Copyright © 2009, 2010 by Paindevs and Patrick Vogel

All Rights reserved.

THE WORK (AS DEFINED BELOW) IS PROVIDED
UNDER THE TERMS OF THIS CREATIVE COMMONS
PUBLIC LICENSE ("CCPL" OR "LICENSE").
THE WORK IS PROTECTED BY COPYRIGHT AND/OR
OTHER APPLICABLE LAW. ANY USE OF THE WORK
OTHER THAN AS AUTHORIZED UNDER THIS LICENSE
OR COPYRIGHT LAW IS PROHIBITED.

BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED
HERE, YOU ACCEPT AND AGREE TO BE BOUND BY THE
TERMS OF THIS LICENSE. TO THE EXTENT THIS
LICENSE MAY BE CONSIDERED TO BE A CONTRACT,
THE LICENSOR GRANTS YOU THE RIGHTS CONTAINED
HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF
SUCH TERMS AND CONDITIONS.

Full license at http://creativecommons.org/licenses/by-nc/3.0/legalcode

/////////////////////////////////////////////////////////////////////////////*/

#ifndef __G2EXT_OSHEADER_H__
#define __G2EXT_OSHEADER_H__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  // -- keep WinAPI compatibility with min. Windows XP (NT 5.1)
#endif //_WIN32_WINNT

#define  WIN32_LEAN_AND_MEAN
#include <windows.h>

#define G2EXT_API __stdcall

#define _WIDEN(x) L ## x
#define WIDEN(x) _WIDEN(x)

#define _DEFTOSTR(x) #x
#define DEFTOSTR(x) _DEFTOSTR(x)

#define G2EXT_DELETE( obj ) if (obj != NULL){delete obj;obj = NULL;}

#endif //__G2EXT_OSHEADER_H__