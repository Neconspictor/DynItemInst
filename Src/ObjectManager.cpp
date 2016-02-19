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

#include <zCOptionExtended.h>
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
#include <ocgameExtended.h>
#include <Logger.h>
#include <set>
#include <zCPar_SymbolTable.h>
using namespace std;

ObjectManager* ObjectManager::instanz = nullptr;


ObjectManager::ObjectManager()
{
	this->instanceMap = std::map<int, DynInstance*>();
	this->nextInstances = std::queue<int>();
};

g2ext_extended::zCPar_SymbolTable* zCParserGetSymbolTable(void* parser)
{
	BYTE* pointer = ((BYTE*)parser) + 0x10;
	return (g2ext_extended::zCPar_SymbolTable*)pointer;
}


ObjectManager* ObjectManager::getObjectManager() {
	if (instanz == nullptr)
	{
		instanz = new ObjectManager();
	}
	return instanz;
};

void ObjectManager::release() {
	if (instanz != nullptr) {
		instanz->releaseInstances();
		delete instanz;
		instanz = nullptr;
	}
};

int ObjectManager::createNewInstanceId(oCItem* item) {
	if (item == nullptr) return 0;

	//Create new zCPar_Symbol for the newly created instance and register it to gothic.
	zCParser* parser = zCParser::GetParser();

	int parentId = getInstanceId(*item);
	zCPar_Symbol* old = parser->GetSymbol(parentId);

	if (old == nullptr) return 0;

	int* indexCount = getParserInstanceCount();
	int key = *indexCount;

	if (instanceBegin < 0)
	{
		instanceBegin = key;
	}
	/*
	changeKeyIfFreeIdAvailable(&key, *indexCount);
	logStream << "Tried to change key." << endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	*/


	zCPar_Symbol* symbol = createNewSymbol(key, old);

	if (!addSymbolToSymbolTable(symbol))
	{
		if (*indexCount < key) *indexCount = key;
		if (*indexCount == key) *indexCount += 1;
	};

	DynInstance* instanceItem = createNewInstanceItem(key);
	if (IsModified(item))
	{
		DynInstance* oldStoreItem = getInstanceItem(parentId);
		instanceItem->copyUserData(*oldStoreItem);
		if (instanceBegin > parentId)
		{
			instanceBegin = parentId;
		}
	}
	instanceItem->store(*item);

	setParentInstanceId(key, parentId);

	instanceItem->setParserSymbolBitfield(symbol->bitfield);
	instanceItem->setZCPar_SymbolName(symbol->name.ToChar());
	instanceItem->setInstanceID(key);

	newInstanceToSymbolMap.insert(pair<int, zCPar_Symbol*>(key, symbol));

	string symbolName = symbol->name.ToChar();
	//transform(symbolName.begin(), symbolName.end(), symbolName.begin(), ::toupper);
	nameToSymbolMap.insert(pair<string, zCPar_Symbol*>(symbolName, symbol));
	nameToIndexMap.insert(pair<string, int>(symbolName, key));

	logStream << "ObjectManager::createNewInstanceId(): indexCount = " << *indexCount << endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << "ObjectManager::createNewInstanceId(): key = " << key << endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);

	return key;
};

void ObjectManager::createInstanceById(int id, DynInstance* item) {
	// allow no reassignments
	map<int, DynInstance*>::iterator it;
	it = instanceMap.find(id);
	if (it != instanceMap.end())
	{
		logStream << "Warning: instance id already exists! Nothing will be created." << endl;
		Logger::getLogger()->log(Logger::Info, &logStream, true, true, true);
		return;
	}

	instanceMap.insert(pair<int, DynInstance*>(id, item));

	if (instanceBegin < 0 || instanceBegin > id)
	{
		instanceBegin = id;
	}
}

