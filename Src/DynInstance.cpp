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

#include "DynInstance.h"
#include "api/g2/zcparser.h"
#include <sstream>
#include <Util.h>
#include <ObjectManager.h>
#include <api/g2/ocgame.h>
#include <api/g2/zcworld.h>

using namespace std;
std::stringstream DynInstance::logStream;

DynInstance::DynInstance()
{
	previousId = -1;
	reusable = false;
}

DynInstance::DynInstance(oCItem& item)
{
	store(item);
	previousId = -1;
	reusable = false;
}

DynInstance::~DynInstance()
{
}

struct RESTORE_PREVIOUS_ID_PARAMS {
	std::stringstream* logStream;
	int instanceID;
	int previousID;
};

static void restorePreviousId(void* obj, void* param, oCItem* itm) {

	RESTORE_PREVIOUS_ID_PARAMS* params = (RESTORE_PREVIOUS_ID_PARAMS*)param;
	int instanceID = params->instanceID;


	if (itm && (itm->GetInstance() == instanceID))
	{
		*params->logStream << "item has instanceId marked as reusable: " << itm->description.ToChar() << endl;
		util::debug(params->logStream);

		ObjectManager* manager = ObjectManager::getObjectManager();
		manager->setInstanceId(itm, params->previousID);
		manager->oCItemSaveRemoveEffect(itm);
		itm->effect = "";
		itm->InitByScript(params->previousID, 1);
		itm->InsertEffect();
	}
}

void DynInstance::checkNotUsed()
{
	if (reusable) return;

	if (previousId < 0) return;

	bool notUsedInOtherWorlds = false;
	if (activeWorlds.size() == 0)
	{
		notUsedInOtherWorlds = true;
	}

	if (activeWorlds.size() == 1)
	{
		string worldName = oCGame::GetGame()->GetWorld()->worldName.ToChar();
		if (activeWorlds.front().compare(worldName) == 0)
		{
			notUsedInOtherWorlds = true;
		}
	}

	// Assign all connected items in the world the previous instance id
	ObjectManager* manager = ObjectManager::getObjectManager();
	/*auto func = [&](oCItem* item)->void {
		if (item && (item->GetInstance() == instanceID))
		{
			logStream << "item has instanceId marked as reusable: " << item->description.ToChar() << endl;
			util::debug(&logStream);

			manager->setInstanceId(item, previousId);
			manager->oCItemSaveRemoveEffect(item);
			item->effect = "";
			item->InitByScript(previousId, 1);
			item->InsertEffect();
		}
	};*/

	RESTORE_PREVIOUS_ID_PARAMS params = {&logStream, instanceID, previousId };
	manager->callForAllItems(restorePreviousId, NULL, &params);

	if (notUsedInOtherWorlds)
	{
		previousId = -1;
	}
}

void DynInstance::setPreviousId(int previousId)
{
	this->previousId = previousId;
}

bool DynInstance::isReusable()
{
	return reusable;
}

void DynInstance::setReusable(bool reuse)
{
	reusable = reuse;
}

