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


#include "api/g2/ocitem.h"
#include <api/g2/ocgame.h>
#include <ObjectManager.h>
#include <api/g2/oCObjectFactory.h>
#include <Logger.h>
#include <DaedalusExports.h>


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


oCItem* __cdecl DaedalusExports::DII_CreateNewItem(int instanceId) // Func VOID CreateNewItem(VAR INT instanceId)
{
	oCItem* result = oCObjectFactory::GetFactory()->CreateItem(instanceId);
	return result;
}


int DaedalusExports::DII_CreateNewInstance(oCItem* item) //Func int CreateNewItem(var C_Item item)
{
	if (item == NULL) {return NULL;}

	logStream << "Param: " << item->name.ToChar();
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);

	// Create new instance with item
	ObjectManager* manager = ObjectManager::getObjectManager();
	int key = manager->createNewInstanceId(item);
	int index = manager->getDynInstanceId(item);
	logStream << "Index: " << index << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);

	logStream << "CreateNewInstance::key: " << key << std::endl;
	logStream << "I was here!" << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream);
	return key;
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


void oCItemOperatorDelete(oCItem* item)
{
	XCALL(0x007144A0);
}

void DaedalusExports::DII_DeleteItem(oCItem* item)
{

	if (item == NULL) {return;}
	oCGame::GetGame()->GetWorld()->RemoveVob(item);
	//oCItem::operator delete(item);
	oCItemOperatorDelete(item);
}

DII_UserData::Data* DaedalusExports::DII_GetUserData(int instanceId) // Func DII_UserData DII_GetUserData(var int instanceId)
{
	ObjectManager* manager = ObjectManager::getObjectManager();

	if (!manager->IsModified(instanceId))
	{
		return NULL;
	}

	DynInstance* storeItem = manager->getInstanceItem(instanceId);
	return storeItem->getUserData();
}