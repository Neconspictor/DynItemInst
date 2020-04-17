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
#include <DII.h>
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

bool DaedalusExports::DII_AddProxy(const char* sourceInstanceName, const char* targetInstanceName)
{
	return ObjectManager::getObjectManager()->addProxy(sourceInstanceName, targetInstanceName);
}

void DaedalusExports::DII_RemoveProxy(const char* sourceInstanceName)
{
	ObjectManager::getObjectManager()->removeProxy(sourceInstanceName);
}


oCItem* __cdecl DaedalusExports::DII_CreateNewItem(int instanceId) // Func void DII_CreateNewItem(var C_Item item, VAR INT instanceId)
{

	oCItem* item = oCObjectFactory::GetFactory()->CreateItem(instanceId);

	logStream << "DII_CreateNewItem: called! " << instanceId;
	util::logWarning(&logStream);

	return item;
}

void __cdecl DaedalusExports::DII_DeleteDII(int parserSymbolIndex)
{
	auto* manager = ObjectManager::getObjectManager();

	try {
		manager->deleteDII(parserSymbolIndex);
	}
	catch (const std::invalid_argument& e) {
		logStream << e.what() << std::endl;
		util::logWarning(&logStream);
	}
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


	int parserSymbolIndex = 0;

	while (!parserSymbolIndex) {
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

		const std::string instanceName = "DII_*" + std::to_string(util::generateRandom(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()));

#pragma pop_macro("max")
#pragma pop_macro("min")

		parserSymbolIndex = manager->createNewInstanceId(item, instanceName);
		if (!parserSymbolIndex) {
			logStream << "DII_CreateNewInstanceStr2: Couldn't create new instance '" << instanceName << "'" << std::endl;
			util::logWarning(&logStream);
		}
	}

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

int DaedalusExports::DII_IsDynamic(oCItem* item) // Func DII_IsDynamic(VAR C_ITEM item)
{
	if (item == NULL) {return FALSE;}

	bool modified = ObjectManager::getObjectManager()->isAssignedToDII(item);
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

BYTE* DaedalusExports::DII_GetUserData(int instanceIdParserSymbolIndex) // Func DII_UserData DII_GetUserData(var int instanceId)
{
	ObjectManager* manager = ObjectManager::getObjectManager();

	if (!manager->isDynamicInstance(instanceIdParserSymbolIndex))
	{
		logStream << "DaedalusExports::DII_GetUserData: instanceId isn't dynamic" << std::endl;
		util::debug(&logStream, Logger::Warning);
		return NULL;
	}

	DynInstance* storeItem = manager->getInstanceItem(instanceIdParserSymbolIndex);
	return storeItem->getUserData();
}


float DaedalusExports::NECPACK_GetLibVersion()
{
	return LIB_VERSION;
}

bool DaedalusExports::DII_UpdateInstance(const char* instanceName, oCItem* item)
{
	ObjectManager* manager = ObjectManager::getObjectManager();

	auto instanceIdParserSymbolIndex = manager->getUnProxiedInstanceID(instanceName);

	DynInstance* dynInstance = manager->getInstanceItem(instanceIdParserSymbolIndex);

	if (!item) {
		logStream << "DII_UpdateInstance: item mustn't be null " << std::endl;
		util::logWarning(&logStream);
		return false;
	}

	if (!dynInstance)
	{
		logStream << "DII_UpdateInstance: dynInstance not found for instance id " << instanceIdParserSymbolIndex  << std::endl;
		util::logWarning(&logStream);
		return false;
	}
	dynInstance->store(*item);

	zCWorld* world = oCGame::GetGame()->GetWorld();

	ItemUpdater::UpdateItemData params = { instanceIdParserSymbolIndex, instanceIdParserSymbolIndex };
	manager->callForAllItems(ItemUpdater::updateItem, NULL, &params);

	return true;
}

void DaedalusExports::DII_AssignInstanceId(oCItem* item, int instanceIdParserSymbolIndex)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->assignInstanceId2(item, instanceIdParserSymbolIndex);
}

void DaedalusExports::DII_GetItemByInstanceId(int itemParserSymbolIndex,  int instanceIdParserSymbolIndex)
{
	if (itemParserSymbolIndex <= 0) return;
	zCParser* parser = zCParser::GetParser();
	zCPar_Symbol* symbol = parser->GetSymbol(itemParserSymbolIndex);
	oCItem* item = (oCItem*)symbol->offset;

	// Check if provided instance id is valid
	zCPar_Symbol* instanceSym = parser->GetSymbol(instanceIdParserSymbolIndex);

	if (!instanceSym)
	{
		logStream << "DaedalusExports::DII_GetItemByInstanceId instanceSym is Null! No item will be searched!" << std::endl;
		util::logWarning(&logStream);
		return;
	}


	ObjectManager* manager = ObjectManager::getObjectManager();
	item = manager->getItemByInstanceId(instanceIdParserSymbolIndex);

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


//.text:006C9030 void __cdecl Game_DeleteAllPfx(class zCTree<class zCVob> *) proc near
typedef void(__cdecl* Game_DeleteAllPfx)(void*);
Game_DeleteAllPfx game_DeleteAllPfx = (Game_DeleteAllPfx)0x006C9030;

//.text:00601C60 public: void __thiscall zCVob::RemoveVobSubtreeFromWorld(void) proc near
typedef void(__thiscall* ZCVobRemoveVobSubtreeFromWorld)(void*);
ZCVobRemoveVobSubtreeFromWorld zCVobRemoveVobSubtreeFromWorld = (ZCVobRemoveVobSubtreeFromWorld)0x00601C60;

//.text:00711930 public: void __thiscall oCItem::CreateVisual(void) proc near
typedef void(__thiscall* OCItemCreateVisual)(void*);
OCItemCreateVisual oCItemCreateVisual = (OCItemCreateVisual)0x00711930;

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


void DaedalusExports::DII_ChangeItemsInstance(const char* sourceName, const char* targetName)
{
	ObjectManager* manager = ObjectManager::getObjectManager();

	auto sourceID = manager->getUnProxiedInstanceID(sourceName);
	auto targetID = manager->getUnProxiedInstanceID(targetName);

	ItemUpdater::UpdateItemData params = { sourceID, targetID };
	manager->callForAllItems(ItemUpdater::updateItemInstance, NULL, &params);
}

TelekinesisInterpolator* DaedalusExports::TELEKINESIS_CreateInterpolator(const zVEC3* vobPosition, const zVEC3* npcPosition,
	int upMoveAmount, int speed)
{

	logStream << "TELEKINESIS_CreateInterpolator(): vobPosition = " << *vobPosition << std::endl;
	logStream << "TELEKINESIS_CreateInterpolator(): npcPosition = " << *npcPosition << std::endl;
	logStream << "TELEKINESIS_CreateInterpolator(): upMoveAmount = " << upMoveAmount << std::endl;
	logStream << "TELEKINESIS_CreateInterpolator(): speed = " << speed << std::endl;
	util::debug(&logStream);


	std::unique_ptr<TelekinesisInterpolator> interpolator = TelekinesisInterpolator::createTelekinesisInterpolator(*vobPosition, *npcPosition, upMoveAmount, speed);

	interpolators.emplace_back(std::move(interpolator));

	return interpolators.back().get();
}

void DaedalusExports::TELEKINESIS_GetInterpolatedVec(TelekinesisInterpolator* interpolatorPtr, zVEC3* dest)
{
	zVEC3 result = interpolatorPtr->interpolate(std::chrono::system_clock::now());
	*dest = result;
}

void DaedalusExports::TELEKINESIS_DeleteInterpolator(TelekinesisInterpolator* interpolatorPtr)
{

	auto newEnd = std::remove_if(interpolators.begin(), interpolators.end(), [&](auto& it)
	{
		return it.get() == interpolatorPtr;
	});

	if (newEnd != interpolators.end())
	{
		logStream << "TELEKINESIS_DeleteInterpolator(): successfully removed interpolator!" << std::endl;
		util::debug(&logStream);
	}

	interpolators.erase(newEnd, interpolators.end());
}

void DaedalusExports::TELEKINESIS_Interpolate(TelekinesisInterpolator* interpolatorPtr, oCItem* item)
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

int DaedalusExports::TELEKINESIS_IsVobSeeable(oCNpc* npc, zCVob* vob)
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
					//logStream << "vob is mob: " << mob->name.ToChar() << std::endl;
					//util::logAlways(&logStream);
					break;
				}
			case VOB_TYPE_ITEM:
				{
					oCItem * item = (oCItem*)vob;
					//logStream << "vob is item: " << item->name.ToChar() << std::endl;
					//util::logAlways(&logStream);
					break;
				}
			case VOB_TYPE_NPC:
				{
					oCNpc * npc2 = (oCNpc*)vob;
					//logStream << "vob is npc: " << npc2->name[0].ToChar() << std::endl;
					//util::logAlways(&logStream);
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
		}

		if (!found)
			found = (intersectLength + 20 < VobLength) ? 0 : 1;
	}

	return found;
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