void DynInstance::store(oCItem& item) {
	idx=item.idx;
	name=item.name.ToChar();
	nameID=item.nameID.ToChar();
	hp=item.hp;
	hp_max=item.hp_max;
	mainflags=item.mainflags;
	flags=item.flags;
	weight=item.weight;
	value=item.value;

	// -- weapons		
	damageType=item.damageType;
	damageTotal=item.damageTotal;
	damage[0]=item.damage[0];
	damage[1]=item.damage[1];
	damage[2]=item.damage[2];
	damage[3]=item.damage[3];
	damage[4]=item.damage[4];
	damage[5]=item.damage[5];
	damage[6]=item.damage[6];
	damage[7]=item.damage[7];

	// -- armor 
	wear = item.wear;
	protection[0]=item.protection[0];
	protection[1]=item.protection[1];
	protection[2]=item.protection[2];
	protection[3]=item.protection[3];
	protection[4]=item.protection[4];
	protection[5]=item.protection[5];
	protection[6]=item.protection[6];
	protection[7]=item.protection[7];

	// -- food
	nutrition=item.nutrition;

	// -- use attributes
	cond_atr[0]=item.cond_atr[0];
	cond_atr[1]=item.cond_atr[1];
	cond_atr[2]=item.cond_atr[2];

	cond_value[0]=item.cond_value[0];
	cond_value[1]=item.cond_value[1];
	cond_value[2]=item.cond_value[2];

	// -- attributes that will be changed on equip
	change_atr[0]=item.change_atr[0];
	change_atr[1]=item.change_atr[1];
	change_atr[2]=item.change_atr[2];

	change_value[0]=item.change_value[0];
	change_value[1]=item.change_value[1];
	change_value[2]=item.change_value[2];

	// -- parser functions
	magic=item.magic;
	on_equip=item.on_equip;
	on_unequip=item.on_unequip;	
	on_state[0]= item.on_state[0];
	on_state[1]= item.on_state[1];
	on_state[2]= item.on_state[2];
	on_state[3]= item.on_state[3];

	// -- owner									
	owner= item.owner;			//	owner: npc instance
	ownerGuild= item.ownerGuild;		//	owner: guild
	disguiseGuild= item.disguiseGuild;

	// -- visual
	visual= item.visual.ToChar();

	// -- change of mesh on equip
	visual_change= item.visual_change.ToChar();	//	ASC file
	effect= item.effect.ToChar();			//  Effect instance

	visual_skin= item.visual_skin;

	scemeName= item.scemeName.ToChar();
	material= item.material;	
	munition= item.munition;		//	Instance of ammunition

	spell= item.spell;			
	range= item.range;			

	mag_circle= item.mag_circle;

	description= item.description.ToChar();
	text[0]= item.text[0].ToChar();
	text[1]= item.text[1].ToChar();
	text[2]= item.text[2].ToChar();
	text[3]= item.text[3].ToChar();
	text[4]= item.text[4].ToChar();
	text[5]= item.text[5].ToChar();

	count[0]= item.count[0];
	count[1]= item.count[1];
	count[2]= item.count[2];
	count[3]= item.count[3];
	count[4]= item.count[4];
	count[5]= item.count[5];

	// -- inventory presentation
	inv_zbias= item.inv_zbias;								//  far plane (how far the item goes into the room by the z-axis)
	inv_rotx= item.inv_rotx;								//  rotation around x-axis (in degrees)
	inv_roty= item.inv_roty;								//  rotation around y-axis (in degrees)
	inv_rotz= item.inv_rotz;								//  rotation around z-axis (in degrees)
	inv_animate= item.inv_animate;							//  rotate the item
	instanz= item.instanz;						//int Symbolindex
	c_manipulation = item.c_manipulation;					//int ?
	last_manipulation= item.last_manipulation;				//zREAL ?
	magic_value = item.magic_value;					//int ?
	effectVob= item.effectVob;						//oCVisualFX*
	next = item.next;	
	int address = reinterpret_cast<int>(&item);
	address += 0x330;
	int* instance = reinterpret_cast<int*>(address);
	instanceID = *instance;
}




