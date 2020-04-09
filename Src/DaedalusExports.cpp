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
#include <api/g2/oCItemExtended.h>
#include <ObjectManager.h>
#include <api/g2/oCObjectFactory.h>
#include <Logger.h>
#include <DaedalusExports.h>
#include <set>
#include <ocgameExtended.h>
#include <DynItemInst.h>
#include <Util.h>
#include <Levitation.h>
#include <zCPar_SymbolTable.h>
#include <api/g2/ocmob.h>


const float DaedalusExports::LIB_VERSION = 1.02f;

std::vector<std::unique_ptr<TelekinesisInterpolator>> DaedalusExports::interpolators;

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


oCItem* __cdecl DaedalusExports::DII_CreateNewItem(int instanceId) // Func void DII_CreateNewItem(var C_Item item, VAR INT instanceId)
{

	oCItem* item = oCObjectFactory::GetFactory()->CreateItem(instanceId);

	logStream << "DII_CreateNewItem: called! " << instanceId;
	util::logWarning(&logStream);

	return item;
}

void DaedalusExports::DII_DeleteItem(oCItem* item)
{
	if (item == NULL) return;

	int* refCtr = (int*)((BYTE*)item + 0x4);
	logStream << "DII_DeleteItem:  refCtr: " << *refCtr;
	util::logWarning(&logStream);

	if (*refCtr >= 0)
	{
		oCGame::GetGame()->GetGameWorld()->RemoveVob(item);
	}

	ObjectManager::oCItemOperatorDelete(item);
}


int DaedalusExports::DII_CreateNewInstance(oCItem* item) //Func int DII_CreateNewInstance(var C_Item item)
{
	if (item == NULL) {return NULL;}

	logStream << "Param: " << item->name.ToChar();
	util::debug(&logStream);

	// Create new instance with item
	ObjectManager* manager = ObjectManager::getObjectManager();

	const std::string instanceName = "DII_" + std::to_string(*manager->getParserInstanceCount());

	int parserSymbolIndex = manager->createNewInstanceId(item, instanceName);
	if (!parserSymbolIndex) {
		logStream << "DII_CreateNewInstanceStr2: Couldn't create new instance '" << instanceName << "'" << std::endl;
		util::logWarning(&logStream);
	}

	int index = manager->getDynInstanceId(item);
	logStream << "Index: " << index << std::endl;
	util::debug(&logStream);

	logStream << "CreateNewInstance::key: " << parserSymbolIndex << std::endl;
	util::debug(&logStream);
	return parserSymbolIndex;
}

zSTRING* DaedalusExports::DII_CreateNewInstanceStr(oCItem* item)
{
	const auto parserSymbolIndex = DII_CreateNewInstance(item);
	auto* symbol = zCParser::GetParser()->GetSymbol(parserSymbolIndex);
	return &symbol->name;
}

int DaedalusExports::DII_CreateNewInstanceStr2(oCItem* item, const char* instanceName)
{
	if (!instanceName || !item) {
		return false;
	}

	std::string instanceNameStr = instanceName;
	std::transform(instanceNameStr.begin(), instanceNameStr.end(), instanceNameStr.begin(), std::toupper);


	logStream << "Param: " << item->name.ToChar();
	util::debug(&logStream);

	// Create new instance with item
	ObjectManager* manager = ObjectManager::getObjectManager();

	int parserSymbolIndex = manager->createNewInstanceId(item, instanceName);
	if (!parserSymbolIndex) {
		logStream << "DII_CreateNewInstanceStr2: Couldn't create new instance '" << instanceName << "'" << std::endl;
		util::logWarning(&logStream);
	}


	int index = manager->getDynInstanceId(item);
	logStream << "Index: " << index << std::endl;
	util::debug(&logStream);

	logStream << "DII_CreateNewInstanceStr2: parser symbol index for instance = " << parserSymbolIndex << std::endl;
	util::debug(&logStream);
	return parserSymbolIndex !=0;
}

void DaedalusExports::DII_EquipItem(oCNpc* npc, int instanceId)
{
	if (npc == NULL) return;
	oCNpcInventory* inv = npc->GetInventory();
	if (inv == NULL) return;

	oCItem* item = DynItemInst::getInvItemByInstanceId2(inv, instanceId);
	if (item == NULL) return;
	npc->Equip(item);
}

