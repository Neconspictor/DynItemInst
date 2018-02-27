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



#include <DynItemInst.h>
#include <ObjectManager.h>
#include <Util.h>
#include <HookManager.h>
#include <Windows.h>
#include <api/g2/zcworld.h>
#include <api/g2/ocgame.h>
#include <api/g2/zcparser.h>
#include <Logger.h>
#include <api/g2/ocnpc.h>
#include <api/g2/ocnpcinventory.h>
#include <api/g2/oCObjectFactory.h>
#include <map>
#include <Configuration.h>
#include <Levitation.h>
#include <functional>
#include <Constants.h>

using namespace constants;

const std::string DynItemInst::SAVE_ITEM_FILE_EXT = ".SAV";
const std::string DynItemInst::SAVE_ITEM_INSTANCES  = "DII_INSTANCES";
const std::string DynItemInst::SAVE_ITEM_ADDIT = "DII_ADDIT_";
const std::string DynItemInst::SAVE_ITEM_HERO_DATA = "DII_HERO_DATA";
const std::string DynItemInst::FILE_PATERN = "DII_*";
bool DynItemInst::denyMultiSlot = false;
bool DynItemInst::levelChange = false;
bool DynItemInst::saveGameIsLoading = false;
bool DynItemInst::saveGameWriting = false;
std::list<std::pair<int, int>> DynItemInst::reusableMarkList;

DynItemInst::InstanceNames DynItemInst::instanceNames = { "DII_DUMMY_ITEM", "_NF_","_FF_" , "_RUNE_", "_OTHER_", 1, 1, 1, 1 };

std::vector<zCPar_Symbol*>* DynItemInst::symbols = new std::vector<zCPar_Symbol*>();
bool DynItemInst::showExtendedDebugInfo = false;


typedef void ( __thiscall* LoadSavegame )(void*, int, int); 
LoadSavegame loadSavegame;
typedef void ( __thiscall* WriteSavegame )(void*, int, int); 
WriteSavegame writeSavegame;
typedef int ( __thiscall* OCItemGetValue )(void*); 
OCItemGetValue oCItemGetValue;
typedef zCPar_Symbol* (__thiscall* ZCPar_SymbolTableGetSymbol)(void* pThis, int index); 
ZCPar_SymbolTableGetSymbol zCPar_SymbolTableGetSymbol;
typedef zCPar_Symbol* (__thiscall* ZCPar_SymbolTableGetSymbolString)(void* pThis, zSTRING const & symbolNaame); 
ZCPar_SymbolTableGetSymbolString zCPar_SymbolTableGetSymbolString;
typedef int (__thiscall* ZCPar_SymbolTableGetIndex)(void* pThis, zSTRING const & symbolNaame); 
ZCPar_SymbolTableGetIndex zCPar_SymbolTableGetIndex;
typedef int (__thiscall* ZCParserGetIndex)(void* pThis, zSTRING const & symbolNaame); 
ZCParserGetIndex zCParserGetIndex;
typedef int (__thiscall* CreateInstance)(void* pThis, int instanceId, void* source);
CreateInstance createInstance;
typedef void (__thiscall* OCGameLoadGame)(void* pThis, int, zSTRING const &);
OCGameLoadGame oCGameLoadGame;
typedef void ( __thiscall* OCGameLoadWorld )(void*, int, zSTRING const &); 
OCGameLoadWorld oCGameLoadWorld;
typedef void ( __thiscall* OCGameChangeLevel )(void*, zSTRING const &, zSTRING const &); 
OCGameChangeLevel oCGameChangeLevel;
typedef int ( __thiscall* OCItemMulitSlot )(void*); 
OCItemMulitSlot oCItemMulitSlot;
typedef void ( __thiscall* OCMobContainerOpen )(void*, oCNpc*); 
OCMobContainerOpen oCMobContainerOpen;


typedef void(__thiscall* ZCVobSetHeadingAtWorld)(void*, zVEC3*);
ZCVobSetHeadingAtWorld zCVobSetHeadingAtWorld;
typedef void(__thiscall* DoSurfaceAlignment)(void*); DoSurfaceAlignment doSurfaceAlignment2;

typedef int(__thiscall* ZCAIPlayerCheckFloorSliding)(void*);
ZCAIPlayerCheckFloorSliding zCAIPlayerCheckFloorSliding2;

typedef void(__thiscall* ZCVobSetPhysicsEnabled)(void*, int); ZCVobSetPhysicsEnabled zCVobSetPhysicsEnabled2;

typedef void(__thiscall* ZCVobCheckAndResolveCollisions)(void*); ZCVobCheckAndResolveCollisions zCVobCheckAndResolveCollisions2;


OCItemInsertEffect DynItemInst::oCItemInsertEffect = (OCItemInsertEffect)0x00712C40;

void DynItemInst::zCVobSetHeadingAtWorldHook(void * pThis, zVEC3 * vec)
{
	if (oCNpc::GetHero() == pThis) {
		return;
	}
	zCVobSetHeadingAtWorld(pThis, vec);
}

void DynItemInst::checkReusableInstances()
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	for (std::list<std::pair<int, int>>::iterator it = reusableMarkList.begin(); it != reusableMarkList.end(); ++it)
	{
		logStream << "DynItemInst::checkReusableInstances: mark as reusable: " << it->first << ", " << it->second << std::endl;
		util::debug(&logStream);
		manager->markAsReusable(it->first, it->second);
	}

	manager->checkReusableInstances();

	reusableMarkList.clear();
}

void DynItemInst::addToReusableLists(int instanceId, int previousId)
{
	reusableMarkList.push_back(std::pair<int, int>(instanceId, previousId));
}

