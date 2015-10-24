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
	zCPar_SymbolTableGetSymbol = (ZCPar_SymbolTableGetSymbol) (ZCPAR_SYMBOL_TABLE_GETSYMBOLE);
	zCPar_SymbolTableGetSymbolString = (ZCPar_SymbolTableGetSymbolString) (ZCPAR_SYMBOL_TABLE_GETSYMBOLE_STRING);
	zCPar_SymbolTableGetIndex = (ZCPar_SymbolTableGetIndex)(ZCPAR_SYMBOL_TABLE_GETINDEX);
	zCParserGetIndex = (ZCParserGetIndex)(ZCPARSER_GETINDEX);
	createInstance = (CreateInstance) (ZCPARSER_CREATE_INSTANCE);
	oCGameLoadGame = (OCGameLoadGame) OCGAME_LOAD_GAME_ADDRESS;

	oCGameLoadWorld = reinterpret_cast<OCGameLoadWorld>(OCGAME_LOAD_WORLD_ADDRESS);
	oCGameChangeLevel = reinterpret_cast<OCGameChangeLevel>(OCGAME_CHANGE_LEVEL_ADDRESS);
	oCItemMulitSlot = (OCItemMulitSlot) OCITEM_MULTI_SLOT;
	oCMobContainerOpen = (OCMobContainerOpen) OCMOB_CONTAINER_OPEN;

	HookManager* hookManager = HookManager::getHookManager();
	hookManager->addFunctionHook((LPVOID*)&loadSavegame, loadSavegameHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&writeSavegame, writeSavegameHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbol, zCPar_SymbolTableGetSymbolHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbolString, zCPar_SymbolTableGetSymbolStringHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetIndex, zCPar_SymbolTableGetIndexHook,  moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCParserGetIndex, zCParserGetIndexHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&createInstance, createInstanceHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameChangeLevel, oCGameChangeLevelHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHook, moduleDesc);

	loadDynamicInstances();
	initAdditMemory();
}

void DynItemInst::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&loadSavegame, loadSavegameHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&writeSavegame, writeSavegameHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbol, zCPar_SymbolTableGetSymbolHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbolString, zCPar_SymbolTableGetSymbolStringHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCPar_SymbolTableGetIndex, zCPar_SymbolTableGetIndexHook,moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCParserGetIndex, zCParserGetIndexHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&createInstance, createInstanceHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameChangeLevel, oCGameChangeLevelHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHook, moduleDesc);
};