void DynInstance::init(oCItem* item, int index) {

	ObjectManager*  manager = ObjectManager::getObjectManager();
	manager->oCItemSaveRemoveEffect(item);

	item->idx=idx;
	item->name=zSTRING(name.c_str());
	item->nameID=zSTRING(nameID.c_str());
	item->hp=hp;
	item->hp_max=hp_max;
	item->mainflags=mainflags;
	item->flags=flags;
	item->weight=weight;
	item->value=value;

	// -- weapons		
	item->damageType=damageType;
	item->damageTotal=damageTotal;

	item->damage[0]=damage[0];
	item->damage[1]=damage[1];
	item->damage[2]=damage[2];
	item->damage[3]=damage[3];
	item->damage[4]=damage[4];
	item->damage[5]=damage[5];
	item->damage[6]=damage[6];
	item->damage[7]=damage[7];

	// -- armor 
	item->wear = wear;
	item->protection[0]=protection[0];
	item->protection[1]=protection[1];
	item->protection[2]=protection[2];
	item->protection[3]=protection[3];
	item->protection[4]=protection[4];
	item->protection[5]=protection[5];
	item->protection[6]=protection[6];
	item->protection[7]=protection[7];

	// -- food
	item->nutrition=nutrition;

	// -- use attributes
	item->cond_atr[0]=cond_atr[0];
	item->cond_atr[1]=cond_atr[1];
	item->cond_atr[2]=cond_atr[2];

	item->cond_value[0]=cond_value[0];
	item->cond_value[1]=cond_value[1];
	item->cond_value[2]=cond_value[2];

	// -- attributes that will be changed on equip
	item->change_atr[0]=change_atr[0];
	item->change_atr[1]=change_atr[1];
	item->change_atr[2]=change_atr[2];

	item->change_value[0]=change_value[0];
	item->change_value[1]=change_value[1];
	item->change_value[2]=change_value[2];

	// -- parser functions
	item->magic=magic;
	item->on_equip=on_equip;
	item->on_unequip=on_unequip;

	item->on_state[0]=on_state[0];
	item->on_state[1]=on_state[1];
	item->on_state[2]=on_state[2];
	item->on_state[3]=on_state[3];

	// -- owner									
	item->owner=owner;			//	owner: npc instance
	item->ownerGuild=ownerGuild;		//	owner: guild
	item->disguiseGuild=disguiseGuild;

	// -- visual
	item->visual=zSTRING(visual.c_str());

	// -- change of mesh on equip
	item->visual_change=zSTRING(visual_change.c_str());	//	ASC file
	item->effect=zSTRING(effect.c_str());			//  Effect instance

	item->visual_skin=visual_skin;

	item->scemeName=zSTRING(scemeName.c_str());
	item->material=material;	
	item->munition=munition;		//	Instance of ammunition

	item->spell=spell;			
	item->range=range;			

	item->mag_circle=mag_circle;

	item->description=zSTRING(description.c_str());
	item->text[0]=zSTRING(text[0].c_str());
	item->text[1]=zSTRING(text[1].c_str());
	item->text[2]=zSTRING(text[2].c_str());
	item->text[3]=zSTRING(text[3].c_str());
	item->text[4]=zSTRING(text[4].c_str());
	item->text[5]=zSTRING(text[5].c_str());

	item->count[0]=count[0];
	item->count[1]=count[1];
	item->count[2]=count[2];
	item->count[3]=count[3];
	item->count[4]=count[4];
	item->count[5]=count[5];

	// -- inventory presentation
	item->inv_zbias=inv_zbias;								//  far plane (how far the item goes into the room by the z-axis)
	item->inv_rotx=inv_rotx;								//  rotation around x-axis (in degrees)
	item->inv_roty=inv_roty;								//  rotation around y-axis (in degrees)
	item->inv_rotz=inv_rotz;								//  rotation around z-axis (in degrees)
	item->inv_animate=inv_animate;							//  rotate the item
	// Skip instanz field!!!
	//item->instanz=instanz;						//int Symbolindex
	item->c_manipulation=c_manipulation;					//int ?
	item->last_manipulation=last_manipulation;				//zREAL ?
	item->magic_value=magic_value;					//int ?
	item->effectVob=effectVob;						//oCVisualFX*
	item->next = next;	
	int address = reinterpret_cast<int>(item);
	address += 0x330;
	int* instance = reinterpret_cast<int*>(address);

	//Get current symbol index and set it as the item's instance id
	*instance = index;

	manager->oCItemSaveInsertEffect(item);
};


int DynInstance::getParentInstanceId() {
	return parentInstanceId;	
};

void DynInstance::setParentInstanceId(int parentInstanceId){
	this->parentInstanceId = parentInstanceId;
}


std::string DynInstance::getZCPar_SymbolName()
{
	return zCPar_Symbol_name;
}


void DynInstance::setZCPar_SymbolName(std::string name)
{
	zCPar_Symbol_name = name;
}

