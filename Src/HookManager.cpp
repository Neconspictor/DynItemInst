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
/*#ifdef _DEBUG
	#pragma comment (lib, "libMinHook-x86-v140-mtd.lib")
#else
	#pragma comment (lib, "libMinHook-x86-v140_xp-mt.lib")
#endif*/
#include <string>
#include <DII.h>
#include <DaedalusExports.h>
#include <sstream>
#include <Logger.h>
#include <Configuration.h>
#include <Levitation.h>
#include <CustomNpcFocus.h>

std::unique_ptr<HookManager> HookManager::mInstance;
std::stringstream HookManager::logStream;

HookManager* HookManager::getHookManager()
{
	if (!mInstance)
	{
		mInstance = std::make_unique<HookManager>();
	}
	return mInstance.get();
}

void HookManager::release()
{
	mInstance.reset();
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
		util::logWarning(&logStream);
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
	if (it == hookToOriginalAddress.end()) {return NULL;}
	return it->second;
}

LPVOID HookManager::getHookAddress(LPVOID original)
{
	std::multimap<LPVOID,LPVOID>::iterator it = originalToHookAddress.find(original);
	if (it == originalToHookAddress.end()) {return NULL;}
	return it->second;
};

void HookManager::hookModules()
{
	for (auto& module : mModules) {
		module->hookModule();		
	}
}

void HookManager::unHookModules()
{
	for (auto& module : mModules) {
		module->unHookModule();
	}
};

void HookManager::addModule(std::unique_ptr<Module> module)
{
	mModules.emplace_back(std::move(module));
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
		util::logFatal(&logStream);
		return;
	}

	MH_EnableHook(address);

	logStream.setf(std::ios::hex, std::ios::basefield);
	logStream << "HookManager::addFunctionHook: " << "Function at address 0x"<< address 
		<< " for module "<< description <<" was successfully hooked."<< std::endl;
	logStream.unsetf(std::ios::hex);
	util::logAlways(&logStream);

	registerHook(address, destination);
}

void HookManager::removeFunctionHook(LPVOID* source, LPVOID destination, std::string description)
{
	LPVOID original = getOriginalAddress(destination);
	if (original == NULL)
	{
		logStream.setf(std::ios::hex, std::ios::basefield);
		logStream << "HookManager::removeFunctionHook: Error: Couldn't find original function for function hook 0x"<< destination
			<< " for module " << description<< std::endl;
		logStream.unsetf(std::ios::hex);
		util::logFatal(&logStream);
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
		util::logFatal(&logStream);
		return;
	}

	*source = original;

	logStream.setf(std::ios::hex, std::ios::basefield);
	logStream << "HookManager::removeFunctionHook: Function hook at address 0x"<< original <<" for module " << description
		<< " was successfully removed."<< std::endl;
	util::logAlways(&logStream);
	logStream.unsetf(std::ios::hex);

	unregisterHook(original, destination);
};


HookManager::HookManager()
{
	mCalled = false;
	mFlags = 0;
};

int HookManager::hook(int flags)
{
	HookManager* manager = getHookManager();
	if (mInstance->mCalled) return mInstance->mFlags;
	mInstance->mCalled = true;

	// init Logger since Configuration file is initialized there
	Logger::getLogger();

	logStream << "HookManager::hook: read Configuration: " << std::endl;
	logStream << "debugEnabled = " << Configuration::debugEnabled() << std::endl;
	logStream << "logToFile = " << Configuration::getLogToFile() << std::endl;
	logStream << "logTozSpy = " << Configuration::getLogTozSpy() << std::endl;
	logStream << "logToConsole = " << Configuration::getLogToConsole() << std::endl;
	logStream << "logInfos = " << Configuration::getLogInfos()<< std::endl;
	logStream << "logWarnings = " << Configuration::getLogWarnings() << std::endl;
	logStream << "logErrors = " << Configuration::getLogErrors() << std::endl;
	logStream << "logFatals = " << Configuration::getLogFatals() << std::endl;
	logStream << "HookManager::hook: finished read." << std::endl;

	util::logAlways(&logStream);

	logStream << "HookManager::hook: Hook target functions..."<< std::endl;
	util::logAlways(&logStream);

	if (MH_Initialize() != MH_OK) {
		logStream << "HookManager::hook: Couldn't initialize hook engine!"<< std::endl;
		util::logFatal(&logStream);
		unhook();
		return 0;
	}

	manager->addModule(std::make_unique<DaedalusExports>());


	if (flags &  static_cast<int>(ConfigFlags::DII)) {
		manager->addModule(std::make_unique <DII>() );
		mInstance->mFlags |= static_cast<int>(ConfigFlags::DII);
	}

	if (flags & static_cast<int>(ConfigFlags::LEVITATION)) {
		manager->addModule(std::make_unique <Levitation>());
		mInstance->mFlags |= static_cast<int>(ConfigFlags::LEVITATION);
	}

	if (flags & static_cast<int>(ConfigFlags::TELEKINESIS)) {
		manager->addModule(std::make_unique<Telekinesis>());
		mInstance->mFlags |= static_cast<int>(ConfigFlags::TELEKINESIS);
	}

	manager->hookModules();

	logStream << "HookManager::hook: done." << std::endl;
	Logger::getLogger()->logAlways(&logStream);

	return mInstance->mFlags;
};


void HookManager::unhook()
{
	if (!mInstance) return;
	logStream << "HookManager::unHook: Unhook functions..." << std::endl;
	mInstance->unHookModules();
	release();
	logStream << "HookManager::unHook: Unhook done."<< std::endl;
	util::logInfo(&logStream);
}