void DynItemInst::hookModule()
{
	loadSavegame = (LoadSavegame) (LOAD_SAVEGAME_ADDRESS);
	writeSavegame = (WriteSavegame) (WRITE_SAVEGAME_ADDRESS);
	oCItemGetValue = (OCItemGetValue) (OCITEM_GET_VALUE_ADDRESS);
	createInstance = (CreateInstance) (ZCPARSER_CREATE_INSTANCE);
	oCGameLoadGame = (OCGameLoadGame) OCGAME_LOAD_GAME_ADDRESS;

	oCGameChangeLevel = reinterpret_cast<OCGameChangeLevel>(OCGAME_CHANGE_LEVEL_ADDRESS);
	oCItemMulitSlot = (OCItemMulitSlot) OCITEM_MULTI_SLOT;
	oCMobContainerOpen = (OCMobContainerOpen) OCMOB_CONTAINER_OPEN;

	zCParserGetIndex = (ZCParserGetIndex)ZCPARSER_GETINDEX;
	zCPar_SymbolTableGetIndex = (ZCPar_SymbolTableGetIndex) ZCPAR_SYMBOL_TABLE_GETINDEX;
	zCPar_SymbolTableGetSymbol = (ZCPar_SymbolTableGetSymbol)ZCPAR_SYMBOL_TABLE_GETSYMBOL;
	zCPar_SymbolTableGetSymbolString = (ZCPar_SymbolTableGetSymbolString)ZCPAR_SYMBOL_TABLE_GETSYMBOL_STRING;

	zCVobSetHeadingAtWorld = (ZCVobSetHeadingAtWorld)ZCVOB_SET_HEADING_AT_WORLD;
	doSurfaceAlignment2 = reinterpret_cast<DoSurfaceAlignment>((DO_SURFACE_ALIGNMENT_ADDRESS));
	zCAIPlayerCheckFloorSliding2 = reinterpret_cast<int(__thiscall*)(void*)>((ZCAIPLAYER_CHECK_FLOOR_SLIDING_ADDRESS));
	zCVobSetPhysicsEnabled2 = reinterpret_cast<ZCVobSetPhysicsEnabled>(ZCVOB_SET_PHYSICS_ENABLED_ADDRESS);
	zCVobCheckAndResolveCollisions2 = reinterpret_cast<ZCVobCheckAndResolveCollisions>((ZCVOB_CHECK_AND_RESOLVE_COLLISION_ADDRESS));

		//0x006521E0

	HookManager* hookManager = HookManager::getHookManager();

	//hookManager->addFunctionHook((LPVOID*)&zCVobSetHeadingAtWorld, zCVobSetHeadingAtWorldHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&doSurfaceAlignment2, DoSurfaceAlignmentHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCAIPlayerCheckFloorSliding2, zCAIPlayerCheckFloorSlidingHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobSetPhysicsEnabled2, zCVobSetPhysicsEnabledHook, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&loadSavegame, loadSavegameHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&writeSavegame, writeSavegameHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHook, moduleDesc);
	

	hookManager->addFunctionHook((LPVOID*)&createInstance, createInstanceHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHook, moduleDesc);
	
	hookManager->addFunctionHook((LPVOID*)&oCGameChangeLevel, oCGameChangeLevelHook, moduleDesc);
	
	hookManager->addFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHook, moduleDesc);
	
	
	hookManager->addFunctionHook((LPVOID*)&zCParserGetIndex, zCParserGetIndexHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetIndex, zCPar_SymbolTableGetIndexHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbol, zCPar_SymbolTableGetSymbolHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbolString, zCPar_SymbolTableGetSymbolStringHook, moduleDesc);
	

	denyMultiSlot = true;
	loadDynamicInstances();
	initAdditMemory();
	denyMultiSlot = false;
}

void DynItemInst::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&loadSavegame, loadSavegameHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&writeSavegame, writeSavegameHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHook, moduleDesc);

	hookManager->removeFunctionHook((LPVOID*)&createInstance, createInstanceHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameChangeLevel, oCGameChangeLevelHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHook, moduleDesc);
	//hookManager->removeFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHook, moduleDesc);

	hookManager->removeFunctionHook((LPVOID*)&zCParserGetIndex, zCParserGetIndexHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetIndex, zCPar_SymbolTableGetIndexHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbol, zCPar_SymbolTableGetSymbolHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbolString, zCPar_SymbolTableGetSymbolStringHook, moduleDesc);
};


void DynItemInst::DoSurfaceAlignmentHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (hero == pThis);
	if (adjust) {
		return;
	}
	return doSurfaceAlignment2(pThis);
}

int DynItemInst::zCAIPlayerCheckFloorSlidingHook(void * pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero)) {
		return 1;
	}
	return zCAIPlayerCheckFloorSliding2(pThis);
}

void DynItemInst::zCVobSetPhysicsEnabledHook(void * pThis, int second)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero)) {
		//if (input->KeyPressed(0x1A)) return; //zCVobSetPhysicsEnabled(pThis, true);
		//zCVobCheckAndResolveCollisions2(pThis);
		return;
	}
	zCVobSetPhysicsEnabled2(pThis, second);
}

_declspec(naked) void DynItemInst::zCPar_SymbolTableGetSymbolStringHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Bytes for remaining opcode */
			nop
			/*finally hook function call*/
			jmp DynItemInst::zCPar_SymbolTableGetSymbolStringHook
	}
}


/*_declspec(naked) void DynItemInst::zCPar_SymbolTableGetSymbolHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		//6 - 5 = 1 Bytes for remaining opcode
			nop
			//finally hook function call
			jmp DynItemInst::zCPar_SymbolTableGetSymbolHook
	}
}*/


_declspec(naked) void DynItemInst::zCPar_SymbolTableGetIndexHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*5 - 5 = 0 Bytes for remaining opcode */
			/*finally hook function call*/
			jmp DynItemInst::zCPar_SymbolTableGetIndexHook
	}
}


_declspec(naked) void DynItemInst::zCParserGetIndexHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Bytes for remaining opcode */
			nop
			/*finally hook function call*/
			jmp DynItemInst::zCParserGetIndexHook
	}
}


_declspec(naked) void DynItemInst::loadSavegameHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*7 - 5 = 2 Bytes for remaining opcode */
			nop
			nop
			/*finally hook function call*/
			jmp DynItemInst::loadSavegameHook
	}
}

_declspec(naked) void DynItemInst::writeSavegameHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Byte for remaining opcode */
			nop
			/*finally hook function call*/
			jmp DynItemInst::writeSavegameHook
	}
}


_declspec(naked) void DynItemInst::oCItemGetValueHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*9 - 5 = 4 Bytes for remaining opcode */
		nop
		nop
		nop
		nop
		/*finally hook function call*/
		jmp DynItemInst::oCItemGetValueHook	
	}
}

_declspec(naked) void DynItemInst::createInstanceHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*5 - 5 = 0 Bytes for remaining opcode */
		/*finally hook function call*/
		jmp DynItemInst::createInstanceHook
	}
}

_declspec(naked) void DynItemInst::oCGameLoadGameHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*5 - 5 = 0 Bytes for remaining opcode */
		/*finally hook function call*/
			jmp DynItemInst::oCGameLoadGameHook
	}
}

_declspec(naked) void DynItemInst::oCGameChangeLevelHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE_2_5
		/*7 - 5 = 2 Bytes for remaining opcode */
		nop
		nop
		/*finally hook function call*/
			jmp DynItemInst::oCGameChangeLevelHook
	}
}

_declspec(naked) void DynItemInst::oCItemMulitSlotHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Byte for remaining opcode */
			nop
			/*finally hook function call*/
			jmp DynItemInst::oCItemMulitSlotHook
	}
}

_declspec(naked) void DynItemInst::oCMobContainerOpenHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Byte for remaining opcode */
			nop
			/*finally hook function call*/
			jmp DynItemInst::oCMobContainerOpenHook
	}
}


int DynItemInst::oCItemGetValueHook(void* pThis) {
	oCItem* item = static_cast<oCItem*>(pThis);
	ObjectManager* manager = ObjectManager::getObjectManager();
	//if (manager->getDynInstanceId(item) > ObjectManager::INSTANCE_BEGIN) {
	if (manager->IsModified(item)) {
		return item->value;
	}
	return oCItemGetValue(pThis);
};



zCPar_Symbol* DynItemInst::zCPar_SymbolTableGetSymbolHook(void* pThis, int index)
{
	zCPar_Symbol* result = zCPar_SymbolTableGetSymbol(pThis, index);
	if (result == NULL)
	{
		result = ObjectManager::getObjectManager()->getSymbolByIndex(index);
	}

	return result;
}

zCPar_Symbol* DynItemInst::zCPar_SymbolTableGetSymbolStringHook(void* pThis, zSTRING const & symbolName)
{
	zCPar_Symbol* result = ObjectManager::getObjectManager()->getSymbolByName(symbolName);
	if (result == NULL)
	{
		result = zCPar_SymbolTableGetSymbolString(pThis, symbolName);
	}
	return result;
}

int DynItemInst::zCPar_SymbolTableGetIndexHook(void* pThis, zSTRING const& symbolName)
{
	int result = zCPar_SymbolTableGetIndex(pThis, symbolName);
	if (result == NULL)
	{
		result = ObjectManager::getObjectManager()->getIndexByName(symbolName);
	} 
	return result;
}