void DynInstance::serialize(std::ostream& os) const
{
	os << reusable << ' ';
	util::writeString(os, zCPar_Symbol_name);
	os << ' ';
	os << zCPar_Symbol_Bitfield << ' ';
	os << parentInstanceId << ' ';
	os << instanceID << ' ';
	os << idx << ' ';
	util::writeString(os, name);
	os << ' ';
	util::writeString(os, nameID);
	os << ' ';
	os << hp << ' ';
	os << hp_max << ' ';
	os << mainflags << ' ';
	os << flags << ' ';
	os << weight << ' ';
	os << value << ' ';

	// -- weapons		
	os << damageType << ' ';
	os << damageTotal << ' ';

	for (int i = 0; i < 8; ++i)
	{
		os << damage[i] << ' ';
	}

	// -- armor 
	os << wear << ' ';
	
	for (int i = 0; i < 8; ++i)
	{
		os << protection[i] << ' ';
	}

	// -- food
	os << nutrition << ' ';

	// -- use attributes
	for (int i = 0; i < 3; ++i)
	{
		os << cond_atr[i] << ' ';
	}

	for (int i = 0; i < 3; ++i)
	{
		os << cond_value[i] << ' ';
	}

	// -- attributes that will be changed on equip
	for (int i = 0; i < 3; ++i)
	{
		os << change_atr[i] << ' ';
	}

	for (int i = 0; i < 3; ++i)
	{
		os << change_value[i] << ' ';
	}

	// -- parser functions
	os << magic << ' ';
	os << on_equip << ' ';
	os << on_unequip << ' ';

	for (int i = 0; i < 4; ++i)
	{
		os << on_state[i] << ' ';
	}

	// -- owner									
	os << owner << ' ';			//	owner: npc instance
	os << ownerGuild << ' ';		//	owner: guild
	os << disguiseGuild << ' ';

	// -- visual
	util::writeString(os, visual);
	os << ' ';

	// -- change of mesh on equip
	util::writeString(os, visual_change);
	os << ' ';
	util::writeString(os, effect);
	os << ' ';

	os << visual_skin << ' ';

	util::writeString(os, scemeName);
	os << ' ';
	os << material << ' ';
	os << munition << ' ';		//	Instance of ammunition

	os << spell << ' ';
	os << range << ' ';

	os << mag_circle << ' ';

	util::writeString(os, description);
	os << ' ';

	for (int i = 0; i < 6; ++i)
	{
		util::writeString(os, text[i]);
		os << ' ';
	}

	for (int i = 0; i < 6; ++i)
	{
		os << count[i] << ' ';
	}

	// -- inventory presentation
	os << inv_zbias << ' ';								//  far plane (how far the item goes into the room by the z-axis)
	os << inv_rotx << ' ';								//  rotation around x-axis (in degrees)
	os << inv_roty << ' ';								//  rotation around y-axis (in degrees)
	os << inv_rotz << ' ';								//  rotation around z-axis (in degrees)
	os << inv_animate << ' ';							//  rotate the item
	os << instanz << ' ';						//    ar & Symbolindex
	os << c_manipulation << ' ';					//int ?
	os << last_manipulation << ' ';				//zREAL ?
	os << magic_value << ' ';					//int ?
	os << effectVob << ' ';						//oCVisualFX*
	os << next << ' ';

	dii_userData.serialize(os);

	os << activeWorlds.size() << ' ';

	for (list<string>::const_iterator it = activeWorlds.begin(); it != activeWorlds.end(); ++it)
	{
		util::writeString(os, *it);
		os << ' ';
	}

	os << previousId << ' ';
}


void DynInstance::deserialize(std::stringstream* is)
{
	/*
		os << notUsed << ' ';
	writeString(os, zCPar_Symbol_name);
	os << ' ';
	os << zCPar_Symbol_Bitfield << ' ';
	os << parentInstanceId << ' ';
	os << instanceID << ' ';
	os << idx << ' ';
	writeString(os, name);
	os << ' ';
	writeString(os, nameID);
	
	*/
	util::getBool(*is, reusable);
	util::readString(is, zCPar_Symbol_name);
	zCPar_Symbol_name = util::trimFromRight(zCPar_Symbol_name);
	util::getInt(*is, zCPar_Symbol_Bitfield);
	util::getInt(*is, parentInstanceId);
	util::getInt(*is, instanceID);
	util::getInt(*is, idx);
	util::readString(is, name);
	util::readString(is, nameID);
	util::getInt(*is, hp);
	util::getInt(*is, hp_max);
	util::getInt(*is, mainflags);
	util::getInt(*is, flags);
	util::getInt(*is, weight);
	util::getInt(*is, value);

	// -- weapons		
	util::getInt(*is, damageType);
	util::getInt(*is, damageTotal);

	for (int i = 0; i < 8; ++i)
	{
		util::getInt(*is, damage[i]);
	}

	// -- armor 
	*is >> wear;

	for (int i = 0; i < 8; ++i)
	{
		util::getInt(*is, protection[i]);
	}

	// -- food
	util::getInt(*is, nutrition);

	// -- use attributes
	for (int i = 0; i < 3; ++i)
	{
		util::getInt(*is, cond_atr[i]);
	}

	for (int i = 0; i < 3; ++i)
	{
		util::getInt(*is, cond_value[i]);
	}

	// -- attributes that will be changed on equip
	for (int i = 0; i < 3; ++i)
	{
		util::getInt(*is, change_atr[i]);
	}

	for (int i = 0; i < 3; ++i)
	{
		util::getInt(*is, change_value[i]);
	}

	// -- parser functions
	util::getInt(*is, magic);
	util::getInt(*is, on_equip);
	util::getInt(*is, on_unequip);

	for (int i = 0; i < 4; ++i)
	{
		util::getInt(*is, on_state[i]);
	}

	// -- owner						
	util::getInt(*is, owner);
	util::getInt(*is, ownerGuild);
	util::getInt(*is, disguiseGuild);

	// -- visual
	util::readString(is, visual);

	// -- change of mesh on equip
	util::readString(is, visual_change);
	util::readString(is, effect);

	util::getInt(*is, visual_skin);

	util::readString(is, scemeName);
	util::getInt(*is, material);
	util::getInt(*is, munition);

	util::getInt(*is, spell);
	util::getInt(*is, range);

	util::getInt(*is, mag_circle);

	util::readString(is, description);

	for (int i = 0; i < 6; ++i)
	{
		util::readString(is, text[i]);
	}

	for (int i = 0; i < 6; ++i)
	{
		util::getInt(*is, count[i]);
	}

	// -- inventory presentation
	util::getInt(*is, inv_zbias);
	util::getInt(*is, inv_rotx);
	util::getInt(*is, inv_roty);
	util::getInt(*is, inv_rotz);
	util::getInt(*is, inv_animate);
	util::getInt(*is, instanz);
	util::getInt(*is, c_manipulation);
	util::getInt(*is, last_manipulation);
	util::getInt(*is, magic_value);
	util::getInt(*is, effectVob);
	util::getInt(*is, next);

	dii_userData.deserialize(is);

	int activeWorldSize = 0;
	util::getInt(*is, activeWorldSize);
	for (int i = 0; i < activeWorldSize; ++i)
	{
		string data = "";
		util::readString(is, data);

		//avoid duplicate names!
		addActiveWorld(data);
	}

	util::getInt(*is, previousId);
}

