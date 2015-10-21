/*////////////////////////////////////////////////////////////////////////////

This file is part of DynItemInst.

Copyright © 2015 David Goeth

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


#include "DllMain.h"
#include "api/g2ext.h"
#include <HookManager.h>
#include <Logger.h>
#define G2EXT_PARAM_NO_G2EXT_CONSOLE 1;

ILog* logger;
ICoreIngame* coreIngame;
std::stringstream logStream;

/*
 * Hooks needed target functions.
 * This function is exported to use it external (e.g. via Ikarus Script package).
*/
void Hook()
{
	HookManager::hook();
}

/*
 * Unhooks needed target functions.
 * This function is exported to use it external (e.g. via Ikarus Script package).
*/
void Unhook()
{
	HookManager::unhook();
}

/**
 * Called by G2Ext.
 */
HRESULT G2EXT_API G2Ext_ModVersion(int& nMajor, int& nMinor, G2EXT_DLL_TYPE& tType) 
{ 
	nMajor = G2EXT_MAJOR_VERSION; 
	nMinor = G2EXT_MINOR_VERSION; 
	tType = G2EXT_DLL_MOD; 
	return S_OK; 
};

/**
 * Called by G2Ext.
 */
HRESULT G2EXT_API G2Ext_ModPrepare(ICoreOutgame* pCore) 
{
	return S_OK;
};

/**
 * Called by G2Ext. Is Responsible for initialize objects derived from G2Ext and initialize
 * global hook system.
 */
HRESULT G2EXT_API G2Ext_ModInit(ICoreIngame* pCore) 
{
	coreIngame = pCore;
	logger = coreIngame->GetLog();
	Hook();
	return S_OK;
};

/*
 * DLL entry point called by the system. Its main purpose is to release allocated memory
 * when the process terminates. Therefore false memory leaks from detection tools can be avoided. 
 * Surely not a necessary behaviour but a cleaner solution. 
 */
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch(ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: 
		logStream<< "DLL_Process_Attach called."<< std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, true, false);
		break;
	case DLL_PROCESS_DETACH:
		logStream<< "DLL_Process_Detach called."<< std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, true, false);
		Unhook();
		Logger::release();
		break;
    }
    return TRUE;
}