int DynItemInst::zCParserGetIndexHook(void* pThis, zSTRING const& symbolName)
{
	int result = ObjectManager::getObjectManager()->getIndexByName(symbolName);
	if (result == NULL)
	{
		result = zCParserGetIndex(pThis, symbolName);	
	}
	return result;
}

DynItemInst::DynItemInst()
	:Module()
{
	moduleDesc = "DynItemInst";
}

DynItemInst::~DynItemInst()
{
}


 void DynItemInst::loadSavegameHook(void* pThis,int saveGameSlotNumber, int b)
{   
	logStream << "DynItemInst::loadSavegameHook: load savegame..." << std::endl;
	util::logInfo(&logStream);
	saveGameIsLoading = true;
	denyMultiSlot = true;
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	loadSavegame(pThis, saveGameSlotNumber, b);
	loadDynamicInstances();
	initAdditMemory();

	denyMultiSlot = false;
	saveGameIsLoading = false;

	logStream << "DynItemInst::loadSavegameHook: done." << std::endl;
	util::logInfo(&logStream);
};


 void DynItemInst::writeSavegameHook(void* pThis,int saveGameSlotNumber, int b)
{   
	logStream << "DynItemInst::writeSavegameHook: save game..." << std::endl;
	util::logInfo(&logStream);

	saveGameWriting = true;
	// reset instance name counters
	instanceNames.nearFightCounter = 1;
	instanceNames.distanceFightCounter = 1;
	instanceNames.runeCounter = 1;
	instanceNames.otherCounter = 1;

	oCGame* game = oCGame::GetGame();
	zCWorld* world = game->GetWorld();
	oCWorld* ocworld = game->GetGameWorld();
	ObjectManager* manager = ObjectManager::getObjectManager();
	
	zCListSort<oCNpc>* npcList = world->GetNpcList();

	manager->resetDynItemInstances();

	denyMultiSlot = true;

	oCNpc* hero = oCNpc::GetHero();
	while(npcList != NULL) {
		oCNpc* npc = npcList->GetData();
		if (npc == NULL) {
			npcList = npcList->GetNext();
			continue;
		}
		bool isHero = npc == hero;
		
		if (isHero && DynItemInst::levelChange) {
			npcList = npcList->GetNext();
			continue;
		}

		oCNpcInventory* inventory = npc->GetInventory();
		if (inventory == NULL) {
			npcList = npcList->GetNext();
			continue;
		}
		inventory->UnpackAllItems();
		zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
		oCMag_Book* magBook = oCNpcGetSpellBook(npc);
		
		int selectedSpellKey = manager->getSelectedSpellKey(npc);
		
		if (selectedSpellKey >= 0)
		{
			oCMag_BookKillSelectedSpell(magBook);
		}

		resetInstanceNameStruct();

		while(list != NULL) {
			oCItem* item = list->GetData();
			int id = modifyItemForSaving(item, isHero);
			int equiped = id && item->HasFlag(OCITEM_FLAG_EQUIPPED);
			
			magBook = oCNpcGetSpellBook(npc);
			int spellKey = manager->getEquippedSpellKeyByItem(npc, item);

			// selected spell key begins at 0, but spell key strangely at 1 
			bool activeSpellItem = (selectedSpellKey  == (item->spell));
			if (id)
			{
				logStream << "DynItemInst::writeSavegameHook: create addit memory= " << id << std::endl;
				util::debug(&logStream);
			}
			if (id) manager->createAdditionalMemory(item, id, isHero, activeSpellItem, spellKey);
			if (equiped)
			{
				logStream << "DynItemInst:: Item is equipped! " << std::endl;
				util::debug(&logStream);
				//Deny invocation of equip function
				int unEquipFunction = item->on_unequip;
				item->on_unequip = 0;

				//unequip item
				npc->Equip(item);

				//restore functions
				item->on_unequip = unEquipFunction;
				
				//Mark item as equipped
				item->SetFlag(OCITEM_FLAG_EQUIPPED);
			}

			list = list->GetNext();
		}
		npcList = npcList->GetNext();
	}
	
	zCListSort<oCItem>* itemList = world->GetItemList();
	while(itemList != NULL) {
		oCItem* item = itemList->GetData();
		int id = modifyItemForSaving(item, false);
		if (id) manager->createAdditionalMemory(item, id, false);
		itemList = itemList->GetNext();
	}

	denyMultiSlot = false;

	//finally write the savegame and restore items that are reseted by the savegame writing method
	std::string saveGameDir;
	if (saveGameSlotNumber != -1)
	{
		saveGameDir = manager->getSaveGameDirectoryPath(saveGameSlotNumber);	
	} else
	{
		saveGameDir = manager->getCurrentDirectoryPath();
	}

	std::string currentDir = manager->getCurrentDirectoryPath();

	util::copyContentTo(currentDir,saveGameDir, FILE_PATERN);
	//util::copyContentTo(saveGameDir,currentDir, FILE_PATERN);

	zSTRING worldName = ocworld->GetWorldName();
	std::string saveAddit = SAVE_ITEM_ADDIT + std::string(const_cast<char*>(worldName.ToChar())) + 
		SAVE_ITEM_FILE_EXT; 
	std::string saveInstances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;
	std::string heroData = SAVE_ITEM_HERO_DATA + SAVE_ITEM_FILE_EXT;
	
	// Write actual savegame
	writeSavegame(pThis, saveGameSlotNumber, b);
	
	std::list<AdditMemory*> heroItemList;
	manager->getHeroAddits(heroItemList);
	int heroItemSize = heroItemList.size();
	manager->saveHeroData(heroItemList, const_cast<char*>(saveGameDir.c_str()), const_cast<char*>(heroData.c_str()));
	heroItemList.clear();

	manager->saveNewInstances(const_cast<char*>(saveGameDir.c_str()), const_cast<char*>(saveInstances.c_str()));
	manager->saveWorldObjects(heroItemSize, const_cast<char*>(saveGameDir.c_str()), const_cast<char*>(saveAddit.c_str()));

	util::copyFileTo(saveGameDir +  saveInstances, currentDir + saveInstances);
	util::copyFileTo(saveGameDir + saveAddit, currentDir + saveAddit);
	util::copyFileTo(saveGameDir + heroData, currentDir + heroData);

	restoreDynamicInstances(game);
	manager->removeAllAdditionalMemory();	

	saveGameWriting = false;

	logStream << "DynItemInst::writeSavegameHook: done." << std::endl;
	util::logInfo(&logStream);
};

 void DynItemInst::restoreSpecificNpcItemStub( oCItem* item,  
								oCNpcInventory* inventory, 
								AdditMemory* addit,
								std::map<int, oCItem*>* equippedSpells, 
								oCItem** activeSpellItem) {

	int instanceId = item->GetInstance();
	bool originalMultiSlotSetting = denyMultiSlot;
	denyMultiSlot = true;

	// is the item equipped?
	int equipped = item->HasFlag(OCITEM_FLAG_EQUIPPED);

	if (equipped)
	{
		item->SetFlag(OCITEM_FLAG_EQUIPPED);
		handleEquippedMagicItem(item, inventory, addit, instanceId,
			equippedSpells, activeSpellItem);
		denyMultiSlot = originalMultiSlotSetting;
		return;
	}
		
	oCItemInitByScript(item, instanceId, item->instanz);
	item->ClearFlag(OCITEM_FLAG_EQUIPPED);
	//inventory->Remove(item, item->instanz);
	//inventory->Insert(item);

	denyMultiSlot = originalMultiSlotSetting;
}


 int DynItemInst::modifyItemForSaving(oCItem* item, bool isHeroItem) {
	if (item == NULL) return NULL;

	if (item->instanz < 0)
	{
		// item was already processed
		return 0;
	}

	ObjectManager* manager = ObjectManager::getObjectManager();

	int id = manager->getDynInstanceId(item);

	// make exception for runes
	if ((id == NULL) && item->HasFlag(OCITEM_FLAG_ITEM_KAT_RUNE) && item->HasFlag(OCITEM_FLAG_EQUIPPED)) {
		logStream << "DynItemInst::modifyItemForSaving: modified item for special cases: " << item->GetInstance()  << std::endl;
		util::debug(&logStream);
		return manager->getIdForSpecialPurposes();
	}

	if (id == NULL) return NULL;

	manager->oCItemSaveRemoveEffect(item);
	
	zCParser* parser = zCParser::GetParser();
	int saveId; 
	
	if (item->HasFlag(OCITEM_FLAG_EQUIPPED))
	{
		logStream << "DynItemInst::modifyItemForSaving: Equipped item will be processed: " << std::endl;
		logStream << "item->description: " << item->description.ToChar() << std::endl;
		util::debug(&logStream);
		std::stringstream ss;
		if (item->HasFlag(2)) //near fight
		{
			ss << instanceNames.base << instanceNames.nearFight << instanceNames.nearFightCounter;
			++instanceNames.nearFightCounter;
		} else if (item->HasFlag(4)) // distance fight
		{
			ss << instanceNames.base << instanceNames.distanceFight << instanceNames.distanceFightCounter;
			++instanceNames.distanceFightCounter;
		} else if (item->HasFlag(512)) // runes 
		{
			ss << instanceNames.base << instanceNames.rune << instanceNames.runeCounter;
			++instanceNames.runeCounter;
		}
		else // other
		{
			ss << instanceNames.base << instanceNames.other << instanceNames.otherCounter;
			++instanceNames.otherCounter;
		}

		std::string name = ss.str();
		saveId = parser->GetIndex(name.c_str());

		if (saveId <= 0)
		{
			logStream << "instance name not found: " << name << std::endl;
			util::logFatal(&logStream);
			//throw new DynItemInst::DII_InstanceNameNotFoundException(ss.str().c_str());
		}
		
	} else
	{
		saveId = parser->GetIndex("DII_DUMMY_ITEM");
		logStream << "DynItemInst::modifyItemForSaving: saveId: " << saveId << std::endl;
		logStream << "DynItemInst::modifyItemForSaving: saveId is DII_DUMMY_ITEM: " << item->description.ToChar() << std::endl;
		util::debug(&logStream, Logger::Info);
	}
	manager->setInstanceId(item, saveId);

	//add active world name to dyn instance, but only if item is no hero item (important for level change!)
	if (!isHeroItem)
	{
		zCWorld* world = oCGame::GetGame()->GetWorld();
		std::string worldName = world->worldName.ToChar();
		manager->getInstanceItem(id)->addActiveWorld(worldName);
	}

	return id;
}


 oCItem* DynItemInst::makeEquippedCopy(oCItem* item, oCNpcInventory* inventory)
 {
	 if (item->instanz >= 0) {
		 return item;
	 }
	 ObjectManager* manager = ObjectManager::getObjectManager();
	 int additId = -item->instanz;
	 AdditMemory* addit = manager->getAddit(additId);
	 if (addit == NULL) {
		 logStream << "DynItemInst::makeEquippedCopy: Warning: Addit is null!!!" << std::endl;
		 logStream << item->name.ToChar() << " : " << additId << std::endl;
		 util::debug(&logStream, Logger::Fault);
		 return NULL;
	 }

	 int instanceId = addit->instanceId;
	 int instanz = addit->instanz;
	 item->instanz = instanz;

	 zCWorld* world = oCGame::GetGame()->GetWorld();

	 oCNpc* owner = inventory->GetOwner();
	 int amount = item->instanz;
	 
	 //TODO: deny invocation od equip function maybe?
	  owner->EquipItem(item);
	 inventory->Remove(item, item->instanz);
	 zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, instanceId);
	 oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceId);
	 if (list != NULL)
	 {
		 item = list->GetData();
		 copy->instanz = item->instanz + amount;
		 inventory->Remove(item, item->instanz);
	 } else
	 {
		 copy->instanz = amount;
	 }

	 return copy;
 }