int DynInstance::getParserSymbolBitfield()
{
	return zCPar_Symbol_Bitfield;
}

void DynInstance::setParserSymbolBitfield(int bitfield)
{
	zCPar_Symbol_Bitfield = bitfield;
}

bool outOfRange(int index)
{
	if (index < 0 || index > DII_UserData::MAX_USER_DATA) return true;
	return false;
};

/*void DynInstance::setUserData(int index, int data)
{
	dii_userData.userData.pData->data[index] = data;
}

int DynInstance::getUserData(int index)
{
	return dii_userData.userData.pData->data[index];
}*/

BYTE* DynInstance::getUserData()
{
	return dii_userData.userData.pMemory;
}

void DynInstance::copyUserData(DynInstance& source)
{
	/*for (int i = 0; i < dii_userData.userData.intAmount; ++i)
	{
		dii_userData.userData.getIntBegin()[i] = source.dii_userData.userData.getIntBegin()[i];

		//TODO strings!!!
	}*/
}

void DynInstance::addActiveWorld(string worldName)
{
	for (std::list<std::string>::iterator it = activeWorlds.begin(); it != activeWorlds.end(); ++it)
	{
		string active((*it).c_str());
		if (worldName.compare(active) == 0)
		{
			// worldName was already added. Nothing to do
			return;
		}
	}
	activeWorlds.push_back(worldName);
}

void DynInstance::resetActiveWorlds()
{
	zCWorld* world = oCGame::GetGame()->GetWorld();
	string worldName (world->worldName.ToChar());

	//remove current world of list
	activeWorlds.remove(worldName);
}


DII_UserData::DII_UserData()
{
	//get intAmount and strAmount from the parser
	zCParser* parser; parser = zCParser::GetParser();
	int intAmountIndex = parser->GetIndex("DII_USER_DATA_INTEGER_AMOUNT");
	int strAmountIndex = parser->GetIndex("DII_USER_DATA_STRING_AMOUNT");
	zCPar_Symbol* intAmountSymbol = parser->GetSymbol(intAmountIndex);
	zCPar_Symbol* strAmountSymbol = parser->GetSymbol(strAmountIndex);
	
	createMemory(intAmountSymbol->content.data_int, strAmountSymbol->content.data_int);
	//ZeroMemory(userData.pData, sizeof(Data));
	//ZeroMemory(userData.pData->data, sizeof(int) * MAX_USER_DATA);
}

/*DII_UserData::DII_UserData(zCParser* parser, int instance)
{
	ZeroMemory(userData.pData->data, sizeof(int) * MAX_USER_DATA);
	for (int i = 0; i < MAX_STRING_DATA; ++i)
	{
		//zSTRINGSerialized str = userData.stringData[i];
		//if (str.len != 0)
		//{
		//	delete str.ptr;
		//	str.len = 0;
		//}
	}
	parser->CreateInstance(instance, &userData);
}*/

