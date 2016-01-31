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

/////////////////////////////////////////////////////////////////////////////*/

#include "HookManager.h"
#include "Util.h"
#include <MinHook.h>
#ifdef _DEBUG
	#pragma comment (lib, "libMinHook-x86-v140-mdd.lib")
#else
	#pragma comment (lib, "libMinHook-x86-v140-md.lib")
#endif
#include <string>
#include <DynItemInst.h>
#include <DaedalusExports.h>
#include <sstream>
#include <Logger.h>

HookManager* HookManager::instance = nullptr;
std::stringstream HookManager::logStream = std::stringstream();

HookManager* HookManager::getHookManager()
{
	if (instance == nullptr)
	{
		instance = new HookManager();
	}
	return instance;
}

void HookManager::release()
{
	if (instance == nullptr) return;

	std::list<Module*>::iterator it;
	it = instance->modules.begin();

	while(it != instance->modules.end()) {
		Module* module = *it;
		instance->modules.erase(it);
		it = instance->modules.begin();
		delete module;
	}
	
	SAFE_DELETE(instance);
}

void HookManager::registerHook(LPVOID original, LPVOID hook)
{
	std::multimap<LPVOID, LPVOID>::iterator it;
	it = originalToHookAddress.find(original);

	// Was the address previously hooked?
	if (it != originalToHookAddress.end())
	{
		logStream << "HookManager::registerHook: Warning: Address was previously hooked! Hook count = " 
			<< originalToHookAddress.count(original) + 1 << std::endl;
		Logger::getLogger()->log(Logger::Warning, &logStream, true, false);
	}
	originalToHookAddress.insert(std::pair<LPVOID,LPVOID>(original, hook));
	hookToOriginalAddress.insert(std::pair<LPVOID,LPVOID>(hook, original));
}

void HookManager::unregisterHook(LPVOID original, LPVOID hook)
{
	originalToHookAddress.erase(original);
	hookToOriginalAddress.erase(hook);
}

LPVOID HookManager::getOriginalAddress(LPVOID hook)
{
	std::map<LPVOID,LPVOID>::iterator it = hookToOriginalAddress.find(hook);
	if (it == hookToOriginalAddress.end()) {return nullptr;}
	return it->second;
}

LPVOID HookManager::getHookAddress(LPVOID original)
{
	std::multimap<LPVOID,LPVOID>::iterator it = originalToHookAddress.find(original);
	if (it == originalToHookAddress.end()) {return nullptr;}
	return it->second;
};

void HookManager::hookModules()
{
	std::list<Module*>::iterator it = modules.begin();
	for (;it != modules.end(); ++it) {
		(*it)->hookModule();		
	}
}

void HookManager::unHookModules()
{
	std::list<Module*>::iterator it = modules.begin();
	for (;it != modules.end(); ++it) {
		(*it)->unHookModule();		
	}
};

void HookManager::addModule(Module* module)
{
	this->modules.push_back(module);
}

void HookManager::addFunctionHook(LPVOID* source, LPVOID destination, std::string description)
{
	LPVOID address = *source;
	MH_STATUS status = MH_CreateHook(*source, destination,source);

	//LONG error = 0;
	if (status != MH_OK)
	{
		logStream.setf(std::ios::hex, std::ios::basefield);
		logStream << "HookManager::addFunctionHook: Error: Couldn't hook function at address 0x"<< address
			<< " for module " << description<< std::endl;
		logStream.unsetf(std::ios::hex);
		Logger::getLogger()->log(Logger::Fault, &logStream, true, false);
		return;
	}

	MH_EnableHook(address);

	logStream.setf(std::ios::hex, std::ios::basefield);
	logStream << "HookManager::addFunctionHook: " << "Function at address 0x"<< address 
		<< " for module "<< description <<" was successfully hooked."<< std::endl;
	logStream.unsetf(std::ios::hex);
	Logger::getLogger()->log(Logger::Info, &logStream, true, false);

	registerHook(address, destination);
}

void HookManager::removeFunctionHook(LPVOID* source, LPVOID destination, std::string description)
{
	LPVOID original = getOriginalAddress(destination);
	if (original == nullptr)
	{
		logStream.setf(std::ios::hex, std::ios::basefield);
		logStream << "HookManager::removeFunctionHook: Error: Couldn't find original function for function hook 0x"<< destination
			<< " for module " << description<< std::endl;
		logStream.unsetf(std::ios::hex);
		Logger::getLogger()->log(Logger::Fault, &logStream, true, false);
		return;
	}

	MH_STATUS status;
	status = MH_DisableHook(original);
	if (status == MH_OK)
	{
		status = MH_RemoveHook(original);
	}

	if (status != MH_OK)
	{
		logStream.setf(std::ios::hex, std::ios::basefield);
		logStream << "HookManager::removeFunctionHook: Fault: Couldn't remove hook function at address 0x"<< destination 
			<<" for module " << description<< std::endl;
		logStream.unsetf(std::ios::hex);
		Logger::getLogger()->log(Logger::Fault, &logStream, true, false);
		return;
	}

	*source = original;

	logStream.setf(std::ios::hex, std::ios::basefield);
	logStream << "HookManager::removeFunctionHook: Function hook at address 0x"<< original <<" for module " << description
		<< " was successfully removed."<< std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, true, false);
	logStream.unsetf(std::ios::hex);

	unregisterHook(original, destination);
};

HookManager::~HookManager()
{
};


HookManager::HookManager()
{
	called = false;
	this->instance = nullptr;
};

void HookManager::hook()
{
	HookManager* manager = HookManager::getHookManager();
	if (instance->called) return;
	instance->called = true;
	logStream << "HookManager::hook: Try to hook target functions..."<< std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, true);

	if (MH_Initialize() != MH_OK) {
		logStream << "HookManager::hook: Couldn't initialize hook engine!"<< std::endl;
		Logger::getLogger()->log(Logger::Fault, &logStream, true);
		unhook();
		return;
	}

	Module* dynItemInstModule = new DynItemInst();
	Module* externals = new DaedalusExports();
	manager->addModule(dynItemInstModule);
	manager->addModule(externals);
	manager->hookModules();
};


void HookManager::unhook()
{
	if (instance == nullptr) return;
	instance->unHookModules();
	release();
	logStream << "HookManager::unHook: Unhooking done."<< std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, true);
}