int DynItemInst::oCItemGetValueHook(void* pThis) {
	oCItem* item = static_cast<oCItem*>(pThis);
	ObjectManager* manager = ObjectManager::getObjectManager();
	if (manager->getDynInstanceId(item) > ObjectManager::INSTANCE_BEGIN) {
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
	loadSavegame(pThis, saveGameSlotNumber, b);
	loadDynamicInstances();
	initAdditMemory();
};


 void DynItemInst::writeSavegameHook(void* pThis,int saveGameSlotNumber, int b)
{   
	oCGame* game = oCGame::GetGame();
	zCWorld* world = game->GetWorld();
	oCWorld* ocworld = game->GetGameWorld();
	
	zCListSort<oCNpc>* npcList = world->GetNpcList();

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
		while(list != NULL) {
			oCItem* item = list->GetData();
			modifyItemForSaving(item, isHero);
			list = list->GetNext();
		}
		npcList = npcList->GetNext();
	}
	
	zCListSort<oCItem>* itemList = world->GetItemList();
	while(itemList != NULL) {
		oCItem* item = itemList->GetData();
		modifyItemForSaving(item, false);
		itemList = itemList->GetNext();
	}

	ObjectManager* manager = ObjectManager::getObjectManager();

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

	//copy all files from the current savegame folder to the slotDirectory folder
//	if (saveGameSlotNumber != -1) {
//		util::copyContentTo(currentSaveGameDir,saveGameDir);
//	}
	util::copyContentTo(currentDir,saveGameDir, FILE_PATERN);
	util::copyContentTo(saveGameDir,currentDir, FILE_PATERN);

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
	//reset

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
	if (item == NULL) return;
	if (item->instanz < 0) {
		ObjectManager* manager = ObjectManager::getObjectManager();
		int additId = -item->instanz;
		AdditMemory* addit = manager->getAddit(additId);
		if (addit == NULL) {
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
		if (inventory == NULL)
		{
		
			manager->setInstanceId(item, instanceId);
			manager->assignInstanceId(item, instanceId);
			item->SetVisual(zCVisualLoadVisual(item->visual));
			return;
		}

		oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceId);
		world->AddVob(copy);
		copy->instanz = instanz;

		// is the item equipped?
		if (item->HasFlag(OCITEM_FLAG_EQUIPPED))
		{
			oCNpc* owner = inventory->GetOwner();
			owner->Equip(item);
			inventory->RemoveByPtr(item, instanz);
			DynItemInst::denyMultiSlot = false;
			world->AddVob(copy);
			inventory->Insert(copy);
			DynItemInst::denyMultiSlot = true;
			copy = getInvItemByInstanceId(inventory, instanceId)->GetData();
			owner->EquipItem(copy);
			return;
		}
		//copy->flags = flags;
		inventory->RemoveByPtr(item, instanz);
		inventory->Insert(copy);
		//world->RemoveVob(copy);
		//manager->setOrgInstance(item, instanceId);
		//manager->assignInstanceId(item, instanceId);
	}
}

 void DynItemInst::modifyItemForSaving(oCItem* item, bool isHeroItem) {
	if (item == NULL) return;
	ObjectManager* manager = ObjectManager::getObjectManager();
	int id = manager->getDynInstanceId(item);
	if (id == NULL) return;
	
	zCParser* parser = zCParser::GetParser();
	int saveId = parser->GetIndex("ITAR_PIR_L_Addon");
	manager->setInstanceId(item, saveId);
	//item->instanz = -id;
	manager->createAdditionalMemory(item, id, isHeroItem);
}

void DynItemInst::restoreDynamicInstances(oCGame* game) {
	DynItemInst::denyMultiSlot = true;
	zCWorld* world = game->GetWorld();
	zCListSort<oCNpc>* npcList = world->GetNpcList();
	std::list<oCItem*> tempList;
	std::list<oCItem*>::iterator it;

	while(npcList != NULL) {
		oCNpc* npc = npcList->GetData();
		if (npc == NULL) {
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
		while(list != NULL) {
			oCItem* item = list->GetData();
			if (item != NULL) tempList.push_back(item);

			list = list->GetNext();
		}
		npcList = npcList->GetNext();
		for (it = tempList.begin(); it != tempList.end(); ++it)
		{
			restoreItem(*it, inventory);
		}
		tempList.clear();
	}

	tempList.clear();
	zCListSort<oCItem>* itemList = world->GetItemList();
	while(itemList != NULL) {
		oCItem* item = itemList->GetData();
		if (item != NULL)
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
};


 int DynItemInst::createInstanceHook(void* pThis, int instanceId, void* source)
{
	zCPar_Symbol* symbol = zCParser::GetParser()->GetSymbol(instanceId);
	if (symbol == NULL)
	{
		logStream << "DynItemInst::createInstanceHook: symbol is null! InstanceId: " << instanceId << std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	}
	bool isTarget = symbol && (symbol->filenr == ObjectManager::ZCPAR_SYMBOL_MARK_ID);
	if (isTarget)
	{
		oCItem* item = (oCItem*)source;
		ObjectManager* manager = ObjectManager::getObjectManager();
		manager->InitItemWithDynInstance(item, instanceId);	
		return manager->getDynInstanceId(item);
	}

	return createInstance(pThis, instanceId, source);
}

void DynItemInst::oCGameLoadGameHook(void* pThis, int second, zSTRING const& worldName)
{
	logStream << "DynItemInst::oCGameLoadGameHook: release new instances..."<< std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
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
	while(list != NULL)
	{
		oCItem* item = list->GetData();
		if (item != NULL)
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
	//restoreAfterSaveGameWriting(oCGame::GetGame());
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

	while(list != NULL) {
		oCItem* item = list->GetData();
		if (item == NULL) {
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