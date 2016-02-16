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

const std::string DynItemInst::SAVE_ITEM_FILE_EXT = ".SAV";
const std::string DynItemInst::SAVE_ITEM_INSTANCES  = "DII_INSTANCES";
const std::string DynItemInst::SAVE_ITEM_ADDIT = "DII_ADDIT_";
const std::string DynItemInst::SAVE_ITEM_HERO_DATA = "DII_HERO_DATA";
const std::string DynItemInst::FILE_PATERN = "\\DII_*";
bool DynItemInst::denyMultiSlot = false;
bool DynItemInst::levelChange = false;
bool DynItemInst::saveGameIsLoading = false;

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



void DynItemInst::hookModule()
{
	loadSavegame = (LoadSavegame) (LOAD_SAVEGAME_ADDRESS);
	writeSavegame = (WriteSavegame) (WRITE_SAVEGAME_ADDRESS);
	oCItemGetValue = (OCItemGetValue) (OCITEM_GET_VALUE_ADDRESS);
	//zCPar_SymbolTableGetSymbol = (ZCPar_SymbolTableGetSymbol) (ZCPAR_SYMBOL_TABLE_GETSYMBOLE);
	//zCPar_SymbolTableGetSymbolString = (ZCPar_SymbolTableGetSymbolString) (ZCPAR_SYMBOL_TABLE_GETSYMBOLE_STRING);
	//zCPar_SymbolTableGetIndex = (ZCPar_SymbolTableGetIndex)(ZCPAR_SYMBOL_TABLE_GETINDEX);
	//zCParserGetIndex = (ZCParserGetIndex)(ZCPARSER_GETINDEX);
	createInstance = (CreateInstance) (ZCPARSER_CREATE_INSTANCE);
	oCGameLoadGame = (OCGameLoadGame) OCGAME_LOAD_GAME_ADDRESS;

	//oCGameLoadWorld = reinterpret_cast<OCGameLoadWorld>(OCGAME_LOAD_WORLD_ADDRESS);
	oCGameChangeLevel = reinterpret_cast<OCGameChangeLevel>(OCGAME_CHANGE_LEVEL_ADDRESS);
	oCItemMulitSlot = (OCItemMulitSlot) OCITEM_MULTI_SLOT;
	oCMobContainerOpen = (OCMobContainerOpen) OCMOB_CONTAINER_OPEN;

	HookManager* hookManager = HookManager::getHookManager();
	hookManager->addFunctionHook((LPVOID*)&loadSavegame, loadSavegameHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&writeSavegame, writeSavegameHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHookNaked, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbol, zCPar_SymbolTableGetSymbolHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbolString, zCPar_SymbolTableGetSymbolStringHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetIndex, zCPar_SymbolTableGetIndexHook,  moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCParserGetIndex, zCParserGetIndexHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&createInstance, createInstanceHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameChangeLevel, oCGameChangeLevelHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHookNaked, moduleDesc);

	denyMultiSlot = true;
	loadDynamicInstances();
	initAdditMemory();
	denyMultiSlot = false;
}

void DynItemInst::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&loadSavegame, loadSavegameHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&writeSavegame, writeSavegameHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHookNaked, moduleDesc);
	//hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbol, zCPar_SymbolTableGetSymbolHook, moduleDesc);
	//hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbolString, zCPar_SymbolTableGetSymbolStringHook, moduleDesc);
	//hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetIndex, zCPar_SymbolTableGetIndexHook,moduleDesc);
	//hookManager->removeFunctionHook((LPVOID*)&zCParserGetIndex, zCParserGetIndexHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&createInstance, createInstanceHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameChangeLevel, oCGameChangeLevelHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHookNaked, moduleDesc);
};


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
		LEGO_HOOKENGINE_PREAMBLE
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
	if (result == nullptr)
	{
		result = ObjectManager::getObjectManager()->getSymbolByIndex(index);
	}

	return result;
}