bool DynItemInst::itemsAreModified()
{
	return isSaveGameLoading() || levelChange || saveGameWriting;
}

void DynItemInst::restoreWorldItem(void* obj, void* param, oCItem* itm) {
	if (itm == NULL) return;
	if (itm->instanz >= 0) return;

	ObjectManager* manager = ObjectManager::getObjectManager();

	if (!manager->updateItemAndAddit(itm)) {
		//Should not occur!!!!
		//logStream << "DynItemInst::restoreItem: Addit is null or was already visited!!!" << std::endl;
		//logStream << item->name.ToChar() << " : " << additId << std::endl;
		//util::logFault(&logStream);
		return;
	}

	int instanceID = itm->GetInstance();

	zCWorld* world = oCGame::GetGame()->GetWorld();
	oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceID);

	//instanz (item count) has to be set manually
	copy->instanz = itm->instanz;

	// SetPositionWorld has to be done before adding the vob to the world!
	// Otherwise the position won't be updated porperly and vob will be set to origin (0,0,0)!
	zVEC3 pos;
	itm->GetPositionWorld(pos.x, pos.y, pos.z);
	//manager->oCItemSaveRemoveEffect(item);
	//manager->oCItemSaveInsertEffect(item);
	world->RemoveVob(itm);
	copy->SetPositionWorld(pos);

	world->AddVob(copy);
	manager->oCItemSaveInsertEffect(copy);

	//logStream << "DynItemInst::restoreItem: Restored world item: " << instanceId << std::endl;
	//util::debug(&logStream);
}

static void restoreContainerItem(void* obj, void* param, oCItem* itm) {
	if (itm == NULL) return;

	ObjectManager* manager = ObjectManager::getObjectManager();
	int instanceId = manager->getInstanceId(*itm);
	int dummyID = zCParser::GetParser()->GetIndex("DII_DUMMY_ITEM");

	if (dummyID == instanceId) {
		bool test = false;
	}

	if (manager->IsModified(instanceId)) {
		manager->assignInstanceId(itm, instanceId);
	}

	if (itm->instanz >= 0) return;

	//ObjectManager* manager = ObjectManager::getObjectManager();

	if (!manager->updateItemAndAddit(itm)) {
		//Should not occur!!!!
		//logStream << "DynItemInst::restoreItem: Addit is null or was already visited!!!" << std::endl;
		//logStream << item->name.ToChar() << " : " << additId << std::endl;
		//util::logFault(&logStream);
		return;
	}
}


