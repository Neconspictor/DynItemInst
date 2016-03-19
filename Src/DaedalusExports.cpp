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
#include <Util.h>
#include <Levitation.h>


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
		logStream << "DaedalusExports::DII_CreateNewItem: newInstanceSym is Null! No item will be created!" << std::endl;
		util::logWarning(&logStream);
		return;
	}

	// Create new item and store it in global variable 'ITEM'
	/*if (item != nullptr)
	{

		int* refCtr = (int*)((BYTE*)item + 0x4);
		//logStream << "refCtr: " << refCtr << std::endl;
		//Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
		typedef void(__thiscall* OCItemInitByScript)(void* pThis, int, int);
		OCItemInitByScript oCItemInitByScript = (OCItemInitByScript)0x00711BD0;
		logStream << "Effect: " << item->effect.ToChar() << std::endl;
		oCItemInitByScript(item, instanceId, 1);
		logStream << "Effect: " << item->effect.ToChar() << std::endl;
		//oCGame::GetGame()->GetWorld()->AddVob(item);
		util::debug(&logStream);
	}
	else
	{*/
	if (item != nullptr)
	{
		//oCGame::GetGame()->GetGameWorld()->RemoveVob(item);
		oCGame::GetGame()->GetWorld()->RemoveVob(item);
	};
		item = oCObjectFactory::GetFactory()->CreateItem(instanceId);
		//oCGame::GetGame()->GetWorld()->AddVob(item);
	//}

	// update the c_item
	symbol->offset = (int)item;
	logStream << "DaedalusExports::DII_CreateNewItem: symbol->offset: " << symbol->offset << std::endl;
	util::debug(&logStream);
}


int DaedalusExports::DII_CreateNewInstance(oCItem* item) //Func int CreateNewItem(var C_Item item)
{
	if (item == nullptr) {return NULL;}

	logStream << "Param: " << item->name.ToChar();
	util::debug(&logStream);

	// Create new instance with item
	ObjectManager* manager = ObjectManager::getObjectManager();
	int key = manager->createNewInstanceId(item);
	int index = manager->getDynInstanceId(item);
	logStream << "Index: " << index << std::endl;
	util::debug(&logStream);

	logStream << "CreateNewInstance::key: " << key << std::endl;
	util::debug(&logStream);
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

int DaedalusExports::DII_IsInstanceDynamic(int instanceId)
{
	bool modified = ObjectManager::getObjectManager()->isDynamicInstance(instanceId);
	if (modified)
	{
		return TRUE;
	}
	return FALSE;
}

BYTE* DaedalusExports::DII_GetUserData(int instanceId) // Func DII_UserData DII_GetUserData(var int instanceId)
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
		util::debug(&logStream);
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
				util::debug(&logStream);
			}
		}
	};

	manager->callForAllItems(func);

	logStream << "Statistics: " << dynamicItemCount << " items have a dynamic instance id." << std::endl;
	util::debug(&logStream);
}

void DaedalusExports::DII_UpdateInstance(oCItem* item)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	int index = manager->getDynInstanceId(item);
	if (index > 0)
	{
		DynInstance* dynInstance = manager->getInstanceItem(index);
		if (dynInstance == nullptr)
		{
			logStream << "DII_UpdateInstance: dynInstance is null!" << std::endl;
			util::logWarning(&logStream);
			return;
		}
		dynInstance->store(*item);

		zCWorld* world = oCGame::GetGame()->GetWorld();

		//update all items of this id
		auto func = [&](oCItem* itm) ->void {
			if (itm == nullptr) return;
			
			int id = manager->getDynInstanceId(itm);
			if (id == index)
			{
				//int refCtr = *(int*)((BYTE*)itm + 0x4);
				
				bool isInWorld = manager->isItemInWorld(itm);
				int flags = itm->flags;
				itm->RemoveEffect();
				itm->InitByScript(id, itm->instanz);
				itm->flags = flags;

				//itm->InsertEffect();

				if (isInWorld)
				{
					world->AddVob(item);
				}
			}
		};
		manager->callForAllItems(func);

	} else
	{
		logStream << "DII_UpdateInstance: Couldn't update dynamic instance of item at address" << item << std::endl;
		util::logWarning(&logStream);
	}
}

