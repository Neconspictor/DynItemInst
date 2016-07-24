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
#include "api/g2/zcoption.h"
#include "api/g2/zcworld.h"
#include "HookManager.h"
#include "oCMobContainer.h"
#include "Util.h"
#include "api/g2/zcparser.h"
#include <ocgameExtended.h>
#include <Logger.h>
#include <set>
#include <zCPar_SymbolTable.h>
#include <api/g2/oCObjectFactory.h>
#include <oCItemExtended.h>
#include <Constants.h>

using namespace std;
using namespace constants;

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

bool ObjectManager::isValidWeapon(int weaponMode, oCItem * item)
{
	switch (weaponMode)
	{

	case 3:

		//const int ITEM_KAT_NF = 2;
		if (item->mainflags == 2)
		{
			return true;
		}
		break;
	case 4:
		//const int ITEM_KAT_NF = 2;
		if (item->mainflags == 2)
		{
			return true;
		}
		break;
	case 5:
		//const int ITEM_BOW = 1 << 19;
		if (item->HasFlag(1 << 19))
		{
			return true;
		}
		break;
	case 6:
		//const int ITEM_CROSSBOW = 1 << 20;
		if (item->HasFlag(1 << 20))
		{
			return true;
		}
		break;
	case 7:
		// TODO why return always true???
		//mainflag: ITEM_KAT_RUNE = 512
		if (item->mainflags == 512)
		{
			return true;
		}
		break;
	default: break;
	}

	return false;
}

bool ObjectManager::munitionOfItemUsesRightHand(oCItem* rangedWeapon)
{
	if (!isRangedWeapon(rangedWeapon))
	{
		return false;
	}

	//const int ITEM_BOW = 1<<19;
	if (rangedWeapon->HasFlag(1<<19))
	{
		return true;
	}
	
	//const int ITEM_CROSSBOW = 1 << 20;
	return false;
}


zCListSort<oCItem>* ObjectManager::getInvItemByInstanceId(oCNpcInventory* inventory, int instanceId)
{
	inventory->UnpackCategory();
	ObjectManager* manager = ObjectManager::getObjectManager();
	zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
	while (list != nullptr) {
		oCItem* item = list->GetData();
		if (item != nullptr && manager->getInstanceId(*item) == instanceId)
		{
			return list;
		}
		list = list->GetNext();
	}

	return nullptr;
}

int ObjectManager::getSlotNumber(oCNpcInventory* inventory, oCItem* item)
{
	int itemNumber = inventory->GetNumItemsInCategory();
	for (int i = 0; i < itemNumber; ++i)
	{
		oCItem* slotItem = inventory->GetItem(i);
		if (slotItem == item)
		{
			return i;
		}
	}

	//item isn't in the inventory
	return -1;
}

oCItem* ObjectManager::searchItemInInvbyInstanzValue(oCNpcInventory* inventory, int searchValue)
{
	inventory->UnpackCategory();
	int itemNumber = inventory->GetNumItemsInCategory();
	for (int i = 0; i < itemNumber; ++i)
	{
		oCItem* slotItem = inventory->GetItem(i);
		if (slotItem->instanz == searchValue)
		{
			return slotItem;
		}
	}

	//no item was found
	return nullptr;
}

