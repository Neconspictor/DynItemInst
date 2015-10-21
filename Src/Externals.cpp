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

#include "Externals.h"
#include <api/g2/zcparser.h>
#include <HookManager.h>
#include "oCItemExtended.h"
#include <api/g2/ocgame.h>
#include <ObjectManager.h>
#include <api/g2/oCObjectFactory.h>
#include <Logger.h>

typedef void (__thiscall* OCGameDefineExternals_Ulfi)(void* pThis, zCParser* parser);
OCGameDefineExternals_Ulfi oCGameDefineExternals_Ulfi;

Externals::Externals() : Module()
{
	moduleDesc = "Externals";
}

Externals::~Externals()
{
}

void Externals::hookModule()
{
	oCGameDefineExternals_Ulfi = (OCGameDefineExternals_Ulfi)(OCGAME_DEFINE_EXTERNALS_ULFI);
	HookManager* manager = HookManager::getHookManager();
	manager->addFunctionHook((LPVOID*)&oCGameDefineExternals_Ulfi, oCGameDefineExternals_UlfiHook, moduleDesc);
}

void Externals::unHookModule()
{
	HookManager* manager = HookManager::getHookManager();
	manager->removeFunctionHook((LPVOID*)&oCGameDefineExternals_Ulfi, oCGameDefineExternals_UlfiHook, moduleDesc);
}

void Externals::oCGameDefineExternals_UlfiHook(void* pThis, zCParser* parser)
{
	logStream << "define externals..." << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	parser->DefineExternal(zSTRING("DII_CreateNewItem"), DII_CreateNewItem, zCPINSTANCE, zCPINT, zCPVOID);
	parser->DefineExternal(zSTRING("DII_CreateNewInstance"), DII_CreateNewInstance, zCPINT, zCPINSTANCE, zCPVOID);
	parser->DefineExternal(zSTRING("DII_IsDynamic"), DII_IsDynamic, zCPINT, zCPINSTANCE, zCPVOID);
	parser->DefineExternal(zSTRING("DII_DeleteItem"), DII_DeleteItem, zCPVOID, zCPINSTANCE, zCPVOID);
	parser->DefineExternal(zSTRING("DII_GetUserData"), DII_GetUserData, zCPINSTANCE, zCPINT, zCPVOID);
	oCGameDefineExternals_Ulfi(pThis, parser);
}


int Externals::DII_CreateNewItem() // Func C_ITEM DII_CreateNewItem(VAR INT instanceId)
{
	zCParser* parser = zCParser::GetParser();
	int instanceId; 
	parser->GetParameter(instanceId);
	
	int globalItemIndex = parser->GetIndex("ITEM");
	zCPar_Symbol* globItemSymbol = parser->GetSymbol(globalItemIndex);

	// Check if provided instance id is valid
	zCPar_Symbol* symbol = parser->GetSymbol(instanceId);
	if (symbol == NULL)
	{
		oCItem* item = oCObjectFactory::GetFactory()->CreateItem(NULL);
		globItemSymbol->offset = (int)item;
		parser->SetReturn(item);
		return 0;
	}
	
	// Create new item and store it in gloval variable 'ITEM'
	oCItem* item = oCObjectFactory::GetFactory()->CreateItem(instanceId);
	
	globItemSymbol->offset = (int) item;
	oCGame::GetGame()->GetWorld()->AddVob(item);
	parser->SetReturn(item);
	
	return 0;
}


int Externals::DII_CreateNewInstance() //Func int DII_CreateNewInstance(var C_Item item)
{
	zCParser* parser = zCParser::GetParser();
	oCItem* item = (oCItem*) parser->GetInstance();
	std::stringstream	logStream;

	if (item == NULL) {
		parser->SetReturn(NULL);
		return -1;
	}

	// Create new instance with item
	ObjectManager* manager = ObjectManager::getObjectManager();
	int key = manager->createNewInstanceId(item);
	parser->SetReturn(key);
	return 0;
}

int Externals::DII_IsDynamic() // Func int DII_IsDynamic(VAR C_ITEM item)
{
	zCParser* parser = zCParser::GetParser();
	oCItem* item = (oCItem*) parser->GetInstance();

	if (item == NULL) {
		parser->SetReturn(NULL);
		return -1;
	}

	bool modified = ObjectManager::getObjectManager()->IsModified(item);
	if (modified)
	{
		parser->SetReturn(TRUE);
	} else
	{
		parser->SetReturn(FALSE);
	}

	return 0;
}


int Externals::DII_DeleteItem() //Func void DII_DeleteItem(var C_Item item)
{
	zCParser* parser = zCParser::GetParser();
	int index;
	oCItem* item = (oCItem*) parser->GetInstanceAndIndex(index);
	zCPar_Symbol* symbol = parser->GetSymbol(index);

	if (item == NULL) {
		logStream << "DeleteItem: Item is NULL!" << std::endl; 
		Logger::getLogger()->log(Logger::Info, &logStream);
		return -1;
	}

	oCGame::GetGame()->GetWorld()->RemoveVob(item);
	oCItem::operator delete(item);
	symbol->offset = NULL;
	return 0;
}

int Externals::DII_GetUserData() // Func DII_UserData DII_GetUserData(var int instanceId)
{
	zCParser* parser = zCParser::GetParser();
	ObjectManager* manager = ObjectManager::getObjectManager();
	int instanceId;
	parser->GetParameter(instanceId);

	if (!manager->IsModified(instanceId))
	{
		parser->SetReturn(NULL);
		return -1;
	}

	DynInstance* storeItem = manager->getInstanceItem(instanceId);
	parser->SetReturn(storeItem->getUserData());
	return 0;
}