void DynItemInst::restoreDynamicInstances(oCGame* game) {
	logStream << "DynItemInst::restoreDynamicInstances: restore... "  << std::endl;
	util::logInfo(&logStream);

	if (levelChange && saveGameWriting)
	{
		// no need of restoring
		return;
	}

	denyMultiSlot = true;
	zCWorld* world = game->GetWorld();
	zCListSort<oCNpc>* npcList = world->GetNpcList();
	ObjectManager* manager = ObjectManager::getObjectManager();
	std::list<oCItem*> tempList;
	std::list<oCItem*> equippedItems;
	std::list<oCItem*>::iterator it;

	oCNpc* hero = oCNpc::GetHero();

	while(npcList != NULL) {
		oCNpc* npc = npcList->GetData();
		if (npc == NULL || levelChange && (npc == hero)) {
			npcList = npcList->GetNext();
			continue;
		}

		restoreItemsOfNpc(npc);
		npcList = npcList->GetNext();
	}

	//restore items in containers
	std::list<oCMobContainer*>* containers = manager->getMobContainers();

	while (containers->size() > 0) {
		oCMobContainer* container = containers->back();
		containers->pop_back();
		manager->callForAllContainerItems(restoreContainerItem, NULL, NULL, container);
	}
	delete containers;


	// World items have to be restored at last, as items in inventories and containers can be registered in the world
	// But they have to be restored differently!!!
	manager->callForAllWorldItems(restoreWorldItem, NULL, NULL);

	denyMultiSlot = false;

	logStream << "DynItemInst::restoreDynamicInstances: done." << std::endl;
	util::logInfo(&logStream);
}

bool DynItemInst::isSaveGameLoading()
{
	return saveGameIsLoading;
};


 int DynItemInst::createInstanceHook(void* pThis, int instanceId, void* source)
{
	zCPar_Symbol* symbol = zCParser::GetParser()->GetSymbol(instanceId);
	if (symbol == NULL)
	{
		logStream << "DynItemInst::createInstanceHook: symbol is null! InstanceId: " << instanceId << std::endl;
		util::debug(&logStream, Logger::Warning);
	}

	int result = createInstance(pThis, instanceId, source);;

	int instanceBegin = ObjectManager::getObjectManager()->getInstanceBegin();
	bool isTarget = symbol && (instanceId >= instanceBegin) && (instanceBegin > 0);
	if (isTarget)
	{
		oCItem* item = (oCItem*)source;
		ObjectManager* manager = ObjectManager::getObjectManager();
		manager->InitItemWithDynInstance(item, instanceId);	
		result = manager->getDynInstanceId(item);
	}

	return result;
}

void DynItemInst::oCGameLoadGameHook(void* pThis, int second, zSTRING const& worldName)
{
	logStream << "DynItemInst::oCGameLoadGameHook: load..."<< std::endl;
	util::logInfo(&logStream);
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	oCGameLoadGame(pThis, second, worldName);

	logStream << "DynItemInst::oCGameLoadGameHook: done." << std::endl;
	util::logInfo(&logStream);
}

struct LEVEL_CHANGE_PARAMS {
	oCNpc* hero;
	std::list<DynItemInst::LevelChangeBean*>* tempList;
	std::stringstream* logStream;
	int selectedSpellKey;
};

static void levelChangeForHero(void* obj, void* param, oCItem* itm) {
	if (itm != NULL)
	{
		if (itm->HasFlag(OCITEM_FLAG_EQUIPPED))
		{
			LEVEL_CHANGE_PARAMS* params = (LEVEL_CHANGE_PARAMS*)param;
			ObjectManager* manager = ObjectManager::getObjectManager();
			DynItemInst::LevelChangeBean* bean = new DynItemInst::LevelChangeBean();
			std::list<DynItemInst::LevelChangeBean*>* tempList = params->tempList;
			int selectedSpellKey = params->selectedSpellKey;
			oCNpc* hero = params->hero;
			std::stringstream* logStream = params->logStream;

			bean->item = itm;
			bean->dynamicInstanceId = manager->getInstanceId(*itm);
			bean->original_on_equip = itm->on_equip;
			bean->original_on_unequip = itm->on_unequip;
			bean->effectVob = itm->effectVob;
			bean->addit = new AdditMemory();
			bean->addit->spellKey = manager->getEquippedSpellKeyByItem(hero, itm);
			bean->addit->activeSpellItem = (selectedSpellKey == (itm->spell));
			*logStream << "item->effect: " << itm->effect.ToChar() << std::endl;
			*logStream << "item->effectVob: " << itm->effectVob << std::endl;
			util::debug(logStream);
			itm->on_equip = 0;
			itm->on_unequip = 0;

			tempList->push_back(bean);
		}
	}
}

static void levelChangeRemoveInv(void* obj, void* param, oCItem* itm) {
	if (itm != NULL)
	{
		ObjectManager* manager = ObjectManager::getObjectManager();
		if (manager->isDynamicInstance(itm->GetInstance()))
		{
			//remove item from world list!
			int* refCounter = manager->getRefCounter(itm);
			if (*refCounter < 0)
			{
				*refCounter += 1;
			}
			oCGame::GetGame()->GetWorld()->RemoveVob(itm);
		}
	}
}

void __thiscall DynItemInst::oCGameChangeLevelHook(void* pThis, zSTRING const & first, zSTRING const & second) {
	logStream << "DynItemInst::oCGameChangeLevelHook: change level..." << std::endl;
	util::logInfo(&logStream);

	levelChange = true;
	ObjectManager* manager = ObjectManager::getObjectManager();
	oCNpc* hero = oCNpc::GetHero();
	
	std::list<LevelChangeBean*> tempList;
	
	// runes/scrolls should have weaponMode == 7, but 0 (== no weapon readied) is here returned. This 
	// must have to do something thing the engine intern handling of the level change.
	// TODO: Try to fix it!
	int weaponMode = hero->GetWeaponMode();
	
	int readiedWeaponId = -1; 
	int munitionId = -1;
	bool munitionUsesRightHand = false;
	bool npcHasReadiedWeapon = false;
	oCItem* temp = NULL;
	oCItem** selectedSpellItem = &temp;
	std::map<int, oCItem*> equippedSpells;
	AdditMemory* addit = new AdditMemory();
	int selectedSpellKey = manager->getSelectedSpellKey(hero);

	LEVEL_CHANGE_PARAMS params = { hero, &tempList, &logStream, selectedSpellKey };

	/*std::function<void(oCItem*)> func = [&](oCItem* item)->void {
		if (item != NULL)
		{
			if (item->HasFlag(OCITEM_FLAG_EQUIPPED))
			{
				LevelChangeBean* bean = new LevelChangeBean();
				bean->item = item;
				bean->dynamicInstanceId = manager->getInstanceId(*item);
				bean->original_on_equip = item->on_equip;
				bean->original_on_unequip = item->on_unequip;
				bean->effectVob = item->effectVob;
				bean->addit = new AdditMemory();
				bean->addit->spellKey = manager->getEquippedSpellKeyByItem(hero, item);
				bean->addit->activeSpellItem = (selectedSpellKey == (item->spell));
				logStream << "item->effect: " << item->effect.ToChar() << std::endl;
				logStream << "item->effectVob: " << item->effectVob << std::endl;
				util::debug(&logStream);
				item->on_equip = 0;
				item->on_unequip = 0;

				tempList.push_back(bean);
			}
		}
	};*/

	manager->callForInventoryItems(levelChangeForHero, NULL, &params, hero);


	for (std::list<LevelChangeBean*>::iterator it = tempList.begin(); it != tempList.end(); ++it)
	{
		oCItem* item = (*it)->item;
		if (manager->isValidWeapon(weaponMode, item))
		{
			npcHasReadiedWeapon = true;
			readiedWeaponId = item->GetInstance();
			if (manager->isRangedWeapon(item))
			{
				munitionId = item->munition;
				munitionUsesRightHand = manager->munitionOfItemUsesRightHand(item);
			}
			if (weaponMode != 7)
				oCNpcEV_ForceRemoveWeapon(hero, item);
		}
		hero->Equip((*it)->item);
	}

	/*func = [&](oCItem* item)->void {
		if (item != NULL)
		{
			if (manager->isDynamicInstance(item->GetInstance()))
			{
				//remove item from world list!
				int* refCounter = manager->getRefCounter(item);
				if (*refCounter < 0)
				{
					*refCounter += 1;
				}
				oCGame::GetGame()->GetWorld()->RemoveVob(item);
			}
		}
	};*/

	manager->callForInventoryItems(levelChangeRemoveInv, NULL, NULL, hero);

	oCGameChangeLevel(pThis, first, second);

	hero = oCNpc::GetHero();

	for (std::list<LevelChangeBean*>::iterator it = tempList.begin(); it != tempList.end(); ++it)
	{
		restoreItemAfterLevelChange(hero, *it, weaponMode, readiedWeaponId, munitionId, munitionUsesRightHand,
			&equippedSpells, selectedSpellItem);

		//finally delete gracely
		SAFE_DELETE((*it)->addit);
		SAFE_DELETE(*it);
	}

	//TODO: 
	// - Handle runes and scrolls!
	equippedSpells.clear();
	/*for (auto it = equippedSpells.begin(); it != equippedSpells.end(); ++it)
	{
		int key = it->first;
		oCItem* item = it->second;
		oCMag_Book* magBook = hero->GetSpellBook();
		oCMag_BookNextRegisterAt(magBook, key);
		hero->Equip(item);
	}
	equippedSpells.clear();*/

	//After change level it doesn't work for spells to restore properly their weapon mode
	// so this call will always be useful. <- But let it there for later! TODO
	restoreSelectedSpell(hero, *selectedSpellItem);

	tempList.clear();

	//not needed? -> Yesm it is needed!
	initAdditMemory();
	checkReusableInstances();
	levelChange = false;

	logStream << "DynItemInst::oCGameChangeLevelHook: done." << std::endl;
	util::logInfo(&logStream);
}

