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
#include <Levitation.h>
#include <functional>
#include <Constants.h>

using namespace constants;

const std::string DynItemInst::SAVE_ITEM_FILE_EXT = ".SAV";
const std::string DynItemInst::SAVE_ITEM_INSTANCES  = "DII_INSTANCES";
const std::string DynItemInst::FILE_PATERN = "DII_*";

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

void DynItemInst::hookModule()
{
	loadSavegame = (LoadSavegame) (LOAD_SAVEGAME_ADDRESS);
	writeSavegame = (WriteSavegame) (WRITE_SAVEGAME_ADDRESS);
	oCItemGetValue = (OCItemGetValue) (OCITEM_GET_VALUE_ADDRESS);
	createInstance = (CreateInstance) (ZCPARSER_CREATE_INSTANCE);
	oCGameLoadGame = (OCGameLoadGame) OCGAME_LOAD_GAME_ADDRESS;

	oCGameChangeLevel = reinterpret_cast<OCGameChangeLevel>(OCGAME_CHANGE_LEVEL_ADDRESS);
	oCItemMulitSlot = (OCItemMulitSlot) OCITEM_MULTI_SLOT;

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
	
	hookManager->addFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHook, moduleDesc);
	
	
	hookManager->addFunctionHook((LPVOID*)&zCParserGetIndex, zCParserGetIndexHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetIndex, zCPar_SymbolTableGetIndexHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbol, zCPar_SymbolTableGetSymbolHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPar_SymbolTableGetSymbolString, zCPar_SymbolTableGetSymbolStringHook, moduleDesc);
}

void DynItemInst::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&loadSavegame, loadSavegameHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&writeSavegame, writeSavegameHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHook, moduleDesc);

	hookManager->removeFunctionHook((LPVOID*)&createInstance, createInstanceHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHook, moduleDesc);
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
	loadDynamicInstances(saveGameSlotNumber);
	loadSavegame(pThis, saveGameSlotNumber, b);

	logStream << "DynItemInst::loadSavegameHook: done." << std::endl;
	util::logInfo(&logStream);
};


 void DynItemInst::writeSavegameHook(void* pThis,int saveGameSlotNumber, int b)
{   
	logStream << "DynItemInst::writeSavegameHook: save game..." << std::endl;
	util::logInfo(&logStream);

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

	util::copyContentTo(currentDir,saveGameDir, FILE_PATERN);

	// Write actual savegame
	writeSavegame(pThis, saveGameSlotNumber, b);

	// We save now the dynamic instances
	// It is better to do this after savegame writing, as gothic copies (possibly outdated) dynamic-instance-save-files from the current
	// save directory to the target savegame directory. Would be save the dynamic instances before the savegame-writing, gothic would eventually 
 	// overwrite our stuff!!! 
	std::string saveInstances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;
	manager->saveNewInstances(const_cast<char*>(saveGameDir.c_str()), const_cast<char*>(saveInstances.c_str()));
	util::copyFileTo(saveGameDir + saveInstances, currentDir + saveInstances);

	logStream << "DynItemInst::writeSavegameHook: done." << std::endl;
	util::logInfo(&logStream);
};


 int DynItemInst::createInstanceHook(void* pThis, int instanceId, void* source)
{
	zCPar_Symbol* symbol = zCParser::GetParser()->GetSymbol(instanceId);
	if (symbol == NULL)
	{
		logStream << "DynItemInst::createInstanceHook: symbol is null! InstanceId: " << instanceId << std::endl;
		util::debug(&logStream, Logger::Warning);
	}

	// Get the smybol of the item variable and let it point to the 'source' variable (will contain the newly created item!)
	//
 	zCParser* parser = (zCParser*)pThis;
	zCPar_Symbol* itemSymbol = parser->GetSymbol("DII_ITEM");
	 if (itemSymbol != nullptr)
		itemSymbol->offset = (int)source;

	int result = createInstance(pThis, instanceId, source);

	auto isDynamic = ObjectManager::getObjectManager()->isDynamicInstance(instanceId);
	if (isDynamic)
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

int DynItemInst::oCItemMulitSlotHook(void* pThis)
{
	return oCItemMulitSlot(pThis);
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

void DynItemInst::loadDynamicInstances(int saveGameSlotNumber)
{
	logStream << "DynItemInst::loadDynamicInstances: load dii instances..." << std::endl;
	util::logInfo(&logStream);
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	std::string instances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;
	std::string saveGameDir = manager->getSaveGameDirectoryPath(saveGameSlotNumber);
	std::string fileName = saveGameDir + instances;
	manager->loadNewInstances((char*)fileName.c_str());
	logStream << "DynItemInst::loadDynamicInstances: done." << std::endl;
	util::logInfo(&logStream);
}