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

#include "zCOptionExtended.h"
#include "ObjectManager.h"
#include <fstream> 
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "api/g2/zcoption.h"
#include "api/g2/zcworld.h"
#include "HookManager.h"
#include "oCMobContainer.h"
#include "Util.h"
#include <DynItemInst.h>
#include "api/g2/zcparser.h"
#include <api/g2/ocgame.h>
#include <Logger.h>
using namespace std;

ObjectManager* ObjectManager::instanz = 0;

ObjectManager::ObjectManager()
{
	this->instanceMap = std::map<int, DynInstance*>();
	this->nextInstances = std::queue<int>();
};

zCPar_SymbolTable* zCParserGetSymbolTable(void* parser)
{
	BYTE* pointer = ((BYTE*)parser) + 0x10;
	return (zCPar_SymbolTable*)pointer;
}


ObjectManager* ObjectManager::getObjectManager() {
	if (instanz == NULL)
	{
		instanz = new ObjectManager();
	}
	return instanz;
};

void ObjectManager::release() {
	if (instanz != NULL) {
		instanz->releaseInstances();
		delete instanz;
		instanz = NULL;
	}
};

int ObjectManager::createNewInstanceId(oCItem* item) {
	if (item == NULL) return 0;

	//Create new zCPar_Symbol for the newly created instance and register it to gothic.
	zCParser* parser = zCParser::GetParser();
	zCPar_Symbol* symbol = new zCPar_Symbol();
	ZeroMemory(symbol, sizeof(zCPar_Symbol));

	int parentId = getInstanceId(*item);
	zCPar_Symbol* old = parser->GetSymbol(parentId);

	if (old == NULL) return 0;

	int instanceId = INSTANCE_BEGIN - 1;
	if (nextInstances.empty()) {
		instanceId += (static_cast<int>(instanceMap.size()) + 1);
	} else {
		instanceId = static_cast<int>(nextInstances.front());
		nextInstances.pop();
	}

	DynInstance* instanceItem = new DynInstance();
	if (IsModified(item))
	{
		DynInstance* oldStoreItem = getInstanceItem(parentId);
		instanceItem->copyUserData(*oldStoreItem);
	}
	instanceItem->store(*item);
	instanceMap.insert(pair<int, DynInstance*>(instanceId, instanceItem));
	
	int testIndex = parser->GetIndex("ExternalFunction");
	if (testIndex == 0)
	{
		logStream << "testIndex is Null!" << std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	}

	symbol->parent = old->parent;
	symbol->bitfield = old->bitfield;
	// Generate unique name
	std::stringstream ss;
	ss << old->name.ToChar() << "_" << instanceId;
	symbol->name = zSTRING(ss.str().c_str());
	symbol->offset = 0;//0x20C;
	symbol->next = 0;
	symbol->content.data_ptr = 0;//parser->GetSymbol(parser->GetIndex("DII_TestConstructor"));//old->content.data_ptr;
	symbol->filenr = ZCPAR_SYMBOL_MARK_ID;

	setParentInstanceId(instanceId, parentId);

	instanceItem->setParserSymbolBitfield(symbol->bitfield);
	instanceItem->setZCPar_SymbolName(symbol->name.ToChar());
	instanceItem->setInstanceID(instanceId);

	newInstanceToSymbolMap.insert(std::pair<int, zCPar_Symbol*>(instanceId, symbol));

	std::string symbolName = symbol->name.ToChar();
	std::transform(symbolName.begin(), symbolName.end(),symbolName.begin(), ::toupper);
	nameToSymbolMap.insert(std::pair<std::string, zCPar_Symbol*>(symbolName, symbol));
	nameToIndexMap.insert(std::pair<std::string, int>(symbolName, instanceId));

	return instanceId;
};

void ObjectManager::createInstanceById(int id, DynInstance* item) {
	// allow no reassignments
	map<int, DynInstance*>::iterator it;
	it = instanceMap.find(id);
	if (it != instanceMap.end())
	{
		logStream << "Warning: instance id already exists! Nothing will be created." << std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
		return;
	}

	instanceMap.insert(pair<int, DynInstance*>(id, item));
}

