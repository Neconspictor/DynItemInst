/*////////////////////////////////////////////////////////////////////////////

This file is part of neclib.

Copyright © 2015-2020 David Goeth

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

/////////////////////////////////////////////////////////////////////////////**/

#include <DllMain.h>
#include <HookManager.h>
#include <Logger.h>
#include <Util.h>
#include <iostream>

std::stringstream mLogStream;

/**
 * Hooks needed target functions.
 * This function is exported to use it external (e.g. via Ikarus Script package).
*/
int Hook(int flags)
{
	return HookManager::hook(flags);
}

/**
 * Unhooks needed target functions.
 * This function is exported to use it external (e.g. via Ikarus Script package).
*/
void Unhook()
{
	HookManager::unhook();
}

/**
 * DLL entry point called by the system. It's main purpose is to release allocated memory
 * when the process terminates. Therefore false memory leaks from detection tools can be avoided. 
 * Surely not a necessary behaviour but a cleaner solution. 
*/
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch(ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: 
		std::cout << "You should always see me!" << std::endl;
		util::setModuleHandle(hModule);
		mLogStream<< "DLL_Process_Attach called."<< std::endl;
		Logger::getLogger()->logAlways(&mLogStream);
		break;
	case DLL_PROCESS_DETACH:
		mLogStream<< "DLL_Process_Detach called."<< std::endl;
		Logger::getLogger()->logAlways(&mLogStream);
		Unhook();
		Logger::release();
		break;
    }
    return TRUE;
}