zCPar_Symbol* DynItemInst::zCPar_SymbolTableGetSymbolStringHook(void* pThis, zSTRING const & symbolName)
{
	zCPar_Symbol* result = ObjectManager::getObjectManager()->getSymbolByName(symbolName);
	if (result == nullptr)
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
	saveGameIsLoading = true;
	denyMultiSlot = true;
	ObjectManager::getObjectManager()->releaseInstances();
	loadSavegame(pThis, saveGameSlotNumber, b);
	loadDynamicInstances();
	initAdditMemory();
	denyMultiSlot = false;
	saveGameIsLoading = false;
};


 void DynItemInst::writeSavegameHook(void* pThis,int saveGameSlotNumber, int b)
{   
	oCGame* game = oCGame::GetGame();
	zCWorld* world = game->GetWorld();
	oCWorld* ocworld = game->GetGameWorld();
	ObjectManager* manager = ObjectManager::getObjectManager();
	
	zCListSort<oCNpc>* npcList = world->GetNpcList();

	DynItemInst::denyMultiSlot = true;

	oCNpc* hero = oCNpc::GetHero();
	while(npcList != nullptr) {
		oCNpc* npc = npcList->GetData();
		if (npc == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}
		bool isHero = npc == hero;
		if (isHero && DynItemInst::levelChange) {
			npcList = npcList->GetNext();
			continue;
		}
		oCNpcInventory* inventory = npc->GetInventory();
		if (inventory == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}
		inventory->UnpackAllItems();
		zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
		while(list != nullptr) {
			oCItem* item = list->GetData();
			int id = modifyItemForSaving(item, isHero);
			int equiped = id && item->HasFlag(OCITEM_FLAG_EQUIPPED);
			if (id) manager->createAdditionalMemory(item, id, isHero);
			if (equiped)
			{
				logStream << "item should be unequiped now..." << std::endl;
				Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);
				//npc->Equip(item);
				//item->SetFlag(OCITEM_FLAG_EQUIPPED);
				/*typedef int(__thiscall* OCNpcGetWeaponMode)(oCNpc*);
				OCNpcGetWeaponMode oCNpcGetWeaponMode = (OCNpcGetWeaponMode)0x00738C40;
				int weaponMode = oCNpcGetWeaponMode(npc);

				typedef int(__thiscall* OCNpcEV_ForceRemoveWeapon)(oCNpc* pThis, void*);
				OCNpcEV_ForceRemoveWeapon oCNpcEV_ForceRemoveWeapon = (OCNpcEV_ForceRemoveWeapon)0x0074EC40;
				oCNpcEV_ForceRemoveWeapon(npc, nullptr);

				typedef struct
				{
					oCItem* item;
					int data[100];
				} TEST;

				typedef void(__thiscall* OCNpcSetToFightMode)(oCNpc* pThis, void*);
				OCNpcSetToFightMode oCNpcSetToFightMode = (OCNpcSetToFightMode)0x0074CC10;
				TEST* test = new TEST();
				test->item = item;
				oCNpcSetToFightMode(npc, test);
				//delete test;*/
			}

			list = list->GetNext();
		}
		npcList = npcList->GetNext();
	}
	
	zCListSort<oCItem>* itemList = world->GetItemList();
	while(itemList != nullptr) {
		oCItem* item = itemList->GetData();
		int id = modifyItemForSaving(item, false);
		if (id) manager->createAdditionalMemory(item, id, false);
		itemList = itemList->GetNext();
	}

	DynItemInst::denyMultiSlot = false;

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

	util::copyFileTo(saveGameDir + std::string("\\") +  saveInstances, currentDir  + 
		std::string("\\") + saveInstances);
	util::copyFileTo(saveGameDir + std::string("\\") + saveAddit, currentDir  + 
		std::string("\\") + saveAddit);
	util::copyFileTo(saveGameDir + std::string("\\") + heroData, currentDir  + 
		std::string("\\") + heroData);

	restoreDynamicInstances(game);
	manager->removeAllAdditionalMemory();	
};

 void DynItemInst::restoreItem(oCItem* item,  oCNpcInventory* inventory) {
	if (item == nullptr) return;
	if (item->instanz < 0) {
		ObjectManager* manager = ObjectManager::getObjectManager();
		int additId = -item->instanz;
		AdditMemory* addit = manager->getAddit(additId);
		if (addit == nullptr) {
			logStream << "DynItemInst::modifyItem: Warning: Addit is null!!!" << std::endl;
			logStream << item->name.ToChar() << " : " << additId << std::endl;
			Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
			return;
		}

		int instanceId = addit->instanceId;
		int instanz = addit->instanz;
		item->instanz = instanz;

		zCWorld* world = oCGame::GetGame()->GetWorld();

		// is item located in the world?
		if (inventory == nullptr)
		{
			manager->setInstanceId(item, instanceId);
			manager->assignInstanceId(item, instanceId);
			item->SetVisual(zCVisualLoadVisual(item->visual));
			return;
		}

		// is the item equipped?
		int equipped = item->HasFlag(OCITEM_FLAG_EQUIPPED);


		if (equipped)
		{
			logStream << "Restore equipped item..." << std::endl;
			Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);
			//DynItemInst::denyMultiSlot = false;
			oCNpc* owner = inventory->GetOwner();

			//owner->EquipItem(item);

			typedef int(__thiscall* OCNpcGetWeaponMode)(oCNpc*);
			OCNpcGetWeaponMode oCNpcGetWeaponMode = (OCNpcGetWeaponMode)0x00738C40;
			int weaponMode = oCNpcGetWeaponMode(owner);

			/*typedef void(__thiscall* OCItemInitByScript)(void* pThis, int, int);
			OCItemInitByScript oCItemInitByScript = (OCItemInitByScript)0x00711BD0;
			oCItemInitByScript(item, instanceId, instanz);*/

			typedef int(__thiscall* OCNpcEV_ForceRemoveWeapon)(oCNpc* pThis, void*);
			OCNpcEV_ForceRemoveWeapon oCNpcEV_ForceRemoveWeapon = (OCNpcEV_ForceRemoveWeapon) 0x0074EC40;
			
			typedef oCItem*(__thiscall* OCNpcGetWeapon)(oCNpc* pThis);
			OCNpcGetWeapon oCNpcGetWeapon = (OCNpcGetWeapon)0x007377A0;

			typedef void*(__thiscall* OCNpcGetInvSlot)(oCNpc*, zSTRING const &);
			OCNpcGetInvSlot oCNpcGetInvSlot = (OCNpcGetInvSlot)0x00749AE0;

			typedef oCVob*(__thiscall* OCNpcRemoveFromSlot)(oCNpc*, void*, int, int);
			OCNpcRemoveFromSlot oCNpcRemoveFromSlot = (OCNpcRemoveFromSlot)0x0074A340;

			typedef oCItem*(__thiscall* OCNpcPutInInv)(oCNpc*, oCItem*);
			OCNpcPutInInv oCNpcPutInInv = (OCNpcPutInInv)0x00749350;

			typedef int(__thiscall* OCNpcSetRightHand)(oCNpc*, oCVob*);
			OCNpcSetRightHand oCNpcSetRightHand = (OCNpcSetRightHand)0x0073B1C0;

			//oCGame::GetGame()->GetWorld()->AddVob(item);
			//if (oCNpcGetWeapon(owner) == item)
			//{
				oCNpcEV_ForceRemoveWeapon(owner, nullptr);
			//}

			typedef void(__thiscall* OCNpcSetToFightMode)(oCNpc* pThis, void*);
			OCNpcSetToFightMode oCNpcSetToFightMode = (OCNpcSetToFightMode)0x0074CC10;

			typedef struct
			{
				oCItem* item;
				int data[100];
			} TEST;

			/*//1h weapons
			if (item->HasFlag(1<<14) || item->HasFlag(1 << 15))
			{
				void* rightHand = oCNpcGetInvSlot(owner, "NPC_NODE_RIGHTHAND");
				oCVob* rightItem = oCNpcRemoveFromSlot(owner, rightHand, 0, 1);
				if (rightItem == nullptr)
				{
					logStream << "rightItem is null!" << std::endl;
					Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);
				}

				item = oCNpcPutInInv(owner, item);
				oCNpcSetRightHand(owner, nullptr);
				TEST* test = new TEST();
				typedef void(__thiscall* OCMsgWeaponConstructor)(void*);
				OCMsgWeaponConstructor oCMsgWeaponConstructor = (OCMsgWeaponConstructor)0x00766710;
				oCMsgWeaponConstructor(test);
				//test->item = copy;
				oCNpcSetToFightMode(owner, test);
			}*/



			//oCItemInitByScript(item, instanceId, item->instanz);
			//item->SetFlag(OCITEM_FLAG_EQUIPPED);
			
			int amount = item->instanz;
			inventory->Remove(item, item->instanz);
			zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, instanceId);
			oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceId);
			if (list != nullptr)
			{
				item = list->GetData();
				copy->instanz = item->instanz + amount;
				inventory->Remove(item, item->instanz);
			}
			world->AddVob(copy);
			inventory->Insert(copy);
			copy = getInvItemByInstanceId(inventory, instanceId)->GetData();
			//copy->SetFlag(OCITEM_FLAG_EQUIPPED);
			owner->Equip(copy);

			logStream << "item is now equipped!" << std::endl;
			logStream << "Weapon mode: " << weaponMode << std::endl;
			Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);

			copy = getInvItemByInstanceId(inventory, instanceId)->GetData();

			//typedef int(__thiscall* OCNpcDoSetToFightMode)(oCNpc*, oCItem*);
			//OCNpcDoSetToFightMode oCNpcDoSetToFightMode = (OCNpcDoSetToFightMode)0x00745350;
			//oCNpcDoSetToFightMode(owner, copy);
			
			/*typedef int(__thiscall* OCNpcInitByScript)(oCNpc*, int, int);
			OCNpcInitByScript oCNpcInitByScript = (OCNpcInitByScript)0x0072EE70;
			oCNpcInitByScript(owner, owner->GetInstance(), owner->instanz);*/


			/*typedef void(__thiscall* OCNpcDoRemoveMunition)(oCNpc*);
			OCNpcDoRemoveMunition oCNpcDoRemoveMunition = (OCNpcDoRemoveMunition)0x00744470;
			oCNpcDoRemoveMunition(owner);*/

			/*if (oCNpcGetWeapon(owner) != item )
			{
				return;
			}*/

			typedef int(__thiscall* OCNpcSetWeaponMode2)(oCNpc*, int);
			OCNpcSetWeaponMode2 oCNpcSetWeaponMode2 = (OCNpcSetWeaponMode2)0x00738E80;
			oCNpcSetWeaponMode2(owner, weaponMode);  //3 for one hand weapons


			typedef int(__thiscall* OCNpcIsMunitionAvailable)(oCNpc*, oCItem*);
			OCNpcIsMunitionAvailable oCNpcIsMunitionAvailable = (OCNpcIsMunitionAvailable)0x0073C6E0;

			typedef int(__thiscall* OCNpcSetLeftHand)(oCNpc*, oCVob*);
			OCNpcSetLeftHand oCNpcSetLeftHand = (OCNpcSetLeftHand)0x0073B0C0;

			int munitionAvailable = oCNpcIsMunitionAvailable(owner, copy);

			// Is readied weapon a bow?
			if (copy->HasFlag(1 << 19) && munitionAvailable && weaponMode == 5)
			{
				logStream << "Bow is readied!" << std::endl;
				logStream << "Weapon mode: " << weaponMode << std::endl;
				Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);

				oCItem* arrow = getInvItemByInstanceId(inventory, copy->munition)->GetData();
				arrow->instanz -= 1;

				oCItem* arrow2 = oCObjectFactory::GetFactory()->CreateItem(arrow->GetInstance());

				oCNpcSetRightHand(owner, arrow2);

			}else if (weaponMode == 5) {
				logStream << "Bow is readied but item is no bow!" << std::endl;
				Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);

				oCItem* readiedWeapon = oCNpcGetWeapon(owner);
				if (readiedWeapon != nullptr)
				{
					zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, readiedWeapon->munition);
					if (list)
					{
						oCItem* arrow = list->GetData();
						arrow->instanz -= 1;
						oCItem* arrow2 = oCObjectFactory::GetFactory()->CreateItem(arrow->GetInstance());

						oCNpcSetRightHand(owner, arrow2);
					}
				}
			// crossbow
			} else if (copy->HasFlag(1 << 20) && munitionAvailable && weaponMode == 6)
			{
				logStream << "Crossbow is readied!" << std::endl;
				logStream << "Weapon mode: " << weaponMode << std::endl;
				Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);

				oCItem* bolt = getInvItemByInstanceId(inventory, copy->munition)->GetData();
				bolt->instanz -= 1;

				oCItem* bolt2 = oCObjectFactory::GetFactory()->CreateItem(bolt->GetInstance());

				oCNpcSetLeftHand(owner, bolt2);
			} else if (weaponMode == 6)
			{
				logStream << "Crossbow is readied but item is no crossbow!" << std::endl;
				Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);

				oCItem* readiedWeapon = oCNpcGetWeapon(owner);
				if (readiedWeapon != nullptr)
				{
					zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, readiedWeapon->munition);
					if (list)
					{
						oCItem* bolt = list->GetData();
						bolt->instanz -= 1;
						oCItem* bolt2 = oCObjectFactory::GetFactory()->CreateItem(bolt->GetInstance());

						oCNpcSetLeftHand(owner, bolt2);
					}
				}
			}

			DynItemInst::denyMultiSlot = true;
			
			/*oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceId);
			world->AddVob(copy);
			copy->instanz = instanz;
			oCNpc* owner = inventory->GetOwner();
			owner->Equip(item);
			inventory->RemoveByPtr(item, instanz);
			DynItemInst::denyMultiSlot = false;
			world->AddVob(copy);
			inventory->Insert(copy);
			DynItemInst::denyMultiSlot = true;
			copy = getInvItemByInstanceId(inventory, instanceId)->GetData();

			owner->EquipItem(copy);*/

			logStream << "Restored equipped item!" << std::endl;
			Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);
			return;
		}


		typedef void(__thiscall* OCItemInitByScript)(void* pThis, int, int);
		OCItemInitByScript oCItemInitByScript = (OCItemInitByScript)0x00711BD0;

		oCItemInitByScript(item, instanceId, item->instanz);

		inventory->Remove(item, item->instanz);
		inventory->Insert(item);
	}
}