void ObjectManager::drawWeaponSilently(oCNpc* npc, int weaponMode, int readedWeaponId, int munitionId, 
	bool munitionUsesRightHand, unordered_map<int, oCItem*>* equippedSpells, 
	oCItem** activeSpellItem, AdditMemory* addit, bool createCopy)
{
	oCNpcInventory* inventory = npc->GetInventory();
	zCListSort<oCItem>* node = getInvItemByInstanceId(inventory, readedWeaponId);
	if (!node)
	{
		logStream << "ObjectManager::drawWeaponSilently: node is null!" << std::endl;
		util::logFault(&logStream);
		return;
	}

	oCItem* item = node->GetData();

	if (!item)
	{
		logStream << "ObjectManager::drawWeaponSilently: item is null!" << std::endl;
		util::logFault(&logStream);
		return;
	}

	oCItem* copy = item;
	if (createCopy) copy = oCObjectFactory::GetFactory()->CreateItem(readedWeaponId);

	if (!item->HasFlag(512)) //item isn't a rune
	{
		int slotNumber = getSlotNumber(inventory, item);
		util::assertDIIRequirements(slotNumber >= 0, "slotNumber >= 0");
		logStream << "ObjectManager::drawWeaponSilently: slotnumber= " << slotNumber << std::endl;
		logStream << "item->description= " << item->description.ToChar() << std::endl;
		logStream << "item->GetInstance()= " << item->GetInstance() << std::endl;
		logStream << "item->instanz= " << item->instanz << std::endl;
		logStream << "copy->description= " << copy->description.ToChar() << std::endl;
		logStream << "copy->GetInstance()= " << copy->GetInstance() << std::endl;
		logStream << "copy->instanz= " << copy->instanz << std::endl;
		util::debug(&logStream);

		//DynItemInst::denyMultiSlot = false;
		if (createCopy)
		{
			inventory->Remove(item);

			//store some attribute to search for the copy after it was inserted into the inventory
			int copyStoreValue = copy->instanz;
			//assign any value that will be unique
			int searchValue = -6666666;
			copy->instanz = searchValue;
			inventory->Insert(copy);

			// Since multi-slotting was denied, copy is now on a own slot (not merged) and can be accessed
			copy = searchItemInInvbyInstanzValue(inventory, searchValue);
			util::assertDIIRequirements(copy != nullptr, "item to insert shouldn't be null!");
			copy->instanz = copyStoreValue;
		}

		//Deny invocation of equip function
		int equipFunction = copy->on_equip;
		copy->on_equip = 0;
		copy->ClearFlag(OCITEM_FLAG_EQUIPPED);
		npc->Equip(copy);

		//restore function

		logStream << "DynItemInst::restoreItem: item is now equipped!" << std::endl;
		logStream << "DynItemInst::restoreItem: Weapon mode: " << weaponMode << std::endl;
		util::debug(&logStream);
		copy = getInvItemByInstanceId(inventory, readedWeaponId)->GetData();
		copy->on_equip = equipFunction;

		oCNpcSetWeaponMode2(npc, weaponMode);  //3 for one hand weapons
	}

	// Is readied weapon a bow?
	if (copy && copy->HasFlag(1 << 19) && weaponMode == 5)
	{
		logStream << "DynItemInst::restoreItem: Bow is readied!" << std::endl;
		logStream << "DynItemInst::restoreItem: Weapon mode: " << weaponMode << std::endl;
		util::debug(&logStream);

		equipRangedWeapon(copy, inventory, true);
	}

	// Is readied weapon a crossbow?
	else if (copy && copy->HasFlag(1 << 20) && weaponMode == 6)
	{
		logStream << "DynItemInst::restoreItem: Crossbow is readied!" << std::endl;
		logStream << "DynItemInst::restoreItem: Weapon mode: " << weaponMode << std::endl;
		util::debug(&logStream);

		equipRangedWeapon(copy, inventory, false);
	}
	else if (item && item->HasFlag(512)) // Magic 
	{
		logStream << "DynItemInst::restoreItem: Readied weapon is a magic thing!" << std::endl;
		util::debug(&logStream);
		oCMag_Book* magBook = oCNpcGetSpellBook(npc);
		magBook = oCNpcGetSpellBook(npc);
		//int itemSpellKey = oCMag_BookGetKeyByItem(magBook, item);
		if (addit->spellKey > 0)
		{
			oCMag_BookDeRegisterItem(magBook, item);
			oCMag_BookNextRegisterAt(magBook, addit->spellKey);
		}
		if (addit && (addit->spellKey >= 0))
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

		magBook = oCNpcGetSpellBook(npc);
		if (magBook)
		{
			if (addit && (addit->activeSpellItem && activeSpellItem))
			{
				*activeSpellItem = item;
			}

			//logStream << "DynItemInst::restoreItem: selectedSpellKey = " << oCMag_BookGetSelectedSpellNr(magBook) << std::endl;
			//util::debug(&logStream);
			//logStream << "DynItemInst::restoreItem: An Spell is active" << std::endl;
			//util::debug(&logStream);
		}
	}
}