void ObjectManager::releaseInstances() {
	instanceBegin = -1;

	//release all allocated parser symbols and update parser symbol table
	zCParser* parser = zCParser::GetParser();
	g2ext_extended::zCPar_SymbolTable* currSymbolTable = zCParserGetSymbolTable(parser);
	int allocatedSize = instanceMap.size();
	int* symTableSize = getParserInstanceCount();
	*symTableSize = *symTableSize - allocatedSize;
	logStream << "ObjectManager::releaseInstances(): allocatedSize = " << allocatedSize << endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	logStream << "ObjectManager::releaseInstances(): symTableSize = " << *symTableSize << endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);

	// Release all allocated memory referenced by the instance map
	map<int, DynInstance*>::iterator it;
	for (it = instanceMap.begin(); it != instanceMap.end(); ++it) {
		DynInstance* item = (*it).second;
		delete item;
		(*it).second = nullptr;
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
	if (it == instanceMap.end())
	{
		logStream<< "Warning: instance id wasn't found: " << id << std::endl;
		Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
		return false;
	}
	
	setDynInstanceId(item, id);

	if(!initByNewInstanceId(item))
	{
		logStream << "Warning: Item Initialisation failed!" << std::endl;
		Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
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
	if (item == nullptr) return NULL;
	int instanceId = getInstanceId(*item);
	map<int, DynInstance*>::iterator it;
	it = instanceMap.find(instanceId);
	if (it == instanceMap.end()){return NULL;}
	return instanceId;
}

void ObjectManager::setDynInstanceId(oCItem* item, int id){
	std::map<int, DynInstance*>::iterator it;
	it = instanceMap.find(id);
	if (it != instanceMap.end())
	{
		setInstanceId(item, id);
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
		return nullptr;		
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
	if (ifs.fail()) {
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
		info.oldInstanceId = storeItem->getParentInstanceId();
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
	while(vobList != nullptr) {
		zCVob* vob = vobList->GetData();
		vobList = vobList->GetNext();
		if (vob == nullptr){
			continue;
		}

		if (strcmp(typeid(*vob).name(), typeid(*dummy).name()) == 0) {
			containerList->push_back(dynamic_cast<oCMobContainer*>(vob));
		}
	}
	delete dummy;
	return containerList;
};

void ObjectManager::changeKeyIfFreeIdAvailable(int* key, int indexCount)
{
	set<int> usedIds;
	auto func = [&](oCItem* itm)
	{
		if (itm == nullptr) return;
		int instanceId = getInstanceId(*itm);
		if (instanceId >= instanceBegin)
		{
			usedIds.insert(instanceId);
		}
	};

	//call the collection function func for all items in the world
	callForAllItems(func);

	for (auto it = usedIds.begin(); it != usedIds.end(); ++it)
	{
		logStream << "usedId: " << *it << endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
	}

	auto first = usedIds.begin();
	auto second = ++usedIds.begin();
	int diff;
	bool foundId = false;
	for (; second != usedIds.end() && (first != usedIds.end()); ++first, ++second)
	{
		diff = *second - *first;
		logStream << "first: " << *first << endl;
		logStream << "second: " << *second << endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
		if (diff <= 0)
		{
			logStream << "ObjectManager::createNewInstanceId: strange diff: " << diff << endl;
			Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
		}
		else if (diff > 1)
		{
			//there exists a dyn. instance id with no item using it. We gonna reuse it.
			*key = *first + 1;
			foundId = true;
			break;
		}
	}

	//compare last element of usedIds with the last index if no usable id was previously found
	if (usedIds.begin() != usedIds.end() && !foundId)
	{
		int lastUsedId = *(--usedIds.end());
		int lastIndex = indexCount - 1;
		logStream << "lastUsedId: " << lastUsedId << endl;
		logStream << "lastIndex: " << lastIndex << endl;
		Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
		if (lastIndex - lastUsedId >= 1)
		{
			*key = lastUsedId + 1;
		}
	}
};




zCPar_Symbol* ObjectManager::createNewSymbol(ParserInfo* old)
{
	zCParser* parser = zCParser::GetParser();
	zCPar_Symbol* ref = parser->GetSymbol(old->oldInstanceId);
	g2ext_extended::zCPar_SymbolTable* currSymbolTable = zCParserGetSymbolTable(parser);
	zCPar_Symbol* result = currSymbolTable->GetSymbol(old->name.c_str());
	if (result) { return result; }

	zCPar_Symbol* parent = parser->GetSymbol("C_ITEM");

	result = new zCPar_Symbol();
	result->parent = ref->parent;
	result->bitfield = ref->bitfield;
	result->name = zSTRING(old->name.c_str());
	result->offset = 0;
	result->next = ref->next;
	result->content.data_ptr = 0;//ref->content.data_ptr;
	result->filenr = ref->filenr;//ZCPAR_SYMBOL_MARK_ID;
	result->line = ref->line;
	result->line_anz = ref->line_anz;
	result->pos_beg = ref->pos_beg;
	result->pos_anz = ref->pos_anz;

	return result;
}

bool ObjectManager::addSymbolToSymbolTable(zCPar_Symbol* symbol)
{
	zCParser* parser = zCParser::GetParser();
	int* indexCount = getParserInstanceCount();
	int countBefore = *indexCount;
	g2ext_extended::zCPar_SymbolTable* symbolTable = zCParserGetSymbolTable(parser);
	symbolTable->Insert(symbol);
	logStream << "ObjectManager::addSymbolToSymbolTable(): Name = " << symbol->name.ToChar() << endl;
	logStream << "ObjectManager::addSymbolToSymbolTable(): Index = " << parser->GetIndex(symbol->name) << endl;
	logStream << "ObjectManager::addSymbolToSymbolTable(): countBefore = " << countBefore << endl;
	logStream << "ObjectManager::addSymbolToSymbolTable(): index count = " << *indexCount << endl;
	Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);

	// Some Ikarus functions need the correct length of the current symbol table.
	updateIkarusSymbols();

	return countBefore != *indexCount;
}


DynInstance* ObjectManager::createNewInstanceItem(int instanceId)
{
	DynInstance* result = getInstanceItem(instanceId);
	if (!result)
	{
		result = new DynInstance();
		instanceMap.insert(pair<int, DynInstance*>(instanceId, result));
	}

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
	vector<ParserInfo>::iterator it;
	for (it = indexZCParSymbolNameMap.begin(); it != indexZCParSymbolNameMap.end(); ++it)
	{
		ParserInfo info = *it;
		zCPar_Symbol* symbol = createNewSymbol(&info);
		zCParser* parser = zCParser::GetParser();
		if (!addSymbolToSymbolTable(symbol))
		{
			logStream << "adding wasn't successful: " << symbol->name.ToChar() << endl;
			Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
			g2ext_extended::zCPar_SymbolTable* currSymbolTable = zCParserGetSymbolTable(parser);
			int index = currSymbolTable->GetIndex(info.name.c_str());
			int* symTableSize = getParserInstanceCount();
			logStream << "index: " << index << endl;
			logStream << "symTableSize: " << *symTableSize << endl;
			Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
			if (index >= *symTableSize)
			{
				*symTableSize = index + 1;
				updateIkarusSymbols();
				logStream << "ObjectManager::createParserSymbols(): symTableSize = " << *symTableSize << endl;
				Logger::getLogger()->log(Logger::Info, &logStream, false, true, true);
			}
		}

		int index = parser->GetIndex(symbol->name);

		if (index != info.newInstanceId)
		{
			logStream << "ObjectManager::createParserSymbols(): index and newInstanceId don't match!" << endl;
			logStream << "index: " << index << ", newInstanceId: " << info.newInstanceId << endl;
			Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
		}

		int newInstanceId = info.newInstanceId;
		updateContainerItem(&info);
		newInstanceToSymbolMap.insert(pair<int, zCPar_Symbol*>(newInstanceId, symbol));
		string symbolName = symbol->name.ToChar();
		transform(symbolName.begin(), symbolName.end(), symbolName.begin(), ::toupper);
		nameToSymbolMap.insert(pair<string, zCPar_Symbol*>(symbolName, symbol));
		nameToIndexMap.insert(pair<string, int>(symbolName, index));
		//zCPar_Symbol* result = zCParser::GetParser()->GetSymbol(index);
		//logSymbolData(result);
	}
}

zCPar_Symbol* ObjectManager::createNewSymbol(int instanceId, zCPar_Symbol* old)
{

	zCPar_Symbol* symbol;
	zCParser* parser = zCParser::GetParser();
	//zCPar_Symbol* ref = parser->GetSymbol(parser->GetIndex("ItMw_1h_vlk_dagger"));

	symbol = parser->GetSymbol(instanceId);

	// Symbol already exists?
	if (symbol) return symbol;

	symbol = new zCPar_Symbol();
	ZeroMemory(symbol, sizeof(zCPar_Symbol));

	stringstream name; name << "DII_" << instanceId;
	symbol->name = zSTRING(name.str().c_str());
	symbol->parent = old->parent;
	symbol->bitfield = old->bitfield;
	symbol->offset = 0;
	symbol->content.data_ptr = 0;
	/*symbol->next = old->next;
	symbol->content.data_ptr = 0;//ref->content.data_ptr;
	symbol->filenr = old->filenr;//ZCPAR_SYMBOL_MARK_ID;
	symbol->line = old->line;
	symbol->line_anz = old->line_anz;
	symbol->pos_beg = old->pos_beg;
	symbol->pos_anz = old->pos_anz;*/
	/*symbol->parent = old->parent;
	symbol->bitfield = old->bitfield;
	symbol->offset = 0;
	symbol->next = nullptr;
	symbol->content.data_ptr = nullptr; DII_TestConstructor
	symbol->filenr = ZCPAR_SYMBOL_MARK_ID;*/

	return symbol;
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
	// write right save game folder 
	if (saveGameSlotNumber == 0) //quicksave folder
	{
		ss << gothicDirPath.ToChar() << saveGamesDirName.ToChar() << "quicksave";
	} else
	{
		ss << gothicDirPath.ToChar() << saveGamesDirName.ToChar() << "savegame" << saveGameSlotNumber;
	}
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

void ObjectManager::createAdditionalMemory(oCItem* item, int id, bool isHeroItem, bool activeSpellItem, int spellKey){
	if (item->instanz <= 0)
	{
		std::cout << "Warning: item->instanz <= 0!" << std::endl;
		return;
	}

	AdditMemory* addit = new AdditMemory();
	int additKey = calcAdditKey(isHeroItem);
	addit->additId = additKey;
	addit->instanceId = id;
	addit->referenceCount = 1;
	addit->flags = item->flags;
	addit->instanz = item->instanz;
	addit->activeSpellItem = activeSpellItem;
	addit->spellKey = spellKey;
	item->instanz = -additKey;
	keyToAdditMap.insert(pair<int,AdditMemory*>(addit->additId, addit));
}

void ObjectManager::removeAdditionalMemory(int additId){
	bool isHeroItem = additId > HERO_ADDIT_BEGIN;
	map<int, AdditMemory*>::iterator it;
	it = keyToAdditMap.find(additId);
	if (it == keyToAdditMap.end()) return;
	AdditMemory* addit = it->second;
	keyToAdditMap.erase(it);
	if (!isHeroItem) {
		nextAdditKeys.push(addit->additId);
	}
	//writeToConsole("Deleted additional memory", 0);
	//writeToConsole("additId: ", addit->additId);
	delete addit;
}

void ObjectManager::removeAllAdditionalMemory(){
	map<int, AdditMemory*>::iterator itBegin = keyToAdditMap.begin();
	map<int, AdditMemory*>::iterator itEnd = keyToAdditMap.end();
	
	while(itBegin != itEnd) {
		removeAdditionalMemory(itBegin->first);
		itBegin = keyToAdditMap.begin();
		itEnd = keyToAdditMap.end();
	}
}

bool ObjectManager::hasAdditAssignment(oCItem& item){
	AdditMemory* addit = getAddit(item);
	return (addit != NULL);
}

// Provides the id of additional memory for a given oCItem
int ObjectManager::getAdditId(oCItem& item) {
	if (item.instanz < 0) return item.instanz;
	//if (getInstanceId(item) >= INSTANCE_BEGIN) return getInstanceId(item);
	return getInstanceId(item);
	//return NULL;
}

// Provides additional Memory for a specific additional memory id
AdditMemory* ObjectManager::getAddit(int additId) {
	map<int,AdditMemory*>::iterator it;
	it = keyToAdditMap.find(additId);
	if (it == keyToAdditMap.end()) return NULL;
	return it->second;
}

AdditMemory* ObjectManager::getAddit(oCItem& item) {
	int id = getAdditId(item);
	return getAddit(id);
}

void ObjectManager::removeAdditList(std::list<AdditMemory*>* list){
	while (!list->empty()) {
		AdditMemory* addit = list->front();
		list->pop_front();
		removeAdditionalMemory(addit->additId);
	}
};

int ObjectManager::calcAdditKey(bool isHeroItem){
	if (isHeroItem) {
		return HERO_ADDIT_BEGIN + (static_cast<int>(keyToAdditMap.size()) +1);
	}
	return (static_cast<int>(keyToAdditMap.size()) + 1);
};

void ObjectManager::loadWorldObjects(char* filename) {
	ifstream ifs(filename);
	if (ifs.fail()) {
		logStream << "ObjectManager::loadWorldObjects: Warning: File was not found!" << std::endl;
		logStream << "ObjectManager::loadWorldObjects: File name : " << filename << std::endl;
		Logger::getLogger()->log(Logger::Warning, &logStream, false, true, true);
		return;
	}

	// create and open an archive for input
    boost::archive::text_iarchive ia(ifs);

	// Load additional memory informations
	int size;
	ia >> size;
	for (size_t i = 0; i != size; ++i) {
		AdditMemory* addit;
		ia >> addit;
		addit->referenceCount = 0;
		keyToAdditMap.insert(pair<int,AdditMemory*>(addit->additId, addit));
	}
};

void ObjectManager::loadHeroData(char* filename) {
	ifstream ifs(filename);
	if (ifs.fail()) {
		//writeToConsole("ERROR loadHeroData: file was not found!");
		return;
	}

	// create and open an archive for input
    boost::archive::text_iarchive ia(ifs);
	// Load additional memory informations
	int size;
	ia >> size;
	for (size_t i = 0; i != size; ++i) {
		AdditMemory* addit;
		ia >> addit;
		//writeToConsole("addit: ", i);
		addit->referenceCount = 0;
		keyToAdditMap.insert(pair<int,AdditMemory*>(addit->additId, addit));
	}
};


void ObjectManager::saveHeroData(std::list<AdditMemory*> heroItemList, char* directoryPath, char* filename) {
	std::string dir (directoryPath);
	if (!util::existsDir(dir)) {
		if(!util::makePath(dir)) {
			//writeToConsole("ERROR ObjectManager::saveHeroData : Couldn't access directory!");
			return;
		}
	}

	std::string fullpath = dir + std::string("\\") + std::string(filename);
	ofstream fs(const_cast<char*>(fullpath.c_str()));
	 boost::archive::text_oarchive oa(fs);
	// Save additional memory
	int size = heroItemList.size();
	//writeToConsole("size of additional memory: ", size);
	std::list<AdditMemory*>::iterator additIt;
	oa << size;
	for (additIt = heroItemList.begin(); additIt != heroItemList.end(); ++additIt) {
		AdditMemory* addit = *additIt;
		oa << addit;
	}
};

void ObjectManager::saveWorldObjects(int heroItemSize, char* directoryPath, char* filename) {
	std::string dir (directoryPath);
	if (!util::existsDir(dir)) {
		if(!util::makePath(dir)) {
			return;
		}
	}

	std::string fullpath = dir + std::string("\\") + std::string(filename);
	ofstream fs(const_cast<char*>(fullpath.c_str()));
	 boost::archive::text_oarchive oa(fs);
	// Save additional memory
	int size = keyToAdditMap.size() - heroItemSize;
	//writeToConsole("size of additional memory: ", size);
	map<int,AdditMemory*>::iterator additIt;
	oa << size;
	for (additIt = keyToAdditMap.begin(); additIt != keyToAdditMap.end(); ++additIt) {
		AdditMemory* addit = additIt->second;
		if (addit->additId < HERO_ADDIT_BEGIN) {
			oa << addit;
		}
	}
};

void ObjectManager::getHeroAddits(std::list<AdditMemory*>& list) {
	std::map<int, AdditMemory*>::iterator it = keyToAdditMap.begin();
	for (; it != keyToAdditMap.end(); ++it) {
		if (it->first > HERO_ADDIT_BEGIN) {
			list.push_back(it->second);
		}
	}
}


void ObjectManager::updateIkarusSymbols()
{
	// Some Ikarus functions need the correct length of the current symbol table.
	// MEM_Reinit_Parser() updates all involved references.
		zSTRING arg("MEM_ReinitParser");

		//CallFunc needs uppercase string
		zCParser::GetParser()->CallFunc(arg.Upper());
}

void ObjectManager::callForAllItems(function<void(oCItem*)> func)
{
	zCWorld* world = oCGame::GetGame()->GetWorld();
	zCListSort<oCNpc>* npcList = world->GetNpcList();
	std::list<oCItem*> tempList;
	std::list<oCItem*>::iterator it;

	while (npcList != nullptr) {
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
		while (list != nullptr) {
			oCItem* item = list->GetData();
			if (item != nullptr) tempList.push_back(item);

			list = list->GetNext();
		}
		npcList = npcList->GetNext();

		for (it = tempList.begin(); it != tempList.end(); ++it)
		{
			func(*it);
		}
		tempList.clear();
	}

	tempList.clear();
	zCListSort<oCItem>* itemList = world->GetItemList();
	while (itemList != nullptr) {
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
		func(*it);
	}
	tempList.clear();

	list<oCMobContainer*>* containerList = getMobContainers();
	auto contIt = containerList->begin();
	for (; contIt != containerList->end(); ++contIt)
	{
		oCMobContainer* container = *contIt;
		int address = (int)container->containList_next;
		zCListSort<oCItem>* listAddress = reinterpret_cast<zCListSort<oCItem>*>(address);
		zCListSort<oCItem>* list = listAddress;

		while (list != nullptr) {
			oCItem* item = list->GetData();
			if (item != nullptr) {
				tempList.push_back(item);
			}
			list = list->GetNext();
		}

		it = tempList.begin();
		for (; it != tempList.end(); ++it)
		{
			func(*it);
		}
		tempList.clear();
	}

	// containerList has to be deleted manually.
	containerList->clear();
	delete containerList;
}

int ObjectManager::getInstanceBegin()
{
	return instanceBegin;
}

int* ObjectManager::getParserInstanceCount()
{
	zCParser* parser = zCParser::GetParser();
	return (int*)(((BYTE*)parser) + 0x18 + 0x8);
}

int ObjectManager::getIdForSpecialPurposes()
{
	return SPECIAL_ADDIT_BEGIN;
}