DII_UserData::~DII_UserData()
{

	std::stringstream ss; ss <<"Called ~DII_UserData()" << std::endl;
	util::debug(&ss);
	//release allocated memory
	int indexBegin = userData.intAmount * sizeof(int);
	zSTRINGSerialized* ptr = (zSTRINGSerialized*)((userData.pMemory) + indexBegin);

	//delete ptr->ptr;
	//ptr->ptr = NULL;
	//delete userData.pMemory;
	//userData.pMemory = NULL;
}

void DII_UserData::serialize(std::ostream& os) const
{
	os << userData.intAmount << ' ' << userData.strAmount << ' ';

	//save int array
	for (int i = 0; i < userData.intAmount; ++i)
	{
		os << userData.getIntBegin()[i] << ' ';
	}

	stringstream ss;
	ss << "strAmount: " << userData.strAmount << std::endl;
	util::debug(&ss);

	//save bytestream
	int indexBegin = userData.intAmount * sizeof(int);

	//save the first string
	zSTRINGSerialized* ptr;

	for (int i = 0; i < userData.strAmount; ++i)
	{
		ptr = (zSTRINGSerialized*)((userData.pMemory) + indexBegin + i*sizeof(zSTRINGSerialized));
		//os << ptr->_vtbl << ' ';
		//os << ptr->_allocater << ' ';

		string data;
		if (ptr->ptr == NULL)
		{
			data = "";
		}
		else
		{
			data = string(ptr->ptr);
		}
		ss << "string to write: " << data << std::endl;
		util::debug(&ss);

		ss << ": ptr->_vtbl: " << ptr->_vtbl << std::endl;
		util::debug(&ss);

		ss << "ptr->_allocater " << ptr->_allocater << std::endl;
		util::debug(&ss);

		if (ptr->ptr)
		{
			ss << "ptr->ptr: " << ptr->ptr << std::endl;
			util::debug(&ss);
		}
		else
		{
			ss << "ptr->ptr: null" << endl;
			util::debug(&ss);
		}

		ss << "ptr->len: " << ptr->len << endl;
		util::debug(&ss);

		ss << "ptr->res: " << ptr->res << endl;
		util::debug(&ss);

		util::writeString(os, data);
		os << ' ';
	}

	//os << ptr->res;
}

void DII_UserData::deserialize(std::stringstream* is)
{
	util::getInt(*is, userData.intAmount);
	util::getInt(*is, userData.strAmount);

	//init int array
	for (int i = 0; i < userData.intAmount; ++i)
	{
		util::getInt(*is, userData.getIntBegin()[i]);
	}

	// init string array
	zSTRINGSerialized* ptr;
	int indexBegin = userData.intAmount * sizeof(int);
	std::stringstream ss;

	for (int i = 0; i < userData.strAmount; ++i)
	{
		ptr = (zSTRINGSerialized*)((userData.pMemory) + indexBegin + i*sizeof(zSTRINGSerialized));
		//util::getInt(*is, ptr->_vtbl);
		//util::getInt(*is, ptr->_allocater);

		//assert(userData.getStr(i)->ptr == NULL);
		std::string data;
		util::readString(is, data);


		const char* content = data.c_str();
		if (content == NULL) {
			content = "";
		}

		zSTRING dataZ(content);

		memcpy(ptr, &dataZ, sizeof(zSTRING));
		//&userData.pMemory[indexBegin] = new char[userData.getStr(i)->len];
		int size = ptr->len;
		int res = ptr->res;

		zSTRING* test = (zSTRING*)ptr;

		ss << "string loaded: " << std::string(test->ToChar()) << std::endl;
		util::debug(&ss);

	}
}

void * DII_UserData::gothic2OperatorNew(size_t size)
{
	XCALL(0x00565F50);
}

void DII_UserData::createMemory(int intAmount, int strAmount)
{
	std::stringstream ss; ss << "DII_UserData::createMemory: strAmount: " << strAmount << std::endl;
	util::debug(&ss);
	int byteSize = intAmount * sizeof(int) + strAmount * sizeof(zSTRINGSerialized);
	BYTE* memory = (BYTE*)gothic2OperatorNew(byteSize);
	memset(memory, 0, byteSize);

	userData.intAmount = intAmount;
	userData.strAmount = strAmount;
	userData.pMemory = memory;
}