zSTRING instanceNames[] = {"DII_DUMMY_ITEM2", "DII_DUMMY_ITEM3", "DII_DUMMY_ITEM4", "DII_DUMMY_ITEM5", "DII_DUMMY_ITEM6",
"DII_DUMMY_ITEM7", "DII_DUMMY_ITEM8", "DII_DUMMY_ITEM9", "DII_DUMMY_ITEM10"};
int instanceNamePos = 0;

 int DynItemInst::modifyItemForSaving(oCItem* item, bool isHeroItem) {
	if (item == nullptr) return NULL;
	ObjectManager* manager = ObjectManager::getObjectManager();
	int id = manager->getDynInstanceId(item);
	if (id == NULL) return NULL;
	
	zCParser* parser = zCParser::GetParser();
	int saveId; 
	
	if (item->HasFlag(OCITEM_FLAG_EQUIPPED))
	{
		if (instanceNamePos == 9) instanceNamePos = 0;
		saveId = parser->GetIndex(instanceNames[instanceNamePos]);
		++instanceNamePos;
	} else
	{
		saveId = parser->GetIndex("DII_DUMMY_ITEM");
	}
	manager->setInstanceId(item, saveId);
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
	 if (addit == nullptr) {
		 logStream << "DynItemInst::makeEquippedCopy: Warning: Addit is null!!!" << std::endl;
		 logStream << item->name.ToChar() << " : " << additId << std::endl;
		 Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
		 return nullptr;
	 }

	 int instanceId = addit->instanceId;
	 int instanz = addit->instanz;
	 item->instanz = instanz;

	 zCWorld* world = oCGame::GetGame()->GetWorld();

	 oCNpc* owner = inventory->GetOwner();
	 int amount = item->instanz;
	  owner->EquipItem(item);
	 inventory->Remove(item, item->instanz);
	 zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, instanceId);
	 oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceId);
	 if (list != nullptr)
	 {
		 item = list->GetData();
		 copy->instanz = item->instanz + amount;
		 inventory->Remove(item, item->instanz);
	 } else
	 {
		 copy->instanz = amount;
	 }
	 //copy->SetFlag(OCITEM_FLAG_EQUIPPED);
	 //owner->EquipItem(copy);

	 //copy = getInvItemByInstanceId(inventory, instanceId)->GetData();
	 return copy;
 }