void DaedalusExports::DII_AssignInstanceId(oCItem* item, int instanceId)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->assignInstanceId2(item, instanceId);
}

void DaedalusExports::DII_MarkAsReusable(int instanceId, int previousId)
{
	logStream << "DaedalusExports::DII_MarkAsReusable: before call!" << std::endl;
	util::debug(&logStream);
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->markAsReusable(instanceId, previousId);
	//manager->assignInstanceId2(item, instanceId);
	logStream << "DaedalusExports::DII_MarkAsReusable: called" << std::endl;
	util::debug(&logStream);
}

int DaedalusExports::DII_AreChangesPerformed()
{
	if ( DynItemInst::itemsAreModified())
	{
		//returning clear values is here important!
		return 1;
	}
		//returning clear values is here important!
		return 0;
}

void DaedalusExports::DII_GetItemByInstanceId(int index,  int instanceId)
{
	if (index <= 0) return;
	zCParser* parser = zCParser::GetParser();
	zCPar_Symbol* symbol = parser->GetSymbol(index);
	oCItem* item = (oCItem*)symbol->offset;

	// Check if provided instance id is valid
	zCPar_Symbol* instanceSym = parser->GetSymbol(instanceId);

	if (!instanceSym)
	{
		logStream << "DaedalusExports::DII_GetItemByInstanceId instanceSym is Null! No item will be searched!" << std::endl;
		util::logWarning(&logStream);
		return;
	}


	ObjectManager* manager = ObjectManager::getObjectManager();
	item = manager->getItemByInstanceId(instanceId);

	if (!item)
	{
		logStream << "DaedalusExports::DII_GetItemByInstanceId item is null!" << std::endl;
		util::logWarning(&logStream);
	} else
	{
		logStream << "DaedalusExports::callForAllItems: item found..." << std::endl;
		util::debug(&logStream);
	};

	// update the c_item
	symbol->offset = (int)item;
}