int DynItemInst::oCItemMulitSlotHook(void* pThis)
{
	if (denyMultiSlot)
	{
		return 0;
	}
	return oCItemMulitSlot(pThis);
}

void DynItemInst::oCMobContainerOpenHook(void* pThis, oCNpc* npc)
{
	oCMobContainer* container = (oCMobContainer*) pThis;
	ObjectManager* manager = ObjectManager::getObjectManager();
	int address = (int)container->containList_next;
	zCListSort<oCItem>* listAddress = reinterpret_cast<zCListSort<oCItem>*>(address);
	zCListSort<oCItem>* list = listAddress;

	while(list != NULL) {
		oCItem* item = list->GetData();
		if (item == NULL) {
			list = list->GetNext();
			continue;
		}

		int instanceId = manager->getInstanceId(*item);
		bool change = item->instanz < 0;
		if (instanceId < 0)
		{
			instanceId = manager->getIndexByName(item->name);
			change = true;
		}

		if (change && manager->IsModified(instanceId)) {
			manager->assignInstanceId(item, instanceId);
		}

		list = list->GetNext();
	}

	oCMobContainerOpen(pThis, npc);

}

void DynItemInst::oCMag_BookSetFrontSpellHook(void* pThis, int number)
{
	logStream << "DynItemInst::oCMag_BookSetFrontSpellHook: number: " << number << std::endl;
	util::debug(&logStream);
	oCMag_BookSetFrontSpell((oCMag_Book*)pThis, number);
}

zCVisual* DynItemInst::zCVisualLoadVisual(zSTRING const& name)
{
	XCALL(ZCVISUAL_LOAD_VISUAL);
}

zCListSort<oCItem>* DynItemInst::getInvItemByInstanceId(oCNpcInventory* inventory, int instanceId)
{
	inventory->UnpackCategory();
	ObjectManager* manager = ObjectManager::getObjectManager();
	zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
	while(list != NULL) {
		oCItem* item = list->GetData();
		if (item != NULL && manager->getInstanceId(*item) == instanceId)
		{
			return list;
		}
		list = list->GetNext();
	}

	return NULL;
};

oCItem* DynItemInst::getInvItemByInstanceId2(oCNpcInventory* inventory, int instanceId)
{
	inventory->UnpackCategory();
	int itemNumber = inventory->GetNumItemsInCategory();
	for (int i = 0; i < itemNumber; ++i)
	{
		oCItem* itm = inventory->GetItem(i);
		if (itm != NULL && itm->GetInstance() == instanceId)
		{
			return itm;
		}
	}

	return NULL;
}



std::string DynItemInst::getClearedWorldName(zSTRING const & worldName) {
	std::string text (const_cast<char*>(const_cast<zSTRING &>(worldName).ToChar()));
	std::vector<std::string> splits;
	util::split(splits, text, '/');
	text = splits.back();
	splits.clear();
	util::split(splits, text, '\\');
	text = splits.back();
	splits.clear();
	util::split(splits, text, '.');
	//now only two elements are in splits (the file name and the file type (*.ZEN))
	text = splits.front();
	return text;
}

void DynItemInst::loadDynamicInstances()
{
	logStream << "DynItemInst::loadDynamicInstances: load dii instances..." << std::endl;
	util::logInfo(&logStream);
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	std::string instances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;
	std::string saveGameDir = manager->getCurrentDirectoryPath();//manager->getSaveGameDirectoryPath(saveGameSlotNumber);
	std::string fileName = saveGameDir + instances;
	manager->loadNewInstances((char*)fileName.c_str());
	logStream << "DynItemInst::loadDynamicInstances: done." << std::endl;
	util::logInfo(&logStream);
}

void DynItemInst::initAdditMemory()
{
	logStream << "DynItemInst::initAdditMemory: init..." << std::endl;
	util::logInfo(&logStream);

	ObjectManager* manager = ObjectManager::getObjectManager();
	std::string saveGameDir = manager->getCurrentDirectoryPath();

	std::string worldName = oCGame::GetGame()->GetGameWorld()->GetWorldName().ToChar();
	std::string saveFile = SAVE_ITEM_ADDIT + worldName + SAVE_ITEM_FILE_EXT; 
	std::string additSaveGameFilePath = saveGameDir + saveFile;

	std::string heroData; 
	heroData= saveGameDir + SAVE_ITEM_HERO_DATA + SAVE_ITEM_FILE_EXT;
	manager->loadHeroData(const_cast<char*>(heroData.c_str()));
	manager->loadWorldObjects(const_cast<char*>(additSaveGameFilePath.c_str()));

	restoreDynamicInstances(oCGame::GetGame());
	manager->removeAllAdditionalMemory();

	//zCParser* parser = zCParser::GetParser();
	//parser->CallFunc(parser->GetIndex("DII_AFTER_LOADING_FINISHED_CALLBACK"));
	logStream << "DynItemInst::initAdditMemory: done." << std::endl;
	util::logInfo(&logStream);
}