void DynItemInst::restoreDynamicInstances(oCGame* game) {
	DynItemInst::denyMultiSlot = true;
	zCWorld* world = game->GetWorld();
	zCListSort<oCNpc>* npcList = world->GetNpcList();
	ObjectManager* manager = ObjectManager::getObjectManager();
	std::list<oCItem*> tempList;
	std::list<oCItem*> equippedItems;
	std::list<oCItem*>::iterator it;

	while(npcList != nullptr) {
		oCNpc* npc = npcList->GetData();
		if (npc == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}
		
		oCNpcInventory* inventory = npc->GetInventory();
		if (inventory == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}

		inventory->UnpackAllItems();
		zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
		while(list != nullptr) {
			oCItem* item = list->GetData();
			if (item != nullptr) tempList.push_back(item);

			list = list->GetNext();
		}
		npcList = npcList->GetNext();

		for (it = tempList.begin(); it != tempList.end(); ++it)
		{
			oCItem* item = *it;
			if (item == nullptr) continue;
			int equiped = 0;
			if (item->instanz < 0) {
				int additId = -item->instanz;
				AdditMemory* addit = manager->getAddit(additId);
				equiped = addit->flags & OCITEM_FLAG_EQUIPPED;
			}
			if (equiped)
			{
				item->SetFlag(OCITEM_FLAG_EQUIPPED);
				equippedItems.push_back(*it);
				logStream << "id2: " << (*it)->GetInstance() << std::endl;
				Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);
			} else
			{
				restoreItem(*it, inventory);
			}
		}

		/*typedef int(__thiscall* OCNpcEV_ForceRemoveWeapon)(oCNpc* pThis, void*);
		OCNpcEV_ForceRemoveWeapon oCNpcEV_ForceRemoveWeapon = (OCNpcEV_ForceRemoveWeapon)0x0074EC40;
		
		for (it = equippedItems.begin(); it != equippedItems.end(); ++it)
		{
			*it = makeEquippedCopy(*it, inventory);
			logStream << "id1: " << (*it)->GetInstance() << std::endl;
			Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);
		}

		DynItemInst::denyMultiSlot = false;


		for (it = equippedItems.begin(); it != equippedItems.end(); ++it)
		{
			int instanceId = (*it)->GetInstance();
			logStream << "id: " << instanceId<< std::endl;
			Logger::getLogger()->log(Logger::Warning, &logStream, true, true, true);
			//world->AddVob(*it);
			inventory->Insert(*it);
			*it = getInvItemByInstanceId(inventory, instanceId)->GetData();
		}

		if (!equippedItems.empty())
		{
			oCNpcEV_ForceRemoveWeapon(npc, nullptr);
		}
		DynItemInst::denyMultiSlot = true;

		for (it = equippedItems.begin(); it != equippedItems.end(); ++it)
		{
			//restoreItem(*it, inventory);
			npc->EquipItem(*it);
		}*/

		for (it = equippedItems.begin(); it != equippedItems.end(); ++it)
		{
			restoreItem(*it, inventory);
			//npc->EquipItem(*it);
		}

		tempList.clear();
		equippedItems.clear();
	}

	tempList.clear();
	zCListSort<oCItem>* itemList = world->GetItemList();
	while(itemList != nullptr) {
		oCItem* item = itemList->GetData();
		if (item != nullptr)
		{
			tempList.push_back(item);	
		}
		itemList = itemList->GetNext();
	}

	it = tempList.begin();
	for (; it != tempList.end(); ++it)
	{
		restoreItem(*it);
	}
	tempList.clear();

	DynItemInst::denyMultiSlot = false;
}