int DaedalusExports::DII_IsDynamic(oCItem* item) // Func DII_IsDynamic(VAR C_ITEM item)
{
	if (item == NULL) {return FALSE;}

	bool modified = ObjectManager::getObjectManager()->IsModified(item);
	if (modified)
	{
		return TRUE;
	}
	return FALSE;
}

int DaedalusExports::DII_IsInstanceDynamic(int parserSymbolIndex)
{
	bool modified = ObjectManager::getObjectManager()->isDynamicInstance(parserSymbolIndex);
	return modified;
}

BYTE* DaedalusExports::DII_GetUserData(int instanceId) // Func DII_UserData DII_GetUserData(var int instanceId)
{
	ObjectManager* manager = ObjectManager::getObjectManager();

	if (!manager->IsModified(instanceId))
	{
		logStream << "DaedalusExports::DII_GetUserData: instanceId isn't dynamic" << std::endl;
		util::debug(&logStream, Logger::Warning);
		return NULL;
	}

	DynInstance* storeItem = manager->getInstanceItem(instanceId);
	return storeItem->getUserData();
}


float DaedalusExports::DII_GetLibVersion()
{
	return LIB_VERSION;
}


struct UPDATE_INSTANCE_PARAMS {
	int index;
	oCItem* item;
};

static void updateItem(void* obj, void* param, oCItem* itm) {
	if (itm == NULL) return;

	UPDATE_INSTANCE_PARAMS* params = (UPDATE_INSTANCE_PARAMS*)param;
	ObjectManager* manager = ObjectManager::getObjectManager();
	int id = manager->getDynInstanceId(itm);
	if (id == params->index)
	{
		//int refCtr = *(int*)((BYTE*)itm + 0x4);

		bool isInWorld = manager->isItemInWorld(itm);
		int flags = itm->flags;
		manager->oCItemSaveRemoveEffect(itm);
		itm->InitByScript(id, itm->amount);
		itm->flags = flags;

		manager->oCItemSaveInsertEffect(itm);
		//itm->InsertEffect();

		if (isInWorld)
		{
			zCWorld* world = oCGame::GetGame()->GetWorld();
			world->AddVob(params->item);
		}
	}
}