void DynItemInst::equipRangedWeapon(oCItem* item, oCNpcInventory* inventory, bool munitionUsesRightHand)
{
	zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, item->munition);
	oCItem* munition = list->GetData();
	int arrowId = ObjectManager::getObjectManager()->getInstanceId(*munition);
	zCListSort<oCItem>* list2 = list->GetNext();
	oCItem* munition2 = NULL;
	oCNpc* owner = inventory->GetOwner();

	if (list2)
	{
		munition2 = list2->GetData();
	}

	if (munition2 && munition2->instanz > 1)
	{
		logStream << "DynItemInst::updateRangedWeapon: munition2->instanz > 1!";
		util::logFault(&logStream);
	}

	int amount = munition->instanz;
	inventory->Remove(munition, munition->instanz);
	if (munition2)
	{
		inventory->Remove(munition2, munition2->instanz);
		amount += munition2->instanz;
	}
	munition = oCObjectFactory::GetFactory()->CreateItem(munition->GetInstance());
	munition->instanz = amount;
	inventory->Insert(munition);
	munition = getInvItemByInstanceId(inventory, arrowId)->GetData();

	if (munitionUsesRightHand)
	{
		oCNpcSetRightHand(owner, munition->SplitItem(1));
	} else
	{
		oCNpcSetLeftHand(owner, munition->SplitItem(1));
	}
}

void DynItemInst::resetInstanceNameStruct()
{
	instanceNames.nearFightCounter = 1;
	instanceNames.distanceFightCounter = 1;
	instanceNames.runeCounter = 1;
	instanceNames.otherCounter = 1;
}


void DynItemInst::restoreSelectedSpell(oCNpc* npc, oCItem* selectedSpellItem)
{
	if (!selectedSpellItem) return;
	
	logStream << "DynItemInst::restoreSelectedSpell: SET SELECTED SPELL KEY!!!!" << std::endl;
	util::debug(&logStream);
	oCMag_Book* magBook = npc->GetSpellBook();
	int itemSpellKey = oCMag_BookGetKeyByItem(magBook, selectedSpellItem);
	int noOfSpellKey = oCMag_GetNoOfSpellByKey(magBook, itemSpellKey);
	oCSpell* spell = oCMag_BookGetSpellByKey(magBook, itemSpellKey);


	logStream << "DynItemInst::restoreSelectedSpell: itemSpellKey = " << itemSpellKey << std::endl;
	util::debug(&logStream);
	logStream << "DynItemInst::restoreSelectedSpell: itemSpellKey = " << spell << std::endl;
	util::debug(&logStream);
	int weaponMode = oCNpcGetWeaponMode(npc);
	if (weaponMode == 7)
	{
		oCNpcEV_ForceRemoveWeapon(npc, selectedSpellItem);
		oCMag_BookKillSelectedSpell(magBook);
		oCMag_BookSetFrontSpell(magBook, noOfSpellKey - 1);
		oCNpcSetWeaponMode2(npc, weaponMode);
	}
}

struct RESTORE_NPC_ITEM_PARAMS {
	std::stringstream* logStream;
	oCNpcInventory* inventory;
	std::map<int, oCItem*>* equippedSpells;
	oCItem** selectedSpellItem;
};

void DynItemInst::restoreSpecificNpcItem(void* obj, void* param, oCItem* itm) {
	if (itm == NULL) return;
	if (itm->instanz >= 0) return;

	ObjectManager* manager = ObjectManager::getObjectManager();
	RESTORE_NPC_ITEM_PARAMS* params = (RESTORE_NPC_ITEM_PARAMS*)param;
	std::stringstream* logStream = params->logStream;

	int additID = manager->getAdditId(*itm);

	if (!manager->updateItemAndAddit(itm)) {
		//TODO ERROR
		return;
	}

	AdditMemory* addit = manager->getAddit(additID);

	util::assertDIIRequirements(addit != NULL, "DynItemInst::restoreSpecificNpcItem: addit != NULL");

	bool equipped = false;

	if (addit != NULL) {
		equipped = (addit->flags & OCITEM_FLAG_EQUIPPED) != 0;
	}

	if (equipped)
	{
		itm->SetFlag(OCITEM_FLAG_EQUIPPED);
		//equippedItems.push_back(item);
		int instanceId = itm->GetInstance();
		*logStream << "DynItemInst::restoreSpecificNpcItem: item with following id is marked as equipped: " << instanceId << std::endl;
		util::debug(logStream);
		//control that instanceId is that of a marked equipped item
		int checkInstanceId = zCParser::GetParser()->GetIndex(zSTRING(instanceNames.base.c_str()));
		util::assertDIIRequirements(instanceId != checkInstanceId, "DynItemInst::restoreSpecificNpcItem: instanceId != checkInstanceId");

		restoreSpecificNpcItemStub(itm, 
			params->inventory, 
			addit,
			params->equippedSpells, 
			params->selectedSpellItem);
	}
	else
	{
		restoreSpecificNpcItemStub(itm, params->inventory, addit);
	}
}

void DynItemInst::equipDynamicItem(void * obj, void * param, oCItem * itm)
{
	if (itm == NULL) return;
	ObjectManager* manager = ObjectManager::getObjectManager();
	RESTORE_NPC_ITEM_PARAMS* params = (RESTORE_NPC_ITEM_PARAMS*)param;

	int instanceID = itm->GetInstance();
	if (!manager->isDynamicInstance(instanceID)) return;

	bool equipped = (itm->flags & OCITEM_FLAG_EQUIPPED) != 0;

	if (!equipped) return;

	if (itm->HasFlag(512)) return; //item mustn't be a rune


	logStream << "DynItemInst::equipDynamicItem: Restore equipped item..." << std::endl;
	util::debug(&logStream);

	oCNpc* owner = params->inventory->GetOwner();
	int weaponMode = oCNpcGetWeaponMode(owner);

	// only one equipped item can be the readied weapon for all weapon modes.
	// So if the equipped weapon is valid for a specific weapon mode
	// we have found the readied weapon!
	if (manager->isValidWeapon(weaponMode, itm))
	{
		logStream << "DynItemInst::equipDynamicItem: Force to remove weapon..." << std::endl;
		util::debug(&logStream);
		oCNpcEV_ForceRemoveWeapon(owner, itm);
	}

	int amount = itm->instanz;
	if (amount != 1)
	{
		logStream << "DynItemInst::equipDynamicItem: amount > 1!" << std::endl;
		logStream << "item instance id: " << itm->GetInstance() << std::endl;
		logStream << "item->name: " << itm->name.ToChar() << std::endl;
		util::logFault(&logStream);
	}

	zCListSort<oCItem>* list = getInvItemByInstanceId(params->inventory, instanceID);
	oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceID);

	int slotNumber = manager->getSlotNumber(params->inventory, itm);
	//util::assertDIIRequirements(slotNumber >= 0, "slotNumber >= 0");
	logStream << "DynItemInst::equipDynamicItem: slotnumber= " << slotNumber << std::endl;
	logStream << "item->description= " << itm->description.ToChar() << std::endl;
	logStream << "item->GetInstance()= " << itm->GetInstance() << std::endl;
	logStream << "item->instanz= " << itm->instanz << std::endl;
	logStream << "copy->description= " << copy->description.ToChar() << std::endl;
	logStream << "copy->GetInstance()= " << copy->GetInstance() << std::endl;
	logStream << "copy->instanz= " << copy->instanz << std::endl;
	util::debug(&logStream);

	itm->ClearFlag(constants::OCITEM_FLAG_EQUIPPED);
	params->inventory->RemoveByPtr(itm, itm->instanz);

	//store some attribute to search for the copy after it was inserted into the inventory
	//int copyStoreValue = copy->instanz;
	//assign any value that will be unique
	//int searchValue = -6666666;
	//copy->instanz = searchValue;

	copy = params->inventory->Insert(copy);
	util::assertDIIRequirements(copy != NULL, "item to insert shouldn't be null!");

	
	int equipFunction = copy->on_equip;
	
	//Deny invocation of equip function
	copy->on_equip = 0;

	//clear equip flag, so that the item will definitely be equipped
	copy->ClearFlag(constants::OCITEM_FLAG_EQUIPPED);

	// Equip the item; multislot items will get split; We needn't have to give care for the on_equip function, 
	// has the splitted item will be initiliazed by the Dynamic instance!
	owner->Equip(copy);


	//restore function

	logStream << "DynItemInst::equipDynamicItem: item is now equipped!" << std::endl;
	logStream << "DynItemInst::equipDynamicItem: Weapon mode: " << weaponMode << std::endl;
	util::debug(&logStream);

	copy->on_equip = equipFunction;
	oCNpcSetWeaponMode2(owner, weaponMode);  //3 for one hand weapons


	//TODO: Drawn munition is duplicated (one munition item too much)

	// Is readied weapon a bow?
	if (copy->HasFlag(1 << 19) && weaponMode == 5)
	{
		logStream << "DynItemInst::equipDynamicItem: Bow is readied!" << std::endl;
		logStream << "DynItemInst::equipDynamicItem: Weapon mode: " << weaponMode << std::endl;
		util::debug(&logStream);

		equipRangedWeapon(copy, params->inventory, true);
	}

	// Is readied weapon a crossbow?
	else if (copy->HasFlag(1 << 20) && weaponMode == 6)
	{
		logStream << "DynItemInst::equipDynamicItem: Crossbow is readied!" << std::endl;
		logStream << "DynItemInst::equipDynamicItem: Weapon mode: " << weaponMode << std::endl;
		util::debug(&logStream);

		equipRangedWeapon(copy, params->inventory, false);
	}
}

