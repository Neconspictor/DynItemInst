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


const float DaedalusExports::LIB_VERSION = 1.03f;

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

bool DaedalusExports::DII_AddProxy(const zSTRING& sourceInstanceName, const zSTRING& targetInstanceName)
{
	return ObjectManager::getObjectManager()->addProxy(sourceInstanceName, targetInstanceName);
}

void DaedalusExports::DII_ApplyInstanceChangesToAll(const zSTRING& instanceName)
{
	logStream << __FUNCSIG__ << ": " << instanceName.ToChar() << std::endl;
	util::debug(&logStream);

	auto* manager = ObjectManager::getObjectManager();
	auto* parser = zCParser::GetParser();
	auto symbolIndex = parser->GetIndex(instanceName);
	if (!manager->isDynamicInstance(symbolIndex)) {
		logStream << __FUNCSIG__ << ": " << instanceName.ToChar() << " is not a  DII" << std::endl;
		util::logWarning(&logStream);
		return;
	}

	ItemUpdater::UpdateItemData params = { symbolIndex, symbolIndex };
	manager->callForAllItems(ItemUpdater::updateItem, NULL, &params);	
}

void DaedalusExports::DII_RemoveProxy(const zSTRING& sourceInstanceName)
{
	ObjectManager::getObjectManager()->removeProxy(sourceInstanceName);
}


oCItem* __cdecl DaedalusExports::DII_CreateNewItem(int instanceId) // Func void DII_CreateNewItem(var C_Item item, VAR INT instanceId)
{

	oCItem* item = oCObjectFactory::GetFactory()->CreateItem(instanceId);

	logStream << "DII_CreateNewItem: " << instanceId;
	util::debug(&logStream);

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
	util::debug(&logStream);

	if (*refCtr >= 0)
	{
		oCGame::GetGame()->GetGameWorld()->RemoveVob(item);
	}

	ObjectManager::oCItemOperatorDelete(item);
}


int DaedalusExports::DII_CreateNewInstanceInt(oCItem* item) //Func int DII_CreateNewInstance(var C_Item item)
{
	if (item == NULL) {return NULL;}

	logStream << __FUNCSIG__ << ": Param: " << item->name.ToChar();
	util::debug(&logStream);

	// Create new instance with item
	ObjectManager* manager = ObjectManager::getObjectManager();


	int parserSymbolIndex = 0;

	while (!parserSymbolIndex) {
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

		const std::string instanceName = "DII_*" + std::to_string(util::generateRandom(std::numeric_limits<int>::min(), std::numeric_limits<int>::max())) + "*";

#pragma pop_macro("max")
#pragma pop_macro("min")

		parserSymbolIndex = manager->createNewInstanceId(item, instanceName);
		if (!parserSymbolIndex) {
			logStream << __FUNCSIG__ << ": Couldn't create new instance '" << instanceName << "'" << std::endl;
			util::logWarning(&logStream);
		}
	}

	logStream << __FUNCSIG__ << ": key = " << parserSymbolIndex << std::endl;
	util::debug(&logStream);
	return parserSymbolIndex;
}

zSTRING* DaedalusExports::DII_CreateNewInstance(oCItem* item)
{
	const auto parserSymbolIndex = DII_CreateNewInstanceInt(item);
	auto* symbol = zCParser::GetParser()->GetSymbol(parserSymbolIndex);
	return &symbol->name;
}

int DaedalusExports::DII_CreateNewInstanceStr(oCItem* item, const zSTRING& instanceName)
{
	if (!item) {
		return false;
	}

	std::string instanceNameStr = instanceName.ToChar();
	std::transform(instanceNameStr.begin(), instanceNameStr.end(), instanceNameStr.begin(), std::toupper);


	logStream << "Param: " << item->name.ToChar();
	util::debug(&logStream);

	// Create new instance with item
	ObjectManager* manager = ObjectManager::getObjectManager();

	int parserSymbolIndex = manager->createNewInstanceId(item, instanceNameStr);
	if (!parserSymbolIndex) {
		logStream << __FUNCSIG__ << ": Couldn't create new instance '" << instanceNameStr << "'" << std::endl;
		util::logWarning(&logStream);
	}


	int index = manager->getDynInstanceId(item);
	logStream << __FUNCSIG__ << "Index: " << index << std::endl;
	util::debug(&logStream);

	logStream << __FUNCSIG__ << ": parser symbol index for instance = " << parserSymbolIndex << std::endl;
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
		logStream << __FUNCSIG__ << ": instanceId isn't dynamic" << std::endl;
		util::logWarning(&logStream);
		return NULL;
	}

	DynInstance* storeItem = manager->getInstanceItem(instanceIdParserSymbolIndex);
	return storeItem->getUserData();
}


float DaedalusExports::GetLibVersion()
{
	return LIB_VERSION;
}