int ObjectManager::getSelectedSpellKey(oCNpc* npc)
{
	oCNpcInventory* inventory = npc->GetInventory();
	if (inventory == nullptr) {
		return -1;
	}
	inventory->UnpackAllItems();
	zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
	oCMag_Book* magBook = oCNpcGetSpellBook(npc);

	if (magBook) {
		oCSpell* selectedSpell = oCMag_BookGetSelectedSpell(magBook);
		if (selectedSpell)
		{
			return oCSpellGetSpellID(selectedSpell);
		}
	}

	return -1;
}

int ObjectManager::getEquippedSpellKeyByItem(oCNpc* npc, oCItem* item)
{
	int equipped = item->HasFlag(OCITEM_FLAG_EQUIPPED);
	oCMag_Book* magBook = oCNpcGetSpellBook(npc);
	int spellKey = -1;
	if (magBook && equipped) {
		spellKey = oCMag_BookGetKeyByItem(magBook, item);
		logStream << "ObjectManager::getEquippedSpellKeyByItem: spellKey = " << spellKey << std::endl;
		util::debug(&logStream);
	}
	return spellKey;
}

g2ext_extended::zCPar_SymbolTable* ObjectManager::zCParserGetSymbolTable(void* parser)
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
	if (item == nullptr) {
		logStream << "ObjectManager::createNewInstanceId: item is null!" << std::endl;
		util::logWarning(&logStream);
		return 0;
	}

	//Create new zCPar_Symbol for the newly created instance and register it to gothic.
	zCParser* parser = zCParser::GetParser();

	int parentId = getInstanceId(*item);
	zCPar_Symbol* old = parser->GetSymbol(parentId);


	if (old == nullptr) {
		logStream << "ObjectManager::createNewInstanceId: item parser symbol is null!" << std::endl;
		util::logWarning(&logStream);
		return 0;
	}



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

	if (item->isReusable())
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

void ObjectManager::resetDynItemInstances()
{
	for (auto it = instanceMap.begin(); it != instanceMap.end(); ++it)
	{
		DynInstance* instance = it->second;
		instance->resetActiveWorlds();
	}
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
	//oCItemSaveRemoveEffect(item);
	//oCItemSaveInsertEffect(item);

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

	string fullpath = dir + string(filename);
	ofstream ofs(const_cast<char*>(fullpath.c_str()));

    	// archive and stream closed when destructors are called
	map<int, DynInstance*>::iterator it = instanceMap.begin();
	size_t size = instanceMap.size();
	// Save instance items
	ofs << size << '\n';
	for (; it != instanceMap.end(); ++it) {
		ofs << it->first;
		ofs << ' ';
		DynInstance* storeItem = it->second;
		storeItem->serialize(ofs);
		//oa << storeItem;
		ofs << '\n';
	}
}