int DaedalusExports::LEVITATION_IsGamePaused()
{
	return Levitation::gameIsPaused;
}

void DaedalusExports::ItemUpdater::updateItem(void* obj, void* param, oCItem* itm)
{
	if (itm == NULL) return;

	ItemUpdater::UpdateItemData* params = (ItemUpdater::UpdateItemData*)param;
	ObjectManager* manager = ObjectManager::getObjectManager();
	int id = manager->getInstanceId(*itm);;
	if (id == params->expectedInstanceID)
	{
		//int refCtr = *(int*)((BYTE*)itm + 0x4);

		constexpr int ITEM_ACTIVE = 0x40000000;
		constexpr int ITEM_DROPPED = 1 << 10;
		constexpr int ITEM_NFOCUS = 1 << 23;
		bool isActive = itm->flags & ITEM_ACTIVE;
		bool dropped = itm->flags & ITEM_DROPPED;
		bool nfocus = itm->flags & ITEM_NFOCUS;

		bool isInWorld = manager->isItemInWorld(itm);
		if (!isInWorld && isActive)
		{
			//zCWorld* world = oCGame::GetGame()->GetWorld();
			//world->AddVob(itm);
		}

		//zCWorld* world = oCGame::GetGame()->GetWorld();
		//world->AddVob(itm);


		int flags = itm->flags;
		//manager->oCItemSaveRemoveEffect(itm);
		//itm->effect = "SPELLFX_FIREBOW";

		auto resolvedID = manager->resolveProxying(params->newInstanceID);
		bool notSameInstance = manager->resolveProxying(params->newInstanceID) != id;

		if (notSameInstance) {
			logStream << __FUNCSIG__ << ": setting another instance Id causes item effects not to vanish!" << std::endl;
			util::logWarning(&logStream);
		}

		//manager->oCItemSaveRemoveEffect(itm);
		manager->setInstanceId(itm, resolvedID);
		itm->InitByScript(resolvedID, itm->amount);

		if (itm->effect.ToChar() == std::string("")) {
			manager->oCItemSaveRemoveEffect(itm);
		}

		//zCVob* effectVob = (zCVob*)itm->effectVob;
		//if (effectVob) {
		//	effectVob->trafo = itm->trafo;
		//}
		//itm->effectVob

		//itm->flags = flags;
		if (isActive) itm->flags |= ITEM_ACTIVE;
		if (dropped) itm->flags |= ITEM_DROPPED;
		if (nfocus) itm->flags |= ITEM_NFOCUS;

		//world->AddVob(itm);

		isInWorld = manager->isItemInWorld(itm);
		if (isInWorld && !isActive) { //&&!isActive
			//manager->oCItemSaveRemoveEffect(itm);
			manager->oCItemSaveInsertEffect(itm);

			//zCWorld* world = oCGame::GetGame()->GetWorld();
			//world->AddVob(itm);
		}
	}
}

void DaedalusExports::ItemUpdater::updateItemInstance(void* obj, void* param, oCItem* itm)
{
	if (itm == NULL) return;

	ItemUpdater::UpdateItemData* params = (ItemUpdater::UpdateItemData*)param;
	ObjectManager* manager = ObjectManager::getObjectManager();
	int id = manager->getInstanceId(*itm);;
	if (id == params->expectedInstanceID)
	{
		auto resolvedID = manager->resolveProxying(params->newInstanceID);
		manager->setInstanceId(itm, resolvedID);
	}
}