void ObjectManager::releaseInstances() {

	// Release all allocated memory referenced by the instance map
	map<int, DynInstance*>::iterator it;
	for (it = instanceMap.begin(); it != instanceMap.end(); ++it) {
		DynInstance* item = (*it).second;
		delete item;
		(*it).second = 0;
	}

	// clear all data structures
	instanceMap.clear();
	nextInstances = queue<int>();
	indexZCParSymbolNameMap.clear();
	newInstanceToSymbolMap.clear();
	nameToIndexMap.clear();
	nameToSymbolMap.clear();

};

bool ObjectManager::assignInstanceId(oCItem* item, int id){
	map<int,DynInstance*>::iterator it;
	it = instanceMap.find(id);
	// Is id a valid instance id?
	//if (it == instanceMap.end()) return false;
	if (it == instanceMap.end())
	{
		logStream<< "Warning: instance id wasn't found: " << id << std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
		return false;
	}
	
	setDynInstanceId(item, id);

	if(!initByNewInstanceId(item))
	{
		logStream << "Warning: Item Initialisation failed!" << std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
		return false;
	};
	return true;
};


bool ObjectManager::initByNewInstanceId(oCItem* item) {
	int instanceId = getDynInstanceId(item);
	if (instanceId == NULL) return false;
	DynInstance* instanceItem = getInstanceItem(instanceId);
	instanceItem->init(item, getDynInstanceId(item));
	return true;
};

/*
* @Return: The new instance created for the given item. If no instance was created previously,
NULL will be returned.
*/
int ObjectManager::getDynInstanceId(oCItem* item){
	if (item == NULL) return NULL;
	int* pointer = reinterpret_cast<int*>((BYTE*)item + 0x330);
	int instanceId = *pointer;
	map<int, DynInstance*>::iterator it;
	it = instanceMap.find(instanceId);
	if (it == instanceMap.end()) { return NULL;}
	return instanceId;
}

void ObjectManager::setDynInstanceId(oCItem* item, int id){
	std::map<int, DynInstance*>::iterator it;
	it = instanceMap.find(id);
	if (it != instanceMap.end())
	{
		int* pointer = reinterpret_cast<int*>((BYTE*)item + 0x330);
		*pointer = id;
	} else
	{
		logStream << "ObjectManager::setInstanceId: Warning: parameter id has no assigned index. Nothing will be done." << std::endl;
		Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
	}
};

DynInstance* ObjectManager::getInstanceItem(int instanceId){
	map<int,DynInstance*>::iterator it;
	it = instanceMap.find(instanceId);
	if (it == instanceMap.end())
	{
		return NULL;		
	}
	return (*it).second;
}; 


void ObjectManager::saveNewInstances(char* directoryPath, char* filename) {
	std::string dir (directoryPath);
	if (!util::existsDir(dir)) {
		if(!util::makePath(dir)) {
			return;
		}
	}

	std::string fullpath = dir + std::string("\\") + std::string(filename);
	ofstream fs(const_cast<char*>(fullpath.c_str()));
	 boost::archive::text_oarchive oa(fs);

    	// archive and stream closed when destructors are called
	map<int, DynInstance*>::iterator it = instanceMap.begin();
	size_t size = instanceMap.size();
	// Save instance items
	oa << size;
	for ( ; it != instanceMap.end(); ++it) {
		oa << it->first;
		DynInstance* storeItem = it->second;
		oa << storeItem;
	}

};


void ObjectManager::loadNewInstances(char* filename) {
	ifstream ifs(filename);
	if (ifs == NULL) {
		return;
	}

	indexZCParSymbolNameMap.clear();
	
	// create and open an archive for input
    boost::archive::text_iarchive ia(ifs);
	// archive and stream closed when destructors are called
	size_t size;
	ia >> size;
	for (size_t i = 0; i != size; ++i) {
		int id;
		ia >> id;
		DynInstance* storeItem;
		ia >> storeItem;
		DynInstance* item = new DynInstance(*storeItem);
		createInstanceById(id, item);

		ParserInfo info;
		info.name = storeItem->getZCPar_SymbolName();
		info.bitfield = storeItem->getParserSymbolBitfield();
		info.newInstanceId = id;
		info.container = item;
		indexZCParSymbolNameMap.push_back(info);
	}

	createParserSymbols();
};