bool DaedalusExports::DII_UpdateInstance(const zSTRING& instanceName, oCItem* item)
{
	ObjectManager* manager = ObjectManager::getObjectManager();

	auto instanceIdParserSymbolIndex = manager->getUnProxiedInstanceID(instanceName);

	DynInstance* dynInstance = manager->getInstanceItem(instanceIdParserSymbolIndex);

	if (!item) {
		logStream << __FUNCSIG__ << ": item mustn't be null " << std::endl;
		util::logWarning(&logStream);
		return false;
	}

	if (!dynInstance)
	{
		logStream << __FUNCSIG__ << ": dynInstance not found for instance id " << instanceIdParserSymbolIndex  << std::endl;
		util::logWarning(&logStream);
		return false;
	}
	dynInstance->store(*item);

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
		logStream << __FUNCSIG__<< ": instanceSym is Null! No item will be searched!" << std::endl;
		util::logWarning(&logStream);
		return;
	}


	ObjectManager* manager = ObjectManager::getObjectManager();
	item = manager->getItemByInstanceId(instanceIdParserSymbolIndex);

	if (!item)
	{
		logStream << __FUNCSIG__ << ": item is null!" << std::endl;
		util::logWarning(&logStream);
	} else
	{
		logStream << __FUNCSIG__ << ": item found..." << std::endl;
		util::debug(&logStream);
	};

	// update the c_item
	symbol->offset = (int)item;
}


void DaedalusExports::DII_ChangeInstanceForAll(const zSTRING& sourceName, const zSTRING& targetName)
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
	logStream << __FUNCSIG__ << ": vobPosition = " << *vobPosition << std::endl;
	logStream << __FUNCSIG__ << ": npcPosition = " << *npcPosition << std::endl;
	logStream << __FUNCSIG__ << ": upMoveAmount = " << upMoveAmount << std::endl;
	logStream << __FUNCSIG__ << ": speed = " << speed << std::endl;
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
		logStream << __FUNCSIG__ << ": successfully removed interpolator!" << std::endl;
		util::debug(&logStream);
	}

	interpolators.erase(newEnd, interpolators.end());
}

void DaedalusExports::TELEKINESIS_Interpolate(TelekinesisInterpolator* interpolatorPtr, zCVob* vob)
{
	zVEC3 result = interpolatorPtr->interpolate(std::chrono::system_clock::now());
	zVEC3 current = vob->GetVobPosition();
	zVEC3 diff = result - current;
	vob->Move(diff.x, diff.y, diff.z);
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
	const int traceFlags = zTraceRay_vob_ignore_no_cd_dyn
					//| zTRACERAY_VOB_IGNORE
					| zTRACERAY_VOB_IGNORE_CHARACTER
					| zTraceRay_poly_ignore_transp 
					| zTraceRay_vob_ignore_projectiles 
					| zTraceRay_poly_test_water;

	const zVEC3 direction = vob->GetVobPosition() - npc->GetPosition();
	const zVEC3 npcPosition = npc->GetPosition();


	const int result = zCWorldTraceRayNearestHit(world, npcPosition, direction, npc, traceFlags);

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
					logStream << __FUNCSIG__ << ": vob is mob: " << mob->name.ToChar() << std::endl;
					util::debug(&logStream);
					break;
				}
			case VOB_TYPE_ITEM:
				{
					oCItem * item = (oCItem*)vob;
					logStream << __FUNCSIG__ << ": vob is item: " << item->name.ToChar() << std::endl;
					util::debug(&logStream);
					break;
				}
			case VOB_TYPE_NPC:
				{
					oCNpc * focusNpc = (oCNpc*)vob;
					logStream << __FUNCSIG__ << ": vob is npc: " << focusNpc->name[0].ToChar() << std::endl;
					util::debug(&logStream);
					break;
				}
			default: ;
			}
			vob->GetVobType();
		}
	}


	if (result && !found)
	{
		float intersectLength = (world->foundIntersection - npcPosition).Length();
		float VobLength = direction.Length();

		zCVob* foundVob = world->foundVob;
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
		int flags = itm->flags;

		auto resolvedID = manager->resolveProxying(params->newInstanceID);
		bool notSameInstance = manager->resolveProxying(params->newInstanceID) != id;

		if (notSameInstance) {
			logStream << __FUNCSIG__ << ": setting another instance Id causes item effects not to vanish!" << std::endl;
			util::logWarning(&logStream);
		}

		manager->setInstanceId(itm, resolvedID);
		itm->InitByScript(resolvedID, itm->amount);

		if (itm->effect.ToChar() == std::string("")) {
			manager->oCItemSaveRemoveEffect(itm);
		}

		if (isActive) itm->flags |= ITEM_ACTIVE;
		if (dropped) itm->flags |= ITEM_DROPPED;
		if (nfocus) itm->flags |= ITEM_NFOCUS;

		if (isInWorld && !isActive) {
			manager->oCItemSaveInsertEffect(itm);
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