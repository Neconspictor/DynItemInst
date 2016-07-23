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
#include <TestModule.h>
#include <zCPar_SymbolTable.h>


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

	item = oCObjectFactory::GetFactory()->CreateItem(instanceId);

	// update the c_item
	symbol->offset = (int)item;
}

void __cdecl DaedalusExports::DII_ReleaseItem(int index) // Func void DII_ReleaseItem(var C_Item item, VAR INT instanceId)
{
	if (index <= 0) return;
	zCParser* parser = zCParser::GetParser();
	zCPar_Symbol* symbol = parser->GetSymbol(index);
	oCItem* item = (oCItem*)symbol->offset;

	if (item != nullptr)
	{
		int* refCtr = (int*)((BYTE*)item + 0x4);
		if (*refCtr >= 0)
		{
			oCGame::GetGame()->GetGameWorld()->RemoveVob(item);
		}
	}
}


int DaedalusExports::DII_CreateNewInstance(oCItem* item) //Func int DII_CreateNewInstance(var C_Item item)
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
		logStream << "DaedalusExports::DII_GetUserData: instanceId isn't dynamic" << std::endl;
		util::debug(&logStream, Logger::Warning);
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
				manager->oCItemSaveRemoveEffect(itm);
				itm->InitByScript(id, itm->instanz);
				itm->flags = flags;

				manager->oCItemSaveInsertEffect(itm);
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
	logStream << "DaedalusExports::DII_MarkAsReusable: function is enabled!" << std::endl;
	util::debug(&logStream);
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
		manager->oCItemSaveRemoveEffect(item2);
		manager->oCItemSaveInsertEffect(item2);
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



void DaedalusExports::DII_SetHeroFocusMode(int mode)
{
	//for now ignore!
	return;
	typedef void(__cdecl* OCNpcFocusSetFocusMode)(int mode);
	OCNpcFocusSetFocusMode oCNpcFocusSetFocusMode = (OCNpcFocusSetFocusMode)0x006BEC20;
	oCNpcFocusSetFocusMode(mode);
}

//.text:006C9030 void __cdecl Game_DeleteAllPfx(class zCTree<class zCVob> *) proc near
typedef void(__cdecl* Game_DeleteAllPfx)(void*);
Game_DeleteAllPfx game_DeleteAllPfx = (Game_DeleteAllPfx)0x006C9030;

//.text:00601C60 public: void __thiscall zCVob::RemoveVobSubtreeFromWorld(void) proc near
typedef void(__thiscall* ZCVobRemoveVobSubtreeFromWorld)(void*);
ZCVobRemoveVobSubtreeFromWorld zCVobRemoveVobSubtreeFromWorld = (ZCVobRemoveVobSubtreeFromWorld)0x00601C60;

//.text:00711930 public: void __thiscall oCItem::CreateVisual(void) proc near
typedef void(__thiscall* OCItemCreateVisual)(void*);
OCItemCreateVisual oCItemCreateVisual = (OCItemCreateVisual)0x00711930;


void DaedalusExports::DII_Test(oCItem* item, int mode)
{
	typedef void(__thiscall* OCItemInitByScript)(void* pThis, int, int);
	OCItemInitByScript oCItemInitByScript = (OCItemInitByScript)0x00711BD0;
	zVEC3 pos = item->GetVobPosition();
	void* pfxs = ((BYTE*)item) + 0xB8;

	// remove sub tree if visual is not valid!
	if (item->globalVobTreeNode)
	{
		logStream << "DaedalusExports::DII_Test(oCItem*): pfxs isn't null!" << std::endl;
		util::debug(&logStream, Logger::Warning);
		//game_DeleteAllPfx(item->globalVobTreeNode); //0x24
		oCItemCreateVisual(item);
		zCVobRemoveVobSubtreeFromWorld(item);
		item->InitByScript(item->GetInstance(), mode);
		item->SetPositionWorld(pos);
		oCGame::GetGame()->GetWorld()->AddVob(item);
		
	} else
	{
		logStream << "DaedalusExports::DII_Test(oCItem*): pfxs is null!"  << std::endl;
		util::debug(&logStream, Logger::Warning);
		item->InitByScript(item->GetInstance(), mode);
		item->SetPositionWorld(pos);
		oCGame::GetGame()->GetWorld()->AddVob(item);
	}

	logStream << "DII_Test(oCItem*) Called for item: " << item->name.ToChar() << ", " << item->GetSchemeName().ToChar() << std::endl;
	util::debug(&logStream, Logger::Warning);
}

//.text:007929F0 public: void * __cdecl zCParser::CallFunc(int, ...) proc near
typedef void* (__cdecl* ZCParserCallFunc)(int, ...);
ZCParserCallFunc zCParserCallFunc = (ZCParserCallFunc)0x007929F0;

//.text:007A4BB0 public: void __thiscall zCPar_Stack::PushString(class zSTRING &) proc near
typedef void(__thiscall* ZCPar_StackPushString)(void* pThis, zSTRING &);
ZCPar_StackPushString zCPar_StackPushString = (ZCPar_StackPushString)0x007A4BB0;

//.text:006E2BA0 ; int __cdecl sub_006E2BA0()
typedef int(__cdecl* PrintExternal)();
PrintExternal printExternal = (PrintExternal)0x006E2BA0;