void ObjectManager::oCItemInitByScript(oCItem* item, int inst, int second)
{
	XCALL(0x00711BD0);
}

std::list<oCMobContainer*>* ObjectManager::getMobContainers() {
	std::list<oCMobContainer*>* containerList = new std::list<oCMobContainer*>();
	zCWorld* world = oCGame::GetGame()->GetWorld();
	zCListSort<zCVob>* vobList = world->GetVobList();
	oCMobContainer* dummy = new oCMobContainer();
	while(vobList != NULL) {
		zCVob* vob = vobList->GetData();
		vobList = vobList->GetNext();
		if (vob == NULL){
			continue;
		}

		if (strcmp(typeid(*vob).name(), typeid(*dummy).name()) == 0) {
			containerList->push_back(dynamic_cast<oCMobContainer*>(vob));
		}
	}
	delete dummy;
	return containerList;
};


zCPar_Symbol* createNewSymbol(ObjectManager::ParserInfo* old)
{
	zCParser* parser = zCParser::GetParser();
	zCPar_Symbol* parent = parser->GetSymbol("C_ITEM");

	zCPar_Symbol* result = new zCPar_Symbol();
	result->parent = parent;
	result->bitfield = old->bitfield;

	result->name = zSTRING(old->name.c_str());
	result->offset = 0;//0x20C;
	result->next = 0;
	result->content.data_ptr = 0;//parser->GetSymbol(parser->GetIndex("DII_TestConstructor"));
	result->filenr = ObjectManager::ZCPAR_SYMBOL_MARK_ID;
	result->line = 0;
	result->line_anz = 0;
	result->pos_beg = 0;
	result->pos_anz = 0;
	
	return result;
};

void updateContainerItem(ObjectManager::ParserInfo* info)
{
	DynInstance* item = info->container;
	item->setInstanceID(info->newInstanceId);
	item->setZCPar_SymbolName(info->name);
	item->setParserSymbolBitfield(info->bitfield);
};

void ObjectManager::logSymbolData(zCPar_Symbol* sym)
{
	logStream << sym->name.ToChar() << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << sym->parent->name.ToChar() << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream<< sym->bitfield << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << sym->filenr << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << sym->line << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << sym->line_anz << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << sym->next << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << sym->offset << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << sym->pos_beg << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << sym->pos_anz << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
};

void ObjectManager::createParserSymbols()
{
	std::vector<ParserInfo>::iterator it;
	for (it = indexZCParSymbolNameMap.begin(); it != indexZCParSymbolNameMap.end(); ++it)
	{
		ParserInfo info = *it;
		zCPar_Symbol* symbol = createNewSymbol(&info);
		int index = zCParser::GetParser()->GetIndex(symbol->name);
		index = info.newInstanceId;
		int newInstanceId = info.newInstanceId;
		updateContainerItem(&info);
		newInstanceToSymbolMap.insert(std::pair<int, zCPar_Symbol*>(newInstanceId, symbol));
		std::string symbolName = symbol->name.ToChar();
		std::transform(symbolName.begin(), symbolName.end(),symbolName.begin(), ::toupper);
		nameToSymbolMap.insert(std::pair<std::string, zCPar_Symbol*>(symbolName, symbol));
		nameToIndexMap.insert(std::pair<std::string, int>(symbolName, index));
		//zCPar_Symbol* result = zCParser::GetParser()->GetSymbol(index);
		//logSymbolData(result);
	}
};



void ObjectManager::setInstanceId(oCItem* item, int value) {
	int address = reinterpret_cast<int>(item);
	address += 0x330;
	int* instance = reinterpret_cast<int*>(address);
	*instance = value;

}

