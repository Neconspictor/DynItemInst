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

#ifndef Included_Hooks_H
#define Included_Hooks_H

#include "Module.h"
#include <map>
#include <list>
#include <api/core.h>
#include <Windows.h>
#include <sstream>

class zCParser;
extern ICoreIngame* coreIngame;
extern bool debug;
class zCPar_Symbol;
class zSTRING;

enum class ConfigFlags {
	DII = 1 << 0,
	LEVITATION = 1 << 1,
	TELEKINESIS = 1 << 2,
};

/**
 * A hook manager is responsible for managing all modules. Part of its job is to collect and storing
 * all function hooks of all registered modules and hooking and unhooking them.
 */
class HookManager {
public:

	HookManager();

	/**
	 * Default destructor.
	 */
	~HookManager() = default;

	/**
	 * Initializes internal hook engine and hooks all registered modules afterwards.
	 * After this function call all hooks of the registered modules are enabled.
	 */
	static int hook(int flags);

	/**
	 * Uninitializes internal hook engine, unhooks all registered modules and removes them.
	 * After this function call all hooks of the registered modules are disabled.
	 */
	static void unhook();

	/**
	 * Provides the current hook manager instance. If no hook manager exists a new one
	 * will be created.
	 * \return The current hook manager instance.
	 */
	static HookManager* getHookManager();

	/**
	 * Hooks all funcions of the registered modules.
	 */
	void hookModules();

	/**
	 * Unhooks all funcions of the registered modules.
	 */
	void unHookModules();

	/**
	 * Registers a module.
	 */
	void addModule(std::unique_ptr<Module> module);

	/**
	 * Replaces a function the pointer 'source' is pointing at with the the function located at 
	 * 'destination'. An optional description is used for debugging purposes.
	 * \param source The address of the original function.
	 * \param destination The address of the function hook.
	 * \param description A description of the module calling this function.
	 */
	void addFunctionHook(LPVOID* source, LPVOID destination, std::string description = "");

	/**
	 * Removes the hook located at 'destination' from the function the function pointer 
	 * source is pointing at. An optional description is used for debugging purposes.
	 * \param source The address of the original function.
	 * \param destination The address of the function hook.
	 * \param description A description of the module calling this function.
	 */
	void removeFunctionHook(LPVOID* source, LPVOID destination, std::string description = "");

private:
	static std::unique_ptr<HookManager> mInstance;
	static std::stringstream logStream;

	std::vector<std::unique_ptr<Module>> mModules;
	std::multimap<LPVOID,LPVOID> originalToHookAddress;
	std::map<LPVOID, LPVOID> hookToOriginalAddress;
	bool mCalled = false;
	int mFlags = 0;

	/**
	 * Unhooks all registered modules and releases allocated memory.
	 */
	static void release();

	/**
	 * registers a function hook. 'original' is therby the source address of the function
	 * that should be hooked by the function located at 'hook'.
	 * \param original The address of the original function.
	 * \param hook The address of the function hook.
	 */
	void registerHook(LPVOID original, LPVOID hook);

	/**
	 * unregisters a function hook. 'original' is therby the source address of the function
	 * is hooked by the function located at 'hook'.
	 * \param original The address of the original function.
	 * \param hook The address of the function hook.
	 */
	void unregisterHook(LPVOID original, LPVOID hook);

	/**
	 * This function takes an address named 'hook' and provides the address of the function, 
	 * which the function at address 'hook' has hooked. If no original function is found, NULL
	 * will be returned.
	 * \param hook The address of the function hook for which the original function address
	 * should be provided.
	 * \return The address of the original function
	 */
	LPVOID getOriginalAddress(LPVOID hook);

	/**
	 * Provides the address of the function, which has hooked the function located at address
	 * 'original'. If no function hook is found, NULL will be returned.
	 * \param original The address of the original function for which the hook function address
	 * should be provided.
	 * \return The address of function hook
	 */
	LPVOID getHookAddress(LPVOID original);
};

#endif Included_Hooks_H