//.text:007A4B90 public: void __thiscall zCPar_Stack::PushInt(int) proc near
typedef void(__thiscall* ZCPar_StackPushInt)(void* pThis, int);
ZCPar_StackPushInt zCPar_StackPushInt = (ZCPar_StackPushInt)0x007A4B90;

//.text:007A5180 public: void __thiscall zCPar_DataStack::Clear(void) proc near
typedef void(__thiscall* ZCPar_StackClear)(void* pThis);
ZCPar_StackClear zCPar_StackClear = (ZCPar_StackClear)0x007A5180;

//.text:007A4F80 public: void __thiscall zCPar_DataStack::Push(int) proc near
typedef void(__thiscall* ZCPar_StackPush)(void* pThis, int);
ZCPar_StackPush zCPar_StackPush = (ZCPar_StackPush)0x007A4F80;

//.text:007A5070 public: int __thiscall zCPar_DataStack::Pop(void) proc near
typedef int(__thiscall* ZCPar_DataStackPop)(void* pThis);
ZCPar_DataStackPop zCPar_DataStackPop = (ZCPar_DataStackPop)0x007A5070;

//.text:007A1E10 public: void __thiscall zCPar_Symbol::GetStackPos(int &, int) proc near
typedef void(__thiscall* ZCPar_SymbolGetStackPos)(void* pThis, int&, int);
ZCPar_SymbolGetStackPos zCPar_SymbolGetStackPos = (ZCPar_SymbolGetStackPos)0x007A1E10;

//.text:00791960 private: void __thiscall zCParser::DoStack(int) proc near
typedef void(__thiscall* ZCParserDoStack)(void* pThis, int);
ZCParserDoStack zCParserDoStack = (ZCParserDoStack)0x00791960;

zSTRING* msg = new zSTRING("DaedalusExport Test!");

void DaedalusExports::DII_TransformationTest(zCVob* vob)
{
	return;
	//TestModule::Test(vob);
	for (auto it = TestModule::vobList.begin(); it != TestModule::vobList.end();)
	{
		if (*it == vob)
		{
			it = TestModule::vobList.erase(it);
		} else
		{
			++it;
		}
	}
	TestModule::vobList.push_back(vob);

	zCParser* parser = zCParser::GetParser();
	int index = parser->GetIndex(zSTRING("PRINT_TEST3")); 
	logStream << "index: " << index << std::endl;
	Logger::getLogger()->log(Logger::Warning, &logStream);
	bool isExternal = false;
	bool hasReturn = true;
	void* dataStack = (BYTE*)parser + 0x58;
	
	g2ext_extended::zCPar_SymbolTable* currSymbolTable = ObjectManager::zCParserGetSymbolTable(parser);

	zCPar_Symbol* symbol = currSymbolTable->GetSymbol(index);
	int stackPosition = 0; 
	*(int*)((BYTE*)parser + 0x219C) = index;
	zCPar_SymbolGetStackPos(symbol, stackPosition, 0);

	logStream << "stackPosition: " << stackPosition << std::endl;
	logStream << "symbol->name.ToChar(): " << symbol->name.ToChar() << std::endl;
	logStream << "symbol->bitfield: " << symbol->bitfield << std::endl;
	logStream << "symbol->filenr: " << symbol->filenr << std::endl;
	logStream << "symbol->line: " << symbol->line << std::endl;
	logStream << "symbol->line_anz: " << symbol->line_anz << std::endl;
	logStream << "symbol->offset: " << symbol->offset << std::endl;
	logStream << "symbol->next: " << symbol->next << std::endl;
	logStream << "symbol->parent: " << symbol->parent << std::endl;
	logStream << "symbol->pos_beg: " << symbol->pos_beg << std::endl;
	logStream << "symbol->pos_anz: " << symbol->pos_anz << std::endl;
	logStream << "symbol->content.data_int: " << symbol->content.data_int << std::endl;
	Logger::getLogger()->log(Logger::Warning, &logStream);

	zCPar_StackClear(dataStack);

	//zCPar_StackPush(dataStack,(int)msg);
	//zCPar_StackPush(dataStack, 3);
	
	if (isExternal)
	{
		typedef int(__cdecl* External)();
		External external = (External)stackPosition;
		external();
	} else
	{
		zCParserDoStack(parser, stackPosition);
	}
	
	if (!hasReturn)
	{
		return;
	}

	// pop return value
	int result = zCPar_DataStackPop(dataStack);
	for (int i = 0; i < 1; ++i)
	{
		result = zCPar_DataStackPop(dataStack);
	}
	
	*(int*)((BYTE*)parser + 0x219C) = -1;
	//result = *((int*)zCParserCallFunc((int)parser, index));


	logStream << "result: " << result << std::endl;
	//logStream << "[result]: " << *(int*)result << std::endl;
	Logger::getLogger()->log(Logger::Warning, &logStream);
	//zCPar_Symbol symbol;
	//symbol.content.data_pstring = msg;

	//zCParserCallFunc((int)parser, index, msg);
	
	//zCPar_StackClear(dataStack);

	//zCPar_Symbol symbol;
	//symbol.content.data_pstring = msg;

	//parser->CallFunc(zSTRING("PRINT"));
	//parser->CallFunc(index, msg, msg, msg);
	//zCPar_StackPushString(dataStack, *msg);
	//zSTRING test;
	
	//parser->GetParameter(test);
	//logStream << "test: " << test.ToChar() << std::endl;
	//Logger::getLogger()->log(Logger::Warning, &logStream);
	//printExternal();
}