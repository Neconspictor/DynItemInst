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
#include <api/g2/oCObjectFactory.h>
#include <oCItemExtended.h>
using namespace std;

ObjectManager* ObjectManager::instanz = nullptr;

//.text:0073ABE0 public: class oCVob * __thiscall oCNpc::GetLeftHand(void) proc near
typedef oCVob* (__thiscall* OCNpcGetLeftHand)(oCNpc*);
OCNpcGetLeftHand oCNpcGetLeftHand = (OCNpcGetLeftHand) 0x0073ABE0;

//.text:0073AB50 public: class oCVob * __thiscall oCNpc::GetRightHand(void) proc near
typedef oCVob* (__thiscall* OCNpcGetRightHand)(oCNpc*);
OCNpcGetRightHand oCNpcGetRightHand = (OCNpcGetRightHand)0x0073ABE0;


ObjectManager::ObjectManager()
{
	this->instanceMap = map<int, DynInstance*>();
	this->nextInstances = queue<int>();
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

	if (reusableInstances.size() > 0)
	{
		key = reusableInstances.front();
		reusableInstances.pop_front();
		createNewInstanceForExistingId(item, key);
	} else
	{
		createNewInstanceWithoutExistingId(item, key);
	}

	return key;
};

void ObjectManager::createInstanceById(int id, DynInstance* item) {
	// allow no reassignments
	map<int, DynInstance*>::iterator it;
	it = instanceMap.find(id);
	if (it != instanceMap.end())
	{
		logStream << "Instance id already exists! Nothing will be created." << endl;
		util::debug(&logStream, Logger::Warning);
		return;
	}

	instanceMap.insert(pair<int, DynInstance*>(id, item));

	if (instanceBegin < 0 || instanceBegin > id)
	{
		instanceBegin = id;
	}

	if (item->notUsed)
	{
		reusableInstances.push_back(id);
		logStream << "added DynInstance to reusableInstances" << std::endl;
		util::debug(&logStream);
	}
}

void ObjectManager::releaseInstances() {
	instanceBegin = -1;

	//release all allocated parser symbols and update parser symbol table
	int allocatedSize = instanceMap.size();
	int* symTableSize = getParserInstanceCount();
	*symTableSize = *symTableSize - allocatedSize;
	logStream << "ObjectManager::releaseInstances(): allocatedSize = " << allocatedSize << endl;
	util::debug(&logStream);
	logStream << "ObjectManager::releaseInstances(): symTableSize = " << *symTableSize << endl;
	util::debug(&logStream);

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
	reusableInstances.clear();
};

bool ObjectManager::assignInstanceId(oCItem* item, int id){
	map<int,DynInstance*>::iterator it;
	it = instanceMap.find(id);
	if (it == instanceMap.end())
	{
		logStream<< "ObjectManager::assignInstanceId: instance id wasn't found: " << id << std::endl;
		util::debug(&logStream, Logger::Warning);
		return false;
	}
	
	setDynInstanceId(item, id);

	if(!initByNewInstanceId(item))
	{
		logStream << "ObjectManager::assignInstanceId: Item Initialisation failed!" << std::endl;
		util::debug(&logStream, Logger::Warning);
		return false;
	};
	return true;
}

void oCItemOperatorDelete(oCItem* item)
{
	XCALL(0x007144A0);
}

