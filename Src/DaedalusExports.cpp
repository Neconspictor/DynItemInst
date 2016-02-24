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


#include <api/g2/zcparser.h>
#include "oCItemExtended.h"
#include <ObjectManager.h>
#include <api/g2/oCObjectFactory.h>
#include <Logger.h>
#include <DaedalusExports.h>
#include <set>
#include <ocgameExtended.h>
#include <DynItemInst.h>


const float DaedalusExports::LIB_VERSION = 1.02f;

DaedalusExports::DaedalusExports() : Module()
{
	moduleDesc = "DaedalusExports";
}

DaedalusExports::~DaedalusExports()
{
}

void DaedalusExports::hookModule()
{

}

void DaedalusExports::unHookModule()
{

}


void __cdecl DaedalusExports::DII_CreateNewItem(int index, int instanceId) // Func void DII_CreateNewItem(var C_Item item, VAR INT instanceId)
{
	if (index <= 0) return;
	zCParser* parser = zCParser::GetParser();
	zCPar_Symbol* symbol = parser->GetSymbol(index);
	oCItem* item = (oCItem*)symbol->offset;

	// Check if provided instance id is valid
	zCPar_Symbol* newInstanceSym = parser->GetSymbol(instanceId);

	if (newInstanceSym == nullptr)
	{
		logStream << "newInstanceSym is Null! No item will be created!" << std::endl;
		Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
		return;
	}

	// Create new item and store it in global variable 'ITEM'
	if (item != nullptr)
	{

		int* refCtr = (int*)((BYTE*)item + 0x4);
		//logStream << "refCtr: " << refCtr << std::endl;
		//Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
		typedef void(__thiscall* OCItemInitByScript)(void* pThis, int, int);
		OCItemInitByScript oCItemInitByScript = (OCItemInitByScript)0x00711BD0;
		oCItemInitByScript(item, instanceId, 1);
		//oCGame::GetGame()->GetWorld()->AddVob(item);

	}
	else
	{
		item = oCObjectFactory::GetFactory()->CreateItem(instanceId);
		//oCGame::GetGame()->GetWorld()->AddVob(item);
	}

	// update the c_item
	symbol->offset = (int)item;
	logStream << "symbol->offset: " << symbol->offset << std::endl;
	Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
}


int DaedalusExports::DII_CreateNewInstance(oCItem* item) //Func int CreateNewItem(var C_Item item)
{
	if (item == nullptr) {return NULL;}

	logStream << "Param: " << item->name.ToChar();
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);

	// Create new instance with item
	ObjectManager* manager = ObjectManager::getObjectManager();
	int key = manager->createNewInstanceId(item);
	int index = manager->getDynInstanceId(item);
	logStream << "Index: " << index << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);

	logStream << "CreateNewInstance::key: " << key << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream);
	return key;
}

void DaedalusExports::DII_EquipItem(oCNpc* npc, int instanceId)
{
	if (npc == nullptr) return;
	oCNpcInventory* inv = npc->GetInventory();
	if (inv == nullptr) return;

	oCItem* item = DynItemInst::getInvItemByInstanceId2(inv, instanceId);
	if (item == nullptr) return;
	npc->Equip(item);
}

int DaedalusExports::DII_IsDynamic(oCItem* item) // Func DII_IsDynamic(VAR C_ITEM item)
{
	if (item == nullptr) {return FALSE;}

	bool modified = ObjectManager::getObjectManager()->IsModified(item);
	if (modified)
	{
		return TRUE;
	}
	return FALSE;
}


void oCItemOperatorDelete(oCItem* item)
{
	XCALL(0x007144A0);
}


DII_UserData::Data* DaedalusExports::DII_GetUserData(int instanceId) // Func DII_UserData DII_GetUserData(var int instanceId)
{
	ObjectManager* manager = ObjectManager::getObjectManager();

	if (!manager->IsModified(instanceId))
	{
		return nullptr;
	}

	DynInstance* storeItem = manager->getInstanceItem(instanceId);
	return storeItem->getUserData();
}


float DaedalusExports::DII_GetLibVersion()
{
	return LIB_VERSION;
}

void DaedalusExports::DII_DoStatistics()
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	Logger* logger = Logger::getLogger();
	int instanceBegin = manager->getInstanceBegin();

	if (instanceBegin < 0)
	{
		logStream << "Nothing to do, instanceBegin < 0!" << std::endl;
		logger->log(Logger::Info, &logStream, false, true, true);
		return;
	}

	int dynamicItemCount = 0;
	auto compare = [](oCItem* first, oCItem* second)->bool {
		return first < second;
	};

	std::set<oCItem*, std::function<bool(oCItem*, oCItem*)>> itemSet(compare);

	auto func = [&](oCItem* item) ->void {
		if (item == nullptr) return;

		int id = manager->getInstanceId(*item);
		if (id >= instanceBegin)
		{
			auto it = itemSet.find(item);
			if (*it == nullptr)
			{
				itemSet.insert(item);
				++dynamicItemCount;

				logStream << "Found item with dynamic instance id: " << id << std::endl;
				int refCtr = *(int*)((BYTE*)item + 0x4);
				logStream << "refCtr: " << refCtr << std::endl;
				logger->log(Logger::Info, &logStream, false, true, true);
			}
		}
	};

	manager->callForAllItems(func);

	logStream << "Statistics: " << dynamicItemCount << " items have a dynamic instance id." << std::endl;
	logger->log(Logger::Info, &logStream, false, true, true);
}

void DaedalusExports::DII_UpdateInstance(oCItem* item)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	int index = manager->getDynInstanceId(item);
	if (index > 0)
	{
		DynInstance* dynInstance = manager->getInstanceItem(index);
		dynInstance->store(*item);

		//update all items of this id
		auto func = [&](oCItem* itm) ->void {
			if (itm == nullptr) return;
			
			int id = manager->getDynInstanceId(itm);
			if (id == index)
			{
				int refCtr = *(int*)((BYTE*)itm + 0x4);
				typedef void(__thiscall* OCItemInitByScript)(void* pThis, int, int);
				OCItemInitByScript oCItemInitByScript = (OCItemInitByScript)0x00711BD0;
				
				oCItemInitByScript(itm, id, itm->instanz);
			}
		};
		manager->callForAllItems(func);

	} else
	{
		logStream << "DII_UpdateInstance: Couldn't update dynamic instance of item at address" << item << std::endl;
		Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
	}
}

void DaedalusExports::DII_AssignInstanceId(oCItem* item, int instanceId)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->assignInstanceId(item, instanceId);
}