void DaedalusExports::DII_UpdateInstance(oCItem* item)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	int index = manager->getDynInstanceId(item);
	if (index > 0)
	{
		DynInstance* dynInstance = manager->getInstanceItem(index);
		if (dynInstance == NULL)
		{
			logStream << "DII_UpdateInstance: dynInstance is null!" << std::endl;
			util::logWarning(&logStream);
			return;
		}
		dynInstance->store(*item);

		zCWorld* world = oCGame::GetGame()->GetWorld();

		UPDATE_INSTANCE_PARAMS params = { index, item };
		manager->callForAllItems(updateItem, NULL, &params);

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

	zCListSort<oCItem>* itemList = world->GetItemList();
	while (itemList != NULL) {
		oCItem* item = itemList->GetData();
		if (item != NULL)
		{
			if (item->GetInstance() == targetId && (item->amount != 666))
			{
				targetList.push_back(item);
			}
		}
		itemList = itemList->GetNext();
	}


	//.text:0073ABE0 public: class oCVob * __thiscall oCNpc::GetLeftHand(void) proc near
	typedef oCVob*(__thiscall* OCNpcGetLeftHand)(oCNpc*);
	OCNpcGetLeftHand oCNpcGetLeftHand = (OCNpcGetLeftHand)0x0073ABE0;

	// don't consider items located in an npc's inventory
	zCListSort<oCNpc>* npcList = world->GetNpcList();

	while (npcList != NULL) {
		oCNpc* npc = npcList->GetData();
		if (npc == NULL) {
			npcList = npcList->GetNext();
			continue;
		}

		oCVob* leftHanfVob = oCNpcGetLeftHand(npc);
		oCItem* item = dynamic_cast<oCItem*>(leftHanfVob);
		if (item)
		{
			std::list<oCItem*>::iterator findIter = find(targetList.begin(), targetList.end(), item);
			if (findIter != targetList.end())
			{
				logStream << "Found item in npc's left hand that should not be considered!" << std::endl;
				util::debug(&logStream);
				targetList.erase(findIter);
			}
		}

		oCNpcInventory* inventory = npc->GetInventory();
		if (inventory == NULL) {
			npcList = npcList->GetNext();
			continue;
		}

		inventory->UnpackAllItems();
		zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
		while (list != NULL) {
			oCItem* item = list->GetData();
			if (item != NULL)
			{
				std::list<oCItem*>::iterator findIter = find(targetList.begin(), targetList.end(), item);
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
		int amount = item->amount;

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
		item2->amount = amount;
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

//.text:006BED00 ; public: static class zSTRING __cdecl oCNpcFocus::GetFocusName(void)
typedef zSTRING(__cdecl* OCNpcFocusGetFocusName)(); OCNpcFocusGetFocusName oCNpcFocusGetFocusName = (OCNpcFocusGetFocusName)0x006BED00;

void DaedalusExports::DII_TransformationTest(zCVob* vob)
{
	return;
}

void DaedalusExports::DII_TelekineseTest()
{
	zSTRING focusName = oCNpcFocusGetFocusName();
	logStream << "DII_TelekineseTest(): focusName = " << focusName.ToChar() << std::endl;
	util::debug(&logStream, Logger::Warning);
}


bool DaedalusExports::DII_Npc_CanTalk(oCNpc* npc)
{
	//.text:006BCEF0 ; public: int __thiscall oCNpc::CanTalk(void)
	typedef bool(__thiscall* OCNpcCanTalk)(oCNpc* pThis); OCNpcCanTalk oCNpcCanTalk = (OCNpcCanTalk)0x006BCEF0;
	return oCNpcCanTalk(npc);
}

TelekinesisInterpolator* DaedalusExports::DII_Telekinesis_createInterpolator(const zVEC3* vobPosition, const zVEC3* npcPosition,
	int upMoveAmount, int speed)
{

	logStream << "DII_Telekinesis_createInterpolator(): vobPosition = " << *vobPosition << std::endl;
	logStream << "DII_Telekinesis_createInterpolator(): npcPosition = " << *npcPosition << std::endl;
	logStream << "DII_Telekinesis_createInterpolator(): upMoveAmount = " << upMoveAmount << std::endl;
	logStream << "DII_Telekinesis_createInterpolator(): speed = " << speed << std::endl;
	util::logAlways(&logStream);


	std::unique_ptr<TelekinesisInterpolator> interpolator = TelekinesisInterpolator::createTelekinesisInterpolator(*vobPosition, *npcPosition, upMoveAmount, speed);

	interpolators.emplace_back(std::move(interpolator));

	return interpolators.back().get();
}

void DaedalusExports::DII_Telekinesis_GetInterpolatedVec(TelekinesisInterpolator* interpolatorPtr, zVEC3* dest)
{
	zVEC3 result = interpolatorPtr->interpolate(std::chrono::system_clock::now());
	*dest = result;
}

void DaedalusExports::DII_Telekinesis_deleteInterpolator(TelekinesisInterpolator* interpolatorPtr)
{

	auto newEnd = std::remove_if(interpolators.begin(), interpolators.end(), [&](auto& it)
	{
		return it.get() == interpolatorPtr;
	});

	if (newEnd != interpolators.end())
	{
		logStream << "DII_Telekinesis_createInterpolator(): successfully removed interpolator!" << std::endl;
		util::logAlways(&logStream);
	}

	interpolators.erase(newEnd, interpolators.end());
}

void DaedalusExports::DII_Telekinesis_Interpolate(TelekinesisInterpolator* interpolatorPtr, oCItem* item)
{
	//logStream << "DII_Telekinesis_Interpolate(): called!" << std::endl;
	//util::logAlways(&logStream);
	zVEC3 result = interpolatorPtr->interpolate(std::chrono::system_clock::now());
	zVEC3 current = item->GetVobPosition();

	zVEC3 diff = result - current;

	//logStream << "DII_Telekinesis_Interpolate(): diff = " << diff << std::endl;
	//util::logAlways(&logStream);


	//item->SetCollDet(1);
	//item->SetCollDetDyn(1);
	//item->SetCollDetStat(1);
	item->Move(diff.x, diff.y, diff.z);
	//item->SetPositionWorld(result);
}

int DaedalusExports::DII_Npc_CanSeeVob(oCNpc* npc, zCVob* vob)
{
	//.text:00741C10 ; public: int __thiscall oCNpc::CanSee(class zCVob *, int)
	using OCNpcCanSee = int(__thiscall*)(void*, void*, int);
	OCNpcCanSee oCNpcCanSee = (OCNpcCanSee)0x00741C10;


	//.text:00621FA0 ; public: int __fastcall zCWorld::TraceRayNearestHit(class zVEC3 const &, class zVEC3 const &, class zCVob *, int)
	using ZCWorldTraceRayNearestHit = int(__fastcall*)(void*, zVEC3 const&, zVEC3 const&, void*, int);
	ZCWorldTraceRayNearestHit zCWorldTraceRayNearestHit = (ZCWorldTraceRayNearestHit)0x00621FA0;

	const int zTraceRay_vob_ignore_no_cd_dyn = 1 << 0;  // Ignore vobs without collision
	const int zTraceRay_vob_bbox = 1 << 2;  // Intersect bounding boxes (important to detect NPCs)
	const int zTraceRay_poly_normal = 1 << 7;  // Report normal vector of intersection
	const int zTraceRay_poly_ignore_transp = 1 << 8;  // Ignore alpha polys (without this, trace ray is bugged)
	const int zTraceRay_poly_test_water = 1 << 9;  // Intersect water
	const int zTraceRay_vob_ignore_projectiles = 1 << 14; // Ignore projectiles
	const int zTRACERAY_VOB_IGNORE = 1 << 1; //Alle Vobs ignorieren (nur statisches Mesh beachten)

	const int zTRACERAY_VOB_IGNORE_CHARACTER = 1 << 11; //Ignoriere Npcs



	int found = oCNpcCanSee(npc, vob, 0);

	zCWorld* world = oCGame::GetGame()->GetWorld();
	int traceFlags = zTraceRay_vob_ignore_no_cd_dyn
					//| zTRACERAY_VOB_IGNORE
					| zTRACERAY_VOB_IGNORE_CHARACTER
					| zTraceRay_poly_ignore_transp 
					| zTraceRay_vob_ignore_projectiles 
					| zTraceRay_poly_test_water;

	zVEC3 direction = vob->GetVobPosition() - npc->GetPosition();
	float length = direction.Length();
	if (length > 0.001)
	{
		//direction /= length;
	}

	zVEC3 npcPosition = npc->GetPosition();


	int result = zCWorldTraceRayNearestHit(world, npcPosition, direction, npc, traceFlags);

	for(int i = 0; i < world->traceRayVobList.GetSize(); ++i)
	{
		zCVob* vob = world->traceRayVobList[i];
		if (vob != nullptr)
		{
			switch (vob->GetVobType())
			{
			case VOB_TYPE_MOB:
				{
					oCMob * mob = (oCMob*)vob;
					logStream << "vob is mob: " << mob->name.ToChar() << std::endl;
					util::logAlways(&logStream);
					break;
				}
			case VOB_TYPE_ITEM:
				{
					oCItem * item = (oCItem*)vob;
					logStream << "vob is item: " << item->name.ToChar() << std::endl;
					util::logAlways(&logStream);
					break;
				}
			case VOB_TYPE_NPC:
				{
					oCNpc * npc2 = (oCNpc*)vob;
					logStream << "vob is npc: " << npc2->name[0].ToChar() << std::endl;
					util::logAlways(&logStream);
					break;
				}
			default: ;
			}
			vob->GetVobType();
		}
	}

	//int found = !result;

	if (result && !found)
	{
		float intersectLength = (world->foundIntersection - npcPosition).Length();
		float VobLength = direction.Length();

		zCVob* foundVob = world->foundVob;

		//found = (intersectLength + 20 < VobLength) ? 0 : 1;

		found = (world->foundVob == vob) ? 1 : 0;

		if (!found && foundVob != nullptr)
		{
			
			static const int zCVob_bitfield0_collDetectionStatic = ((1 << 1) - 1) << 6;
			static const int zCVob_bitfield0_collDetectionDynamic = ((1 << 1) - 1) << 7;

			bool hasStaticCollision = foundVob->bitfield[0] & zCVob_bitfield0_collDetectionStatic;
			bool hasDynamicCollision = foundVob->bitfield[0] & zCVob_bitfield0_collDetectionDynamic;

			logStream << "foundVob has static collision detection on: " << hasStaticCollision << std::endl;
			logStream << "foundVob has dynamic collision detection on: " << hasDynamicCollision << std::endl;
			util::logAlways(&logStream);

			//int testCanSee = oCNpcCanSee(npc, vob, 0);
			//logStream << "testCanSee: " << testCanSee << std::endl;
			//util::logAlways(&logStream);
		}

		if (!found)
			found = (intersectLength + 20 < VobLength) ? 0 : 1;
	}

	return found;

	//return oCNpcCanSee(npc, vob, 0);
}

struct zCAIVobMove
{
	//zCObject {
	int    _vtbl;				// 4 bytes
	int    _zCObject_refCtr;   // 4 bytes
	char data[52];  //size = 0x3C
};

struct zCClassDef {

	zSTRING className;            //zSTRING
	zSTRING baseClassName;        //zSTRING
	zSTRING scriptClassName;      //zSTRING
	int baseClassDef;            //zCClassDef* //davon abgeleitet

	int createNewInstance;       //zCObject* ( *) (void) //Pointer auf klassenspezifische Funktion
	int createNewInstanceBackup; //zCObject* ( *) (void) //Pointer auf klassenspezifische Funktion

									 /*
									 enum zTClassFlags {
									 zCLASS_FLAG_SHARED_OBJECTS      = 1<<0, //Mehrfach benutzt Objekte (wie Visuals zum Beispiel)
									 zCLASS_FLAG_TRANSIENT           = 1<<1, //Flüchtig, soll nicht gespeichert werden.
									 zCLASS_FLAG_RESOURCE            = 1<<2, //keine Ahnung / vermutlich irrelevant
									 };*/

	int classFlags;              //zDWORD //siehe enum
	int classSize;               //zDWORD //Größe in Bytes

	int numLivingObjects;        //Anzahl Objekte von dieser Klasse
	int numCtorCalled;           //Konstruktor wurde sooft aufgerufen

	int hashTable;               //zCObject** //Hashtabelle der Größe 1024. Objekte sind mit zCObject.hashNext verknüpft, falls mehrere auf den selben Wert hashen.
									 //zCArray<zCObject*> objectList;    //alle benannten (!) Objekte von genau (!) dieser Klasse (!) //Ausrufezeichenanmerkungen: 1.) unbenannte sind nicht drin 2.) Objekte von Unterklassen sind nicht drin 3.) diese Eigenschaft kann sehr nützlich sein.
	int objectList_array;       //zCObject**
	int objectList_numAlloc;    //int
	int objectList_numInArray;  //int

	int archiveVersion;          //zWORD //vermutlich nutzlos
	int archiveVersionSum;       //zWORD //vermutlich nutzlos
};

void DaedalusExports::DII_DrobVob(oCNpc* npc, zCVob* vob)
{
	logStream << "Called DII_DrobVob()!" << std::endl;
	logStream << "npc = " << npc->GetName().ToChar() << std::endl;
	util::logAlways(&logStream);

	/*

	//.text:00565F50 ; void *__cdecl operator new(size_t)
	using GothicNewOperator = void* (__cdecl*)(std::size_t size);
	GothicNewOperator gothicNewOperator = (GothicNewOperator)0x00565F50;

	//.data:00AAD720; private: static class zCClassDef oCAIVobMove::classDef
	using ClassDefPtr = void*; 

	//.text:005AAEB0 ; public: static void __cdecl zCClassDef::ObjectCreated(class zCObject *, class zCClassDef *)
	using ZCClassDefObjectCreated = void(__cdecl*)(void* object, ClassDefPtr classDef);
	ZCClassDefObjectCreated zCClassDefObjectCreated = (ZCClassDefObjectCreated)0x005AAEB0;

	//.text:0069F220 ; public: __thiscall oCAIVobMove::oCAIVobMove(void)
	using OCAIVobMoveConstructor = void(__thiscall*)(zCAIVobMove* aiVobMove);
	OCAIVobMoveConstructor oCAIVobMoveConstructor = (OCAIVobMoveConstructor)0x0069F220;

	using zCAIBase = void*;

	//.text:005FE8F0 ; public: void __thiscall zCVob::SetAI(class zCAIBase *)
	using ZCVobSetAI = void(__thiscall*)(zCVob* vob, zCAIBase* base);
	ZCVobSetAI zCVobSetAI = (ZCVobSetAI)0x005FE8F0;

	//.text:0069F540 ; public: virtual void __thiscall oCAIVobMove::Init(class zCVob *, class zCVob *, class zVEC3 &, float, float, class zMAT4 *)
	using OCAIVobMoveInit = void(__thiscall*)(zCAIVobMove* pThis, zCVob* target, zCVob* parent, zVEC3& vec, float val1, float val2, zMAT4* matrix);
	OCAIVobMoveInit oCAIVobMoveInit = (OCAIVobMoveInit)0x0069F540;


	zVEC3 oldPosition = vob->GetVobPosition();

	zCAIVobMove* aiVobMove = (zCAIVobMove*)gothicNewOperator(60);
	ClassDefPtr oCAIVobMoveclassDef = (ClassDefPtr)0x00AAD720;
	zCClassDefObjectCreated(aiVobMove, oCAIVobMoveclassDef);
	oCAIVobMoveConstructor(aiVobMove);
	zCVobSetAI(vob, (zCAIBase*)aiVobMove);

	aiVobMove->_zCObject_refCtr -= 1;

	zVEC3 vec;
	zMAT4 matrix(1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1);
	oCAIVobMoveInit(aiVobMove, vob, npc, vec, 0.0f, 100.0f, &npc->trafoObjToWorld);

	vob->SetPositionWorld(oldPosition);
	vob->trafoObjToWorld.m[0][3] = oldPosition.x;
	vob->trafoObjToWorld.m[1][3] = oldPosition.y;
	vob->trafoObjToWorld.m[2][3] = oldPosition.z;


	return;
	*/


	//.data:00AAD840 ; private: static class zCClassDef oCAIDrop::classDef
	zCClassDef* classDef = (zCClassDef*)0x00AAD840;

	logStream << "classDef->className = " << classDef->className.ToChar() << std::endl;
	logStream << "classDef->baseClassName = " << classDef->baseClassName.ToChar() << std::endl;
	logStream << "classDef->classSize = " << classDef->classSize << std::endl;
	util::logAlways(&logStream);


	//.text:00602930 ; public: void __thiscall zCVob::SetSleeping(int)
	using ZCVobSetSleeping = void(__thiscall*)(void* pThis, int);
	ZCVobSetSleeping zCVobSetSleeping = (ZCVobSetSleeping)0x00602930;

	//.text:0061D190 ; public: void __thiscall zCVob::SetPhysicsEnabled(int)
	using ZCVobSetPhysicsEnabled = void(__thiscall*)(void* pThis, int);
	ZCVobSetPhysicsEnabled zCVobSetPhysicsEnabled = (ZCVobSetPhysicsEnabled)0x0061D190;

	//.text:005FE960; public: class zCRigidBody * __thiscall zCVob::GetRigidBody(void)
	using zCRigidBodyPtr = void*;
	using ZCVobGetRigidBody = zCRigidBodyPtr(__thiscall*)(void* pThis);
	ZCVobGetRigidBody zCVobGetRigidBody = (ZCVobGetRigidBody)0x005FE960;

	//.text:005B66D0 ; public: void __thiscall zCRigidBody::SetVelocity(class zVEC3 const &)
	using ZCRigidBodySetVelocity = void(__thiscall*)(zCRigidBodyPtr pThis, zVEC3 const& velocity);
	ZCRigidBodySetVelocity zCRigidBodySetVelocity = (ZCRigidBodySetVelocity)0x005B66D0;
	


	zCVobSetSleeping(vob, 0);
	zCVobSetPhysicsEnabled(vob, 1);
	//zCRigidBodyPtr rigidBody = zCVobGetRigidBody(vob);
	//zCRigidBodySetVelocity(rigidBody, zVEC3(0, -1,0));
}