bool ObjectManager::assignInstanceId2(oCItem* item, int id)
{
	//TODO: get rid of the copy item!!!
	oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(id);

	if (!copy) return false;
	
	item->idx = copy->idx;
	item->name = copy->name;
	item->nameID = copy->nameID;
	item->hp = copy->hp;
	item->hp_max = copy->hp_max;
	item->mainflags = copy->mainflags;
	item->flags = copy->flags;
	item->weight = copy->weight;
	item->value = copy->value;

	// -- weapons		
	item->damageType = copy->damageType;
	item->damageTotal = copy->damageTotal;

	item->damage[0] = copy->damage[0];
	item->damage[1] = copy->damage[1];
	item->damage[2] = copy->damage[2];
	item->damage[3] = copy->damage[3];
	item->damage[4] = copy->damage[4];
	item->damage[5] = copy->damage[5];
	item->damage[6] = copy->damage[6];
	item->damage[7] = copy->damage[7];

	// -- armor 
	item->wear = copy->wear;
	item->protection[0] = copy->protection[0];
	item->protection[1] = copy->protection[1];
	item->protection[2] = copy->protection[2];
	item->protection[3] = copy->protection[3];
	item->protection[4] = copy->protection[4];
	item->protection[5] = copy->protection[5];
	item->protection[6] = copy->protection[6];
	item->protection[7] = copy->protection[7];

	// -- food
	item->nutrition = copy->nutrition;

	// -- use attributes
	item->cond_atr[0] = copy->cond_atr[0];
	item->cond_atr[1] = copy->cond_atr[1];
	item->cond_atr[2] = copy->cond_atr[2];

	item->cond_value[0] = copy->cond_value[0];
	item->cond_value[1] = copy->cond_value[1];
	item->cond_value[2] = copy->cond_value[2];

	// -- attributes that will be changed on equip
	item->change_atr[0] = copy->change_atr[0];
	item->change_atr[1] = copy->change_atr[1];
	item->change_atr[2] = copy->change_atr[2];

	item->change_value[0] = copy->change_value[0];
	item->change_value[1] = copy->change_value[1];
	item->change_value[2] = copy->change_value[2];

	// -- parser functions
	item->magic = copy->magic;
	item->on_equip = copy->on_equip;
	item->on_unequip = copy->on_unequip;

	item->on_state[0] = copy->on_state[0];
	item->on_state[1] = copy->on_state[1];
	item->on_state[2] = copy->on_state[2];
	item->on_state[3] = copy->on_state[3];

	// -- owner									
	item->owner = copy->owner;			//	owner: npc instance
	item->ownerGuild = copy->ownerGuild;		//	owner: guild
	item->disguiseGuild = copy->disguiseGuild;

	// -- visual
	item->visual = copy->visual;

	// -- change of mesh on equip
	item->visual_change = copy->visual_change;	//	ASC file
	item->effect = copy->effect;			//  Effect instance

	item->visual_skin = copy->visual_skin;

	item->scemeName = copy->scemeName;
	item->material = copy->material;
	item->munition = copy->munition;		//	Instance of ammunition

	item->spell = copy->spell;
	item->range = copy->range;

	item->mag_circle = copy->mag_circle;

	item->description = copy->description;
	item->text[0] = copy->text[0];
	item->text[1] = copy->text[1];
	item->text[2] = copy->text[2];
	item->text[3] = copy->text[3];
	item->text[4] = copy->text[4];
	item->text[5] = copy->text[5];

	item->count[0] = copy->count[0];
	item->count[1] = copy->count[1];
	item->count[2] = copy->count[2];
	item->count[3] = copy->count[3];
	item->count[4] = copy->count[4];
	item->count[5] = copy->count[5];

	// -- inventory presentation
	item->inv_zbias = copy->inv_zbias;								//  far plane (how far the item goes into the room by the z-axis)
	item->inv_rotx = copy->inv_rotx;								//  rotation around x-axis (in degrees)
	item->inv_roty = copy->inv_roty;								//  rotation around y-axis (in degrees)
	item->inv_rotz = copy->inv_rotz;								//  rotation around z-axis (in degrees)
	item->inv_animate = copy->inv_animate;							//  rotate the item
																// Skip instanz field!!!
																//item->instanz=instanz;						//int Symbolindex
	item->c_manipulation = copy->c_manipulation;					//int ?
	item->last_manipulation = copy->last_manipulation;				//zREAL ?
	item->magic_value = copy->magic_value;					//int ?
	//item->effectVob = effectVob;						//oCVisualFX*
	item->next = copy->next;
	int address = reinterpret_cast<int>(item);
	address += 0x330;
	int* instance = reinterpret_cast<int*>(address);

	//Get current symbol index and set it as the item's instance id
	*instance = id;

	//insert effect finally
	oCItemSaveRemoveEffect(item);
	oCItemSaveInsertEffect(item);

	//remove copy item
	//oCItemOperatorDelete(copy);
	//copy = nullptr;
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
		logStream << "ObjectManager::setInstanceId: parameter id has no assigned index. Nothing will be done." << endl;
		util::debug(&logStream, Logger::Warning);
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

	string fullpath = dir + string("\\") + string(filename);
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

list<oCMobContainer*>* ObjectManager::getMobContainers() {
	list<oCMobContainer*>* containerList = new list<oCMobContainer*>();
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
		util::debug(&logStream);
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
		util::debug(&logStream);
		if (diff <= 0)
		{
			logStream << "ObjectManager::createNewInstanceId: strange diff: " << diff << endl;
			util::debug(&logStream);
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
		util::debug(&logStream);
		if (lastIndex - lastUsedId >= 1)
		{
			*key = lastUsedId + 1;
		}
	}
}

void ObjectManager::createNewInstanceWithoutExistingId(oCItem* item, int key)
{
	zCParser* parser = zCParser::GetParser();
	int parentId = getInstanceId(*item);
	zCPar_Symbol* old = parser->GetSymbol(parentId);

	if (old == nullptr) return;

	int* indexCount = getParserInstanceCount();
	zCPar_Symbol* symbol = createNewSymbol(key, old);

	if (!addSymbolToSymbolTable(symbol))
	{
		if (*indexCount < key) *indexCount = key;
		if (*indexCount == key) *indexCount += 1;
	};

	DynInstance* instanceItem = createNewInstanceItem(key);
	if (IsModified(item))
	{
		/*DynInstance* oldStoreItem = getInstanceItem(parentId);
		instanceItem->copyUserData(*oldStoreItem);
		if (instanceBegin > parentId)
		{
			instanceBegin = parentId;
		}*/
	}
	instanceItem->store(*item);

	setParentInstanceId(key, parentId);

	instanceItem->setParserSymbolBitfield(symbol->bitfield);
	instanceItem->setZCPar_SymbolName(symbol->name.ToChar());
	instanceItem->setInstanceID(key);

	newInstanceToSymbolMap.insert(pair<int, zCPar_Symbol*>(key, symbol));

	string symbolName = symbol->name.ToChar();
	nameToSymbolMap.insert(pair<string, zCPar_Symbol*>(symbolName, symbol));
	nameToIndexMap.insert(pair<string, int>(symbolName, key));

	logStream << "ObjectManager::createNewInstanceWithoutExistingId(): indexCount = " << *indexCount << endl;
	util::debug(&logStream);
	logStream << "ObjectManager::createNewInstanceWithoutExistingId(): key = " << key << endl;
	util::debug(&logStream);
}

void ObjectManager::createNewInstanceForExistingId(oCItem* item, int instanceId)
{
	DynInstance* instanceItem = getInstanceItem(instanceId);
	int parentId = getInstanceId(*item);
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
	instanceItem->setInstanceID(instanceId);
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
	util::debug(&logStream);

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

void ObjectManager::updateContainerItem(ObjectManager::ParserInfo* info)
{
	DynInstance* item = info->container;
	item->setInstanceID(info->newInstanceId);
	item->setZCPar_SymbolName(info->name);
	item->setParserSymbolBitfield(info->bitfield);

	if (item->notUsed)
	{
		reusableInstances.push_back(item->getInstanceID());
	}
};

void ObjectManager::logSymbolData(zCPar_Symbol* sym)
{
	logStream << sym->name.ToChar() << endl;
	util::logInfo(&logStream);
	logStream << sym->parent->name.ToChar() << endl;
	util::logInfo(&logStream);
	logStream<< sym->bitfield << endl;
	util::logInfo(&logStream);
	logStream << sym->filenr << endl;
	util::logInfo(&logStream);
	logStream << sym->line << endl;
	util::logInfo(&logStream);
	logStream << sym->line_anz << endl;
	util::logInfo(&logStream);
	logStream << sym->next << endl;
	util::logInfo(&logStream);
	logStream << sym->offset << endl;
	util::logInfo(&logStream);
	logStream << sym->pos_beg << endl;
	util::logInfo(&logStream);
	logStream << sym->pos_anz << endl;
	util::logInfo(&logStream);
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
			logStream << "ObjectManager::createParserSymbols: Adding wasn't successful: " << symbol->name.ToChar() << endl;
			g2ext_extended::zCPar_SymbolTable* currSymbolTable = zCParserGetSymbolTable(parser);
			int index = currSymbolTable->GetIndex(info.name.c_str());
			int* symTableSize = getParserInstanceCount();
			logStream << "index: " << index << endl;
			logStream << "symTableSize: " << *symTableSize << endl;
			util::debug(&logStream, Logger::Warning);
			if (index >= *symTableSize)
			{
				*symTableSize = index + 1;
				updateIkarusSymbols();
				logStream << "ObjectManager::createParserSymbols(): resized symbol table. symTableSize = " << *symTableSize << endl;
				util::logInfo(&logStream);
			}
		}

		int index = parser->GetIndex(symbol->name);

		if (index != info.newInstanceId)
		{
			logStream << "ObjectManager::createParserSymbols(): index and newInstanceId don't match!" << endl;
			logStream << "index: " << index << ", newInstanceId: " << info.newInstanceId << endl;
			util::logFatal(&logStream);
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
		logStream << "ObjectManager::InitItemWithDynInstance: instanceId wasn't found!" << std::endl;
		util::logWarning(&logStream);
		return false;
	}
	return assignInstanceId(item, index);
}

bool ObjectManager::IsModified(oCItem* item)
{
	int index = getDynInstanceId(item);
	auto it = instanceMap.find(index);
	if (it == instanceMap.end())
	{
		return false;
	} 
	return true;
}

bool ObjectManager::IsModified(int instanceId)
{
	auto it = instanceMap.find(instanceId);
	if (it == instanceMap.end())
	{
		return false;
	} 
	return true;
}

zCPar_Symbol* ObjectManager::getSymbolByIndex(int index)
{
	auto it = newInstanceToSymbolMap.find(index);
	if (it == newInstanceToSymbolMap.end()) { return nullptr; }
	return it->second;
}

zCPar_Symbol* ObjectManager::getSymbolByName(zSTRING symbolName)
{
	string name = symbolName.ToChar();
	transform(name.begin(), name.end(),name.begin(), ::toupper);
	auto it = nameToSymbolMap.find(name);
	if (it == nameToSymbolMap.end()) { return nullptr; }
	return it->second;
}


int ObjectManager::getIndexByName(zSTRING symbolName)
{
	string name = symbolName.ToChar();
	transform(name.begin(), name.end(),name.begin(), ::toupper);
	auto it = nameToIndexMap.find(name);
	if (it == nameToIndexMap.end()) { return NULL; }
	return it->second;
}

void ObjectManager::createAdditionalMemory(oCItem* item, int id, bool isHeroItem, bool activeSpellItem, int spellKey){
	if (item->instanz <= 0)
	{
		logStream << "ObjectManager::createAdditionalMemory: item->instanz <= 0!" << endl;
		util::logWarning(&logStream);
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
	auto it = keyToAdditMap.find(additId);
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
	auto itBegin = keyToAdditMap.begin();
	auto itEnd = keyToAdditMap.end();
	
	while(itBegin != itEnd) {
		removeAdditionalMemory(itBegin->first);
		itBegin = keyToAdditMap.begin();
		itEnd = keyToAdditMap.end();
	}
}

bool ObjectManager::hasAdditAssignment(oCItem& item){
	AdditMemory* addit = getAddit(item);
	return (addit != nullptr);
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
	auto it = keyToAdditMap.find(additId);
	if (it == keyToAdditMap.end()) return nullptr;
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
		logStream << "ObjectManager::loadWorldObjects: File was not found!" << std::endl;
		logStream << "File name : " << filename << std::endl;
		util::logWarning(&logStream);
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
	logStream << "ObjectManager::loadHeroData: load hero data..." << std::endl;
	util::logInfo(&logStream);

	ifstream ifs(filename);
	if (ifs.fail()) {
		logStream << "ObjectManager::loadHeroData: File wasn't found!" << endl;
		util::logWarning(&logStream);
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

	logStream << "ObjectManager::loadHeroData: done." << endl;
	util::logInfo(&logStream);
};


void ObjectManager::saveHeroData(std::list<AdditMemory*> heroItemList, char* directoryPath, char* filename) {
	logStream << "ObjectManager::saveHeroData: save hero data..." << endl;
	util::logInfo(&logStream);

	string dir (directoryPath);
	if (!util::existsDir(dir)) {
		if(!util::makePath(dir)) {
			logStream << "ObjectManager::saveHeroData: Couldn't access directory: " << directoryPath << endl;
			util::logFault(&logStream);
			return;
		}
	}

	string fullpath = dir + string("\\") + string(filename);
	ofstream fs(const_cast<char*>(fullpath.c_str()));
	 boost::archive::text_oarchive oa(fs);

	// Save additional memory
	int size = heroItemList.size();

	logStream << "ObjectManager::saveHeroData: size of additional memory : " << size << endl;
	util::logInfo(&logStream);

	list<AdditMemory*>::iterator additIt;
	oa << size;
	for (additIt = heroItemList.begin(); additIt != heroItemList.end(); ++additIt) {
		AdditMemory* addit = *additIt;
		oa << addit;
	}

	logStream << "ObjectManager::saveHeroData: done." << endl;
	util::logInfo(&logStream);
};

void ObjectManager::saveWorldObjects(int heroItemSize, char* directoryPath, char* filename) {
	logStream << "ObjectManager::saveWorldObjects: save dii world objects..." << endl;
	util::logInfo(&logStream);
	
	string dir (directoryPath);
	if (!util::existsDir(dir)) {
		if(!util::makePath(dir)) {
			logStream << "ObjectManager::saveWorldObjects: folder doesn't exists: " << directoryPath << endl;
			util::logFault(&logStream);
			return;
		}
	}

	string fullpath = dir + string("\\") + string(filename);
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

	logStream << "ObjectManager::saveWorldObjects: done." << endl;
	util::logInfo(&logStream);
};

void ObjectManager::getHeroAddits(std::list<AdditMemory*>& list) {
	map<int, AdditMemory*>::iterator it = keyToAdditMap.begin();
	for (; it != keyToAdditMap.end(); ++it) {
		if (it->first > HERO_ADDIT_BEGIN) {
			list.push_back(it->second);
		}
	}
}


void ObjectManager::updateIkarusSymbols()
{
	logStream << "ObjectManager::updateIkarusSymbols: update Ikarus symbols..." << endl;
	util::logInfo(&logStream);

	// Some Ikarus functions need the correct length of the current symbol table.
	// MEM_Reinit_Parser() updates all involved references.
		zSTRING arg("MEM_ReinitParser");

	//CallFunc needs uppercase string
	zCParser::GetParser()->CallFunc(arg.Upper());
}

void ObjectManager::callForAllItems(function<void(oCItem*)> func, oCItem** stopIfNotNullItem)
{
	zCWorld* world = oCGame::GetGame()->GetWorld();
	zCListSort<oCNpc>* npcList = world->GetNpcList();
	list<oCItem*> tempList;
	list<oCItem*>::iterator it;

	while (npcList != nullptr) {
		oCNpc* npc = npcList->GetData();
		if (npc == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}

		oCVob* leftHandVob = oCNpcGetLeftHand(npc);
		if (oCItem* leftHandItem = dynamic_cast<oCItem*>(leftHandVob))
		{
			func(leftHandItem);
			if (stopIfNotNullItem && *stopIfNotNullItem) {
				logStream << "ObjectManager::callForAllItems: leftHandItem!" << endl;
				util::debug(&logStream);
				return;
			}
		}

		if (oCItem* rightHandItem = dynamic_cast<oCItem*>(oCNpcGetRightHand(npc)))
		{
			func(rightHandItem);
			if (stopIfNotNullItem && *stopIfNotNullItem) {
				logStream << "ObjectManager::callForAllItems: rightHandItem!" << endl;
				util::debug(&logStream);
				return;
			}
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
			if (stopIfNotNullItem && *stopIfNotNullItem) {
				logStream << "ObjectManager::callForAllItems: found item in npc's inventory!" << endl;
				util::debug(&logStream);
				return;
			}
		}
		tempList.clear();
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
			if (stopIfNotNullItem && *stopIfNotNullItem) {
				// containerList has to be deleted manually.
				containerList->clear();
				delete containerList;
				if (stopIfNotNullItem && *stopIfNotNullItem) {
					logStream << "ObjectManager::callForAllItems: found item in container!" << endl;
					util::debug(&logStream);
					return;
				}
				return;
			}
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
		if (stopIfNotNullItem && *stopIfNotNullItem) {
			logStream << "ObjectManager::callForAllItems: found item in world!" << endl;
			util::debug(&logStream);
			return;
		}
	}
	tempList.clear();

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

void ObjectManager::markAsReusable(int instanceId, int previousId)
{
	DynInstance* instance = getInstanceItem(instanceId);
	if (!instance) return;

	auto func = [&](oCItem* item)->void {
		if (item && (item->GetInstance() == instanceId))
		{
			logStream << "item has instanceId marked as reusable: " << item->description.ToChar() << endl;
			util::debug(& logStream);
			initByNewInstanceId(item);
			setInstanceId(item, previousId);
		}
	};

	callForAllItems(func);

	instance->notUsed = true;
	reusableInstances.push_back(instanceId);
}

bool ObjectManager::isItemInWorld(oCItem* item)
{
	zCWorld* world = oCGame::GetGame()->GetWorld();
	zCListSort<oCItem>* list = world->GetItemList();
	return list->IsInList(item);
}

oCItem* ObjectManager::getItemByInstanceId(int instanceId)
{
	oCItem* result = nullptr;
	auto func = [&](oCItem* itm) ->void {
		if (itm == nullptr) return;
		if (result != nullptr) return;
		
		if (itm->GetInstance() == instanceId)
		{
			result = itm;
			logStream << "ObjectManager::getItemByInstanceId: result found" << endl;
			util::debug(&logStream);
		}
	};

	callForAllItems(func, &result);
	return result;
}

void ObjectManager::oCItemSaveInsertEffect(oCItem* item)
{
	if (item == nullptr)
	{
		return;
	}

	int* ptr = ((int*)item) + 0x340;

	//Has the item already an effect active?
	if (ptr != nullptr)
	{
		return;
	}

	item->InsertEffect();
}

void ObjectManager::oCItemSaveRemoveEffect(oCItem* item)
{
	if (item == nullptr)
	{
		return;
	}

	int* ptr = ((int*)item) + 0x340;

	//Has the item no effect to remove?
	if (ptr == nullptr)
	{
		return;
	}

	item->RemoveEffect();
}

bool ObjectManager::isDynamicInstance(int instanceId)
{
	map<int, DynInstance*>::iterator it;
	it = instanceMap.find(instanceId);
	if (it == instanceMap.end())
	{
		return false;
	}
	return true;
};