void ObjectManager::loadNewInstances(char* filename) {
	ifstream ifs(filename);
	if (ifs.fail()) {
		return;
	}

	indexZCParSymbolNameMap.clear();
	
	// archive and stream closed when destructors are called
	size_t size = 0;
	std::stringstream ss;
	std::string line, token;
	getline(ifs, line);
	ss << line;
	getline(ss, token);
	size = atoi(token.c_str());

	for (size_t i = 0; i != size; ++i) {
		ss.str("");
		ss.clear();
		getline(ifs, line);
		ss << line;
		int id;
		getline(ss, token, ' ');
		id = atoi(token.c_str());
		DynInstance* instance = new DynInstance();
		//ifs >> storeItem;
		instance->deserialize(&ss);
		//DynInstance* item = new DynInstance(*storeItem);
		createInstanceById(id, instance);

		ParserInfo info;
		info.name = instance->getZCPar_SymbolName();
		info.bitfield = instance->getParserSymbolBitfield();
		info.newInstanceId = id;
		info.oldInstanceId = instance->getParentInstanceId();
		info.container = instance;
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
	transform(symbolName.begin(), symbolName.end(), symbolName.begin(), ::toupper);
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
/*	if (IsModified(item))
	{
		//DynInstance* oldStoreItem = getInstanceItem(parentId);
		//instanceItem->copyUserData(*oldStoreItem);
		if (instanceBegin > parentId)
		{
			instanceBegin = parentId;
		}
	}*/

	instanceItem->store(*item);
	instanceItem->setInstanceID(instanceId);

	//this instance id is now used again!
	instanceItem->notUsed = false;

	setParentInstanceId(instanceId, parentId);
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

	if (item->isReusable())
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

zCPar_Symbol* ObjectManager::createNewSymbol(int instanceId, zCPar_Symbol* old) const
{

	zCPar_Symbol* symbol;
	zCParser* parser = zCParser::GetParser();
	//zCPar_Symbol* ref = parser->GetSymbol(parser->GetIndex("ItMw_1h_vlk_dagger"));

	symbol = parser->GetSymbol(instanceId);

	// Symbol already exists?
	if (symbol) return symbol;

	symbol = new zCPar_Symbol();
	ZeroMemory(symbol, sizeof(zCPar_Symbol));

	stringstream ss; ss << "DII_" << instanceId;
	string name = ss.str();
	transform(name.begin(), name.end(), name.begin(), ::toupper);
	symbol->name = zSTRING(name.c_str());
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
		ss << gothicDirPath.ToChar() << saveGamesDirName.ToChar() << "quicksave\\";
	} else
	{
		ss << gothicDirPath.ToChar() << saveGamesDirName.ToChar() << "savegame" << saveGameSlotNumber << "\\";
	}
	return ss.str();
};

std::string ObjectManager::getCurrentDirectoryPath() {
	zCOption* option = zCOption::GetOption();
	zSTRING& gothicDirPath = option->GetDirString(static_cast<zTOptionPaths>(24));
	zSTRING& saveGamesDirName = option->GetDirString(static_cast<zTOptionPaths>(2));

	std::stringstream ss;
	//write full path to the savegame
	ss << gothicDirPath.ToChar() << saveGamesDirName.ToChar() << "current\\";
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
	logStream << "ObjectManager::createAdditionalMemory: created addit memory: "<< item->name.ToChar() << endl;
	logStream << "ObjectManager::createAdditionalMemory: addit-key: " << additKey << endl;
	logStream << "ObjectManager::createAdditionalMemory: object ptr: " << (int)item << endl;
	util::debug(&logStream);
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
	logStream << "ObjectManager::removeAdditionalMemory: addit-key removed: " << additId << endl;
	util::debug(&logStream);
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

int ObjectManager::calcAdditKey(bool isHeroItem) const
{
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

	std::stringstream ss;
	std::string line;
	// Load additional memory informations
	int size;
	getline(ifs, line);
	size = atoi(line.c_str());
	for (size_t i = 0; i != size; ++i) {
		AdditMemory* addit = new AdditMemory();
		//ia >> addit;
		ss.str();
		ss.clear();
		getline(ifs, line);
		ss << line;
		addit->deserialize(&ss);
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

	std::stringstream ss;
	std::string line;
	// Load additional memory informations
	int size;
	getline(ifs, line);
	size = atoi(line.c_str());
	for (size_t i = 0; i != size; ++i) {
		AdditMemory* addit = new AdditMemory();
		//ia >> addit;
		ss.str();
		ss.clear();
		getline(ifs, line);
		ss << line;
		addit->deserialize(&ss);
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

	string fullpath = dir + string(filename);
	ofstream ofs(const_cast<char*>(fullpath.c_str()));

	// Save additional memory
	int size = heroItemList.size();

	logStream << "ObjectManager::saveHeroData: size of additional memory : " << size << endl;
	util::logInfo(&logStream);

	list<AdditMemory*>::iterator additIt;
	ofs << size << '\n';
	for (additIt = heroItemList.begin(); additIt != heroItemList.end(); ++additIt) {
		AdditMemory* addit = *additIt;
		//ofs << addit;
		addit->serialize(ofs);
		ofs << '\n';
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

	string fullpath = dir + string(filename);
	ofstream ofs(const_cast<char*>(fullpath.c_str()));

	// Save additional memory
	int size = keyToAdditMap.size() - heroItemSize;
	//writeToConsole("size of additional memory: ", size);
	map<int,AdditMemory*>::iterator additIt;
	ofs << size << '\n';
	for (additIt = keyToAdditMap.begin(); additIt != keyToAdditMap.end(); ++additIt) {
		AdditMemory* addit = additIt->second;
		if (addit->additId < HERO_ADDIT_BEGIN) {
			//oa << addit;
			addit->serialize(ofs);
			ofs << '\n';
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


void ObjectManager::callForInventoryItems(function<void(oCItem*)> func, oCNpc* npc)
{
	list<oCItem*> tempList;
	oCNpcInventory* inventory = npc->GetInventory();
	if (inventory == nullptr) return;

	inventory->UnpackAllItems();
	zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
	while (list != nullptr) {
		oCItem* item = list->GetData();
		if (item != nullptr) tempList.push_back(item);

		list = list->GetNext();
	}

	for (auto it = tempList.begin(); it != tempList.end(); ++it)
	{
		func(*it);
	}
	tempList.clear();
}

void ObjectManager::callForAllWorldItems(function<void(oCItem*)> func)
{
	zCWorld* world = oCGame::GetGame()->GetWorld();
	list<oCItem*> tempList;
	zCListSort<oCItem>* itemList = world->GetItemList();
	while (itemList != nullptr) {
		oCItem* item = itemList->GetData();
		if (item != nullptr)
		{
			tempList.push_back(item);
		}
		itemList = itemList->GetNext();
	}

	for (auto it = tempList.begin(); it != tempList.end(); ++it)
	{
		func(*it);
	}
}

int ObjectManager::getInstanceBegin() const
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

	instance->setPreviousId(previousId);
	instance->setReusable(true);
	instance->checkNotUsed();

	if (instance->isReusable())
	{
		reusableInstances.push_back(instanceId);
	}
}

void ObjectManager::checkReusableInstances()
{
	for (auto it = instanceMap.begin(); it != instanceMap.end(); ++it)
	{
		DynInstance* instance = it->second;
		instance->checkNotUsed();

		if (instance->isReusable())
		{
			reusableInstances.push_back(instance->getInstanceID());
		}
	}
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
}
int * ObjectManager::getRefCounter(oCItem * item)
{
	return (int*)((BYTE*)item + 0x4);
}

bool ObjectManager::isRangedWeapon(oCItem* item)
{
	//const int ITEM_KAT_FF = 4;
	if (item->mainflags == 4)
	{
		return true;
	}
	return false;
}

void ObjectManager::equipRangedWeapon(oCItem* item, oCNpcInventory* inventory, bool munitionUsesRightHand)
{
	zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, item->munition);
	oCItem* munition = list->GetData();
	int arrowId = ObjectManager::getObjectManager()->getInstanceId(*munition);
	zCListSort<oCItem>* list2 = list->GetNext();
	oCItem* munition2 = nullptr;
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
	}
	else
	{
		oCNpcSetLeftHand(owner, munition->SplitItem(1));
	}
}