/*
* Provides the instance id, the given item currently as.
*/
int ObjectManager::getInstanceId(oCItem& item) {
	int address = reinterpret_cast<int>(&item);
	address += 0x330;
	int* instance = reinterpret_cast<int*>(address);
	return *instance;

}


void ObjectManager::setParentInstanceId(int instanceId, int parentId) {
	std::map<int,DynInstance*>::iterator it;
	it = instanceMap.find(instanceId);
	if (it == instanceMap.end()) return;
	DynInstance* instanceItem = it->second;
	instanceItem->setParentInstanceId(parentId);
}

/*
*	Provides the instance id the unmodified item had at the time at which 
*	the new instance id instanceId was created for a specific item.
*/
int ObjectManager::getParentInstanceId(int instanceId) {
	std::map<int,DynInstance*>::iterator it;
	it = instanceMap.find(instanceId);
	if (it == instanceMap.end()) return NULL;
	DynInstance* instanceItem = it->second;
	return instanceItem->getParentInstanceId();
}


std::string ObjectManager::getSaveGameDirectoryPath(int saveGameSlotNumber) {
	zCOption* option = zCOption::GetOption();
	zSTRING& gothicDirPath = option->GetDirString(static_cast<zTOptionPaths>(24));
	zSTRING& saveGamesDirName = option->GetDirString(static_cast<zTOptionPaths>(2));

	std::stringstream ss;
	//write full path to the savegame
	ss << gothicDirPath.ToChar() << saveGamesDirName.ToChar() << "savegame" << saveGameSlotNumber;
	return ss.str();
};

std::string ObjectManager::getCurrentDirectoryPath() {
	zCOption* option = zCOption::GetOption();
	zSTRING& gothicDirPath = option->GetDirString(static_cast<zTOptionPaths>(24));
	zSTRING& saveGamesDirName = option->GetDirString(static_cast<zTOptionPaths>(2));

	std::stringstream ss;
	//write full path to the savegame
	ss << gothicDirPath.ToChar() << saveGamesDirName.ToChar() << "current";
	return ss.str();
};

bool ObjectManager::InitItemWithDynInstance(oCItem* item, int index)
{
	// only init item if an instance id exists
	if (index == NULL)
	{
		logStream << "Warning: instanceId wasn't found!" << std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
		return false;
	}
	return assignInstanceId(item, index);
}

bool ObjectManager::IsModified(oCItem* item)
{
	int index = getDynInstanceId(item);
	std::map<int,DynInstance*>::iterator it;
	it = instanceMap.find(index);
	if (it == instanceMap.end())
	{
		return false;
	} 
	return true;
}

bool ObjectManager::IsModified(int instanceId)
{
	std::map<int,DynInstance*>::iterator it;
	it = instanceMap.find(instanceId);
	if (it == instanceMap.end())
	{
		return false;
	} 
	return true;
}

zCPar_Symbol* ObjectManager::getSymbolByIndex(int index)
{
	std::map<int, zCPar_Symbol*>::iterator it;
	it = newInstanceToSymbolMap.find(index);
	if (it == newInstanceToSymbolMap.end()) { return NULL; }
	return it->second;
}

zCPar_Symbol* ObjectManager::getSymbolByName(zSTRING symbolName)
{
	std::string name = symbolName.ToChar();
	std::transform(name.begin(), name.end(),name.begin(), ::toupper);
	std::map<std::string, zCPar_Symbol*>::iterator it;
	it = nameToSymbolMap.find(name);
	if (it == nameToSymbolMap.end()) { return NULL; }
	return it->second;
}


int ObjectManager::getIndexByName(zSTRING symbolName)
{
	std::string name = symbolName.ToChar();
	std::transform(name.begin(), name.end(),name.begin(), ::toupper);
	std::map<std::string, int>::iterator it;
	it = nameToIndexMap.find(name);
	if (it == nameToIndexMap.end()) { return NULL; }
	return it->second;
}