void DynItemInst::restoreItemsOfNpc(oCNpc * npc)
{
	std::list<oCItem*> tempList, equippedItems;
	ObjectManager* manager = ObjectManager::getObjectManager();
	oCNpcInventory* inventory = npc->GetInventory();
	oCItem* temp = NULL;
	oCItem** selectedSpellItem = &temp;
	std::map<int, oCItem*> equippedSpells;

	RESTORE_NPC_ITEM_PARAMS params = { &logStream, inventory, &equippedSpells, selectedSpellItem };

	// restore dynmamic items
	manager->callForInventoryItems(restoreSpecificNpcItem, NULL, &params, npc);

	// sort equipped dynamic items after all dynamic items are restored
	bool originalMultiSlotSetting = denyMultiSlot;
	denyMultiSlot = false;
	manager->callForInventoryItems(equipDynamicItem, NULL, &params, npc);
	denyMultiSlot = originalMultiSlotSetting;

	for (std::map<int, oCItem*>::iterator it = equippedSpells.begin(); it != equippedSpells.end(); ++it)
	{
		int key = it->first;
		oCItem* item = it->second;
		oCMag_Book* magBook = npc->GetSpellBook();
		oCMag_BookNextRegisterAt(magBook, key);
		npc->Equip(item);
	}
	equippedSpells.clear();

	restoreSelectedSpell(npc, *selectedSpellItem);
}

oCItem* DynItemInst::restoreItemAfterLevelChange(oCNpc* npc, LevelChangeBean* bean, int weaponMode, int readiedWeaponId,
	int munitionId, bool munitionUsesRightHand, std::map<int, oCItem*>* equippedSpells, oCItem** selectedSpellItem)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	oCNpcInventory* inv = npc->GetInventory();
	inv->UnpackAllItems();
	zCListSort<oCItem>* node = getInvItemByInstanceId(inv, bean->dynamicInstanceId);
	oCItem* item = NULL;
	if (node) item = node->GetData();
	if (!item) 	{
		logStream << "DynItemInst::oCGameChangeLevelHook: inventory item to be equipped is null!" << std::endl;
		util::logFault(&logStream);
	} else {
		logStream << "DynItemInst::oCGameChangeLevelHook: item to equip: " << std::endl;
		logStream << "item instance id: " << item->GetInstance() << std::endl;
		logStream << "item->name.ToChar(): " << item->name.ToChar() << std::endl;
		logStream << "item->instanz: " << item->instanz << std::endl;
		util::logInfo(&logStream);


		bool npcHasReadiedWeapon = weaponMode > 0;

		if (npcHasReadiedWeapon && (bean->dynamicInstanceId == readiedWeaponId))
		{
			// drawSilently handles on_equip functions properly, no need to unset it before!
			manager->drawWeaponSilently(npc, weaponMode, readiedWeaponId,
				munitionId, munitionUsesRightHand, equippedSpells, selectedSpellItem, bean->addit, false);
			
			// item is already equipped; so we can leave
			return item;
		}

		item->on_equip = 0;
		item->on_unequip = 0;

		if (item && item->HasFlag(512)) // is a rune/scroll
		{
			logStream << "DynItemInst::oCGameChangeLevelHook: selected weapon is a magic thing!" << std::endl;
			util::debug(&logStream);
			oCMag_Book* magBook = oCNpcGetSpellBook(npc);
			if (bean->addit->spellKey > 0)
			{

				oCMag_BookDeRegisterItem(magBook, item);
				oCMag_BookNextRegisterAt(magBook, bean->addit->spellKey);

				//item->SetFlag(OCITEM_FLAG_EQUIPPED);
			}
			if (bean->addit->spellKey >= 0)
			{
				equippedSpells->insert(std::pair<int, oCItem*>(bean->addit->spellKey, item));
			}
		}

		if (!item->HasFlag(OCITEM_FLAG_EQUIPPED)) {
			npc->Equip(item);

			//remove added item effects if item had before none.
			if (!bean->effectVob)
				manager->oCItemSaveRemoveEffect(item);
		}

		//update equip functions
		item->on_equip = bean->original_on_equip;
		item->on_unequip = bean->original_on_unequip;
	}

	return item;
}

void DynItemInst::handleEquippedMagicItem(oCItem* item, oCNpcInventory* inventory, AdditMemory* addit, 
	int instanceId, std::map<int, oCItem*>* equippedSpells, oCItem** activeSpellItem)
{

	if (item == NULL) return;
	if (!item->HasFlag(512)) return; // Magic 

	oCNpc* owner = inventory->GetOwner();

	logStream << "DynItemInst::restoreItem: Readied weapon is a magic thing!" << std::endl;
	util::debug(&logStream);
	oCMag_Book* magBook = oCNpcGetSpellBook(owner);
	magBook = oCNpcGetSpellBook(owner);
	int itemSpellKey = oCMag_BookGetKeyByItem(magBook, item);
	if (itemSpellKey <= 7)
	{
		oCMag_BookDeRegisterItem(magBook, item);
		oCMag_BookNextRegisterAt(magBook, itemSpellKey - 1);
	}
	if (addit->spellKey >= 0)
	{
		if (!equippedSpells)
		{
			logStream << "DynItemInst::restoreItem: equippedSpells is null!" << std::endl;
			util::debug(&logStream, Logger::Warning);
		}
		else
		{
			equippedSpells->insert(std::pair<int, oCItem*>(addit->spellKey, item));
		}
	}

	magBook = oCNpcGetSpellBook(owner);
	if (magBook)
	{
		if (addit->activeSpellItem && activeSpellItem)
		{
			*activeSpellItem = item;
		}
	}
}