bool DynItemInst::isSaveGameLoading()
{
	return saveGameIsLoading;
};


 int DynItemInst::createInstanceHook(void* pThis, int instanceId, void* source)
{
	zCPar_Symbol* symbol = zCParser::GetParser()->GetSymbol(instanceId);
	if (symbol == nullptr)
	{
		logStream << "DynItemInst::createInstanceHook: symbol is null! InstanceId: " << instanceId << std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
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
	logStream << "DynItemInst::oCGameLoadGameHook: release new instances..."<< std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, true, true, true);
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	oCGameLoadGame(pThis, second, worldName);
}

void __thiscall DynItemInst::oCGameChangeLevelHook(void* pThis, zSTRING const & first, zSTRING const & second) {
	DynItemInst::levelChange = true;
	ObjectManager* manager = ObjectManager::getObjectManager();
	oCNpcInventory* inv = oCNpc::GetHero()->GetInventory();
	inv->UnpackAllItems();
	std::list<oCItem*> tempList;
	std::list<int> idList;
	std::list<oCItem*>::iterator it;
	std::list<int>::iterator idIt;
	zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inv->inventory_data);
	while(list != nullptr)
	{
		oCItem* item = list->GetData();
		if (item != nullptr)
		{
			if (item->HasFlag(OCITEM_FLAG_EQUIPPED))
			{
				tempList.push_back(item);
				idList.push_back(manager->getInstanceId(*item));
			}
		}
		list = list->GetNext();
	}

	for (it = tempList.begin(); it != tempList.end(); ++it)
	{
		oCNpc::GetHero()->Equip(*it);
	}
	tempList.clear();

	oCGameChangeLevel(pThis, first, second);
	inv = oCNpc::GetHero()->GetInventory();
	inv->UnpackAllItems();
	
	for (idIt = idList.begin(); idIt != idList.end(); ++idIt)
	{
		oCItem* item = getInvItemByInstanceId(inv, *idIt)->GetData();
		oCNpc::GetHero()->Equip(item);
	}
	idList.clear();
	DynItemInst::levelChange = false;
	initAdditMemory();
}

