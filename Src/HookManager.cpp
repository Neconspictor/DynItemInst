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
#include <General.h>
#include <sstream>
#include <Logger.h>
#include <Configuration.h>
#include <Levitation.h>

std::unique_ptr<HookManager> HookManager::mInstance;
std::stringstream HookManager::mLogStream;

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
		mLogStream << __FUNCTION__ << ": Address was previously hooked! Hook count = "
			<< originalToHookAddress.count(original) + 1 << std::endl;
		util::logWarning(&mLogStream);
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
		mLogStream << __FUNCTION__ << ": hooking " << module->getName() << std::endl;
		util::logAlways(&mLogStream);
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

	if (getHookAddress(address)) {
		mLogStream.setf(std::ios::hex, std::ios::basefield);
		mLogStream << __FUNCTION__ << ": Function at address 0x" << address
			<< " for module " << description << " is already hooked!" << std::endl;
		mLogStream.unsetf(std::ios::hex);
		util::logFatal(&mLogStream);
		return;
	}


	MH_STATUS status = MH_CreateHook(*source, destination,source);

	//LONG error = 0;
	if (status != MH_OK)
	{
		mLogStream.setf(std::ios::hex, std::ios::basefield);
		mLogStream << __FUNCTION__ << ": Couldn't hook function at address 0x"<< address
			<< " for module " << description<< std::endl;
		mLogStream.unsetf(std::ios::hex);
		util::logFatal(&mLogStream);
		return;
	}

	MH_EnableHook(address);

	mLogStream.setf(std::ios::hex, std::ios::basefield);
	mLogStream << __FUNCTION__ << ": Function at address 0x"<< address
		<< " for module "<< description <<" was successfully hooked."<< std::endl;
	mLogStream.unsetf(std::ios::hex);
	util::logAlways(&mLogStream);

	registerHook(address, destination);
}

void HookManager::removeFunctionHook(LPVOID* source, LPVOID destination, std::string description)
{
	LPVOID original = getOriginalAddress(destination);
	if (original == NULL)
	{
		mLogStream.setf(std::ios::hex, std::ios::basefield);
		mLogStream << "HookManager::removeFunctionHook: Error: Couldn't find original function for function hook 0x"<< destination
			<< " for module " << description<< std::endl;
		mLogStream.unsetf(std::ios::hex);
		util::logFatal(&mLogStream);
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
		mLogStream.setf(std::ios::hex, std::ios::basefield);
		mLogStream << "HookManager::removeFunctionHook: Fault: Couldn't remove hook function at address 0x"<< destination 
			<<" for module " << description<< std::endl;
		mLogStream.unsetf(std::ios::hex);
		util::logFatal(&mLogStream);
		return;
	}

	*source = original;

	mLogStream.setf(std::ios::hex, std::ios::basefield);
	mLogStream << "HookManager::removeFunctionHook: Function hook at address 0x"<< original <<" for module " << description
		<< " was successfully removed."<< std::endl;
	util::logAlways(&mLogStream);
	mLogStream.unsetf(std::ios::hex);

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

	mLogStream << __FUNCTION__ << ": read Configuration: " << std::endl;
	mLogStream << "debugEnabled = " << Configuration::debugEnabled() << std::endl;
	mLogStream << "logToFile = " << Configuration::getLogToFile() << std::endl;
	mLogStream << "logTozSpy = " << Configuration::getLogTozSpy() << std::endl;
	mLogStream << "logToConsole = " << Configuration::getLogToConsole() << std::endl;
	mLogStream << "logInfos = " << Configuration::getLogInfos()<< std::endl;
	mLogStream << "logWarnings = " << Configuration::getLogWarnings() << std::endl;
	mLogStream << "logErrors = " << Configuration::getLogErrors() << std::endl;
	mLogStream << "logFatals = " << Configuration::getLogFatals() << std::endl;
	mLogStream << __FUNCTION__ << ": finished read." << std::endl;

	util::logAlways(&mLogStream);

	mLogStream << __FUNCTION__ << ": Hook target functions..."<< std::endl;
	util::logAlways(&mLogStream);

	if (MH_Initialize() != MH_OK) {
		mLogStream << __FUNCTION__ << ": Couldn't initialize hook engine!"<< std::endl;
		util::logFatal(&mLogStream);
		unhook();
		return 0;
	}

	manager->addModule(std::make_unique<General>());


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

	mLogStream << __FUNCTION__ << ": done." << std::endl;
	Logger::getLogger()->logAlways(&mLogStream);

	return mInstance->mFlags;
};


void HookManager::unhook()
{
	if (!mInstance) return;
	mLogStream << __FUNCTION__ << ": Unhook functions..." << std::endl;
	mInstance->unHookModules();
	release();
	mLogStream << __FUNCTION__ << ": Unhook done."<< std::endl;
	util::logInfo(&mLogStream);
}