void DaedalusExports::DII_ChangeItemsInstanceId(int targetId, int newId)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	zCWorld* world = oCGame::GetGame()->GetWorld();
	oCWorld* gameWorld = oCGame::GetGame()->GetGameWorld();
	oCObjectFactory* factory = oCObjectFactory::GetFactory();
	std::list<oCItem*> targetList;
	auto func = [&](oCItem* item) ->void {
		if (item->GetInstance() == targetId && (item->instanz != 666))
		{
			targetList.push_back(item);
		}
	};


	zCListSort<oCItem>* itemList = world->GetItemList();
	while (itemList != nullptr) {
		oCItem* item = itemList->GetData();
		if (item != nullptr)
		{
			func(item);
		}
		itemList = itemList->GetNext();
	}


	//.text:0073ABE0 public: class oCVob * __thiscall oCNpc::GetLeftHand(void) proc near
	typedef oCVob*(__thiscall* OCNpcGetLeftHand)(oCNpc*);
	OCNpcGetLeftHand oCNpcGetLeftHand = (OCNpcGetLeftHand)0x0073ABE0;

	// don't consider items located in an npc's inventory
	zCListSort<oCNpc>* npcList = world->GetNpcList();

	while (npcList != nullptr) {
		oCNpc* npc = npcList->GetData();
		if (npc == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}

		oCVob* leftHanfVob = oCNpcGetLeftHand(npc);
		oCItem* item = dynamic_cast<oCItem*>(leftHanfVob);
		if (item)
		{
			auto findIter = find(targetList.begin(), targetList.end(), item);
			if (findIter != targetList.end())
			{
				logStream << "Found item in npc's left hand that should not be considered!" << std::endl;
				util::debug(&logStream);
				targetList.erase(findIter);
			}
		}

		oCNpcInventory* inventory = npc->GetInventory();
		if (inventory == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}

		inventory->UnpackAllItems();
		zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
		while (list != nullptr) {
			oCItem* item = list->GetData();
			if (item != nullptr)
			{
				auto findIter = find(targetList.begin(), targetList.end(), item);
				if (findIter != targetList.end())
				{
					logStream << "Found item in npc's inventory that should not be considered!" << std::endl;
					util::debug(&logStream);
					targetList.erase(findIter);
				}
			}

			list = list->GetNext();
		}
		npcList = npcList->GetNext();
	}


	typedef void(__thiscall* ZCVobRemoveVobSubtreeFromWorld)(void*);
	ZCVobRemoveVobSubtreeFromWorld zCVobRemoveVobSubtreeFromWorld = (ZCVobRemoveVobSubtreeFromWorld) 0x00601C60;

	typedef void(__thiscall* ZCObjectSetObjectName)(void*, zSTRING const &);
	ZCObjectSetObjectName zCObjectSetObjectName = (ZCObjectSetObjectName)0x005A9CE0;

	//.text:0077D0E0 public: void __thiscall oCVob::SetOnFloor(class zVEC3 &) proc near
	//typedef void(__thiscall* OCVobSetOnFloor)(void*, zVEC3 &);
	//OCVobSetOnFloor oCVobSetOnFloor = (OCVobSetOnFloor)0x0077D0E0;

	//.text:0077D130 public: int __thiscall oCVob::GetFloorPosition(class zVEC3 &) proc near
	typedef int (__thiscall* OCVobGetFloorPosition)(void*, zVEC3 &);
	OCVobGetFloorPosition oCVobGetFloorPosition = (OCVobGetFloorPosition)0x0077D130;

	while(!targetList.empty())
	{
		oCItem* item = targetList.front();
		targetList.pop_front();
		float x, y, z; 
		item->GetPositionWorld(x,y,z);
		zVEC3 pos(x,y,z);
		zVEC3 posForFloor(x, y + 200, z);
		//manager->setInstanceId(item, newId);
		int flags = item->flags;
		int amount = item->instanz;

		oCItem* item2 = factory->CreateItem(newId);
		//memcpy(item2->);
		item2->trafoObjToWorld = item->trafoObjToWorld;
		item2->trafoObjToWorld.m[0][3] = 0;
		item2->trafoObjToWorld.m[1][3] = 0;
		item2->trafoObjToWorld.m[2][3] = 0;

		zCVobRemoveVobSubtreeFromWorld(item);
		gameWorld->RemoveVob(item);
		item2->SetPositionWorld(pos);
		//item2->Move(pos.x, pos.y, pos.z);
		world->AddVob(item2);
		//item2->SetPositionWorld(pos);
		//item2->SetPositionWorld(posForFloor);
		//oCVobGetFloorPosition(item2, pos);
		//item2->SetPositionWorld(pos);
		
		//item2->SetCollDetStat(1);
		//item2->SetCollDetDyn(1);
		item2->flags = flags;
		item2->instanz = amount;
		zCObjectSetObjectName(item2, zSTRING("ITLSTORCHBURNING"));
		logStream << "exchanged item with id " << targetId << " with new item with id " << newId << std::endl;
		util::debug(&logStream);		
	};

	logStream << "DaedalusExports::DII_ChangeItemsInstanceId: done."<< std::endl;
	util::debug(&logStream);
}

void DaedalusExports::DII_ToggleLevitation()
{
	//toggle adjustHeroPosition
	Levitation::yPos = oCNpc::GetHero()->GetPositionWorld().y;
	Levitation::adjustHeroPosition = !Levitation::adjustHeroPosition;
}