int DynItemInst::oCItemMulitSlotHook(void* pThis)
{
	if (DynItemInst::denyMultiSlot)
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

	while(list != nullptr) {
		oCItem* item = list->GetData();
		if (item == nullptr) {
			list = list->GetNext();
			continue;
		}

		int instanceId = manager->getInstanceId(*item);
		bool change = false;
		if (instanceId == -1)
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

zCVisual* DynItemInst::zCVisualLoadVisual(zSTRING const& name)
{
	XCALL(ZCVISUAL_LOAD_VISUAL);
}

zCListSort<oCItem>* DynItemInst::getInvItemByInstanceId(oCNpcInventory* inventory, int instanceId)
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
	while(list != nullptr) {
		oCItem* item = list->GetData();
		if (item != nullptr && manager->getInstanceId(*item) == instanceId)
		{
			return list;
		}
		list = list->GetNext();
	}
	return nullptr;
};

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
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	std::string instances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;
	std::string saveGameDir = manager->getCurrentDirectoryPath();//manager->getSaveGameDirectoryPath(saveGameSlotNumber);
	std::string fileName = saveGameDir + std::string("\\") + instances;
	manager->loadNewInstances((char*)fileName.c_str());
}

void DynItemInst::initAdditMemory()
{
	ObjectManager* manager = ObjectManager::getObjectManager();
	std::string saveGameDir = manager->getCurrentDirectoryPath();

	std::string worldName = oCGame::GetGame()->GetGameWorld()->GetWorldName().ToChar();
	std::string saveFile = SAVE_ITEM_ADDIT + worldName + SAVE_ITEM_FILE_EXT; 
	std::string additSaveGameFilePath = saveGameDir + std::string("\\") + saveFile;

	std::string heroData; 
	heroData= saveGameDir + std::string("\\") 
			+ SAVE_ITEM_HERO_DATA + SAVE_ITEM_FILE_EXT;
	manager->loadHeroData(const_cast<char*>(heroData.c_str()));
	manager->loadWorldObjects(const_cast<char*>(additSaveGameFilePath.c_str()));

	restoreDynamicInstances(oCGame::GetGame());
	manager->removeAllAdditionalMemory();
	logStream << "loadSavegameHook finished." << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
};