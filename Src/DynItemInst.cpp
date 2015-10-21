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

#include <oCItemExtended.h>
#include <DynItemInst.h>
#include <ObjectManager.h>
#include <iostream>
#include <HookManager.h>
#include <Windows.h>
#include <api/g2/zcparser.h>
#include <Logger.h>
#include <api/g2/ocnpc.h>
#include <api/g2/zcworld.h>
#include <api/g2/ocgame.h>

const std::string DynItemInst::SAVE_ITEM_FILE_EXT = ".SAV";
const std::string DynItemInst::SAVE_ITEM_INSTANCES  = "DII_INSTANCES";

std::vector<zCPar_Symbol*>* DynItemInst::symbols = new std::vector<zCPar_Symbol*>();
bool DynItemInst::showExtendedDebugInfo = false;
int DynItemInst::saveGameSlotNumber = 0;

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
typedef void (__thiscall* OCNpcUnEquipItem)(void* pThis, oCItem* item);
OCNpcUnEquipItem oCNpcUnEquipItem;
typedef void (__thiscall* OCGameChangeLevel)(void* pThis, zSTRING const &, zSTRING const &);
OCGameChangeLevel oCGameChangeLevel;

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

	oCNpcUnEquipItem = (OCNpcUnEquipItem)OCNPC_UNEQUIP_ITEM;
	oCGameChangeLevel = (OCGameChangeLevel) OCGAME_CHANGE_LEVEL;


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

	if (result != NULL)
	{
		zSTRING copy = symbolName;
		std::cout << copy.ToChar() << std::endl;
	}

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
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	std::string instances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;

	std::string saveGameDir;
	if (saveGameSlotNumber != -1)
	{
	 saveGameDir = manager->getSaveGameDirectoryPath(saveGameSlotNumber);
	} else
	{
		saveGameDir = manager->getCurrentDirectoryPath();	
	}

	std::string fileName = saveGameDir + std::string("\\") + instances;
	manager->loadNewInstances(const_cast<char*>(fileName.c_str()));

	loadSavegame(pThis, saveGameSlotNumber, b);
};


 void DynItemInst::writeSavegameHook(void* pThis,int saveGameSlotNumber, int b)
{   
	ObjectManager* manager = ObjectManager::getObjectManager();
	std::string saveGameDir; 
	if (saveGameSlotNumber != -1)
	{
	 saveGameDir = manager->getSaveGameDirectoryPath(saveGameSlotNumber);
	} else
	{
		saveGameDir = manager->getCurrentDirectoryPath();	
	}
	std::string saveInstances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;
	writeSavegame(pThis, saveGameSlotNumber, b);
	manager->saveNewInstances(const_cast<char*>(saveGameDir.c_str()), const_cast<char*>(saveInstances.c_str()));

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
		oCGame::GetGame()->GetWorld()->AddVob(item);
		return manager->getDynInstanceId(item);
	}

	return createInstance(pThis, instanceId, source);
}

void DynItemInst::oCGameLoadGameHook(void* pThis, int second, zSTRING const& worldName)
{
	logStream << "DynItemInst::oCGameLoadGameHook: release new instances..."<< std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	ObjectManager::getObjectManager()->releaseInstances();
	oCGameLoadGame(pThis, second, worldName);
}


void DynItemInst::oCGameChangeLevelHook(void* pThis, zSTRING const& first, zSTRING const& second)
{
	logStream << "oCGameChangeLevelHook: ..." << std::endl;
	Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);

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
			if (item->HasFlag(0x40000000))
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