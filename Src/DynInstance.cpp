/*////////////////////////////////////////////////////////////////////////////

This file is part of neclib.

Copyright © 2015-2020 David Goeth

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

/////////////////////////////////////////////////////////////////////////////**/

#include "DynInstance.h"
#include "api/g2/zcparser.h"
#include <sstream>
#include <Util.h>
#include <ObjectManager.h>
#include <ocgameExtended.h>
#include <api/g2/zcworld.h>

using namespace std;
std::stringstream DynInstance::mLogStream;

DynInstance::DynInstance()
{
}

DynInstance::DynInstance(oCItem& item)
{
	store(item);
}

DynInstance::~DynInstance()
{
}

struct RESTORE_PREVIOUS_ID_PARAMS {
	std::stringstream* mLogStream;
	int instanceID;
	int previousID;
};

static void restorePreviousId(void* obj, void* param, oCItem* itm) {

	RESTORE_PREVIOUS_ID_PARAMS* params = (RESTORE_PREVIOUS_ID_PARAMS*)param;
	int instanceID = params->instanceID;


	if (itm && (itm->GetInstance() == instanceID))
	{
		*params->mLogStream << "item has instanceId marked as reusable: " << itm->description.ToChar() << endl;
		util::debug(*params->mLogStream);

		ObjectManager* manager = ObjectManager::getObjectManager();
		manager->setInstanceId(itm, params->previousID);
		manager->oCItemSaveRemoveEffect(itm);
		itm->effect = "";
		itm->InitByScript(params->previousID, 1);
		itm->InsertEffect();
	}
}


void DynInstance::store(oCItem& item) {

	auto* parser = zCParser::GetParser();

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
	magic = util::getSymbolName(item.magic).ToChar();
	on_equip= util::getSymbolName(item.on_equip).ToChar();
	on_unequip= util::getSymbolName(item.on_unequip).ToChar();

	for (int i = 0; i < 4; ++i) {
		on_state[i] = util::getSymbolName(item.on_state[i]).ToChar();
	}

	// -- owner									
	owner= util::getSymbolName(item.owner).ToChar();			//	owner: npc instance
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
	munition= util::getSymbolName(item.munition).ToChar();		//	Instance of ammunition

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
	amount = item.amount;
	//instanz= item.instanz;						//int Symbolindex
	c_manipulation = item.c_manipulation;					//int ?
	last_manipulation= item.last_manipulation;				//zREAL ?
	magic_value = item.magic_value;					//int ?

	//next = item.next;	
	//int address = reinterpret_cast<int>(&item);
	//address += 0x330;
	//int* instance = reinterpret_cast<int*>(address);
	//instanceID = *instance;
}




void DynInstance::init(oCItem* item, int instanceParserSymbolID) {

	ObjectManager*  manager = ObjectManager::getObjectManager();
	//manager->oCItemSaveRemoveEffect(item);

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
	item->magic= util::getIndexDefaultZero(magic.c_str());
	item->on_equip= util::getIndexDefaultZero(on_equip.c_str());;
	item->on_unequip= util::getIndexDefaultZero(on_unequip.c_str());

	for (int i = 0; i < 4; ++i) {
		item->on_state[i] = util::getIndexDefaultZero(on_state[i].c_str());
	}

	// -- owner									
	item->owner= util::getIndexDefaultZero(owner.c_str());			//	owner: npc instance
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
	item->munition= util::getIndexDefaultZero(munition.c_str());		//	Instance of ammunition

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
	
	//item->effectVob = 0;

	//manager->oCItemSaveRemoveEffect(item);
	//manager->oCItemSaveInsertEffect(item);
	
	//item->effectVob=effectVob;						//oCVisualFX*
	//item->next = 0;	
	int address = reinterpret_cast<int>(item);
	address += 0x330;
	int* instance = reinterpret_cast<int*>(address);

	//Get current symbol index and set it as the item's instance id
	*instance = instanceParserSymbolID;
};


const std::string& DynInstance::getPrototypeSymbolName() {
	return mPrototypeSymbolName;	
};

void DynInstance::setPrototypeSymbolName(const std::string& symbolName){
	mPrototypeSymbolName = symbolName;
}


const std::string& DynInstance::getSymbolName()
{
	return mSymbolName;
}


void DynInstance::setSymbolName(const std::string& symbolName)
{
	mSymbolName = symbolName;
}

void DynInstance::serialize(std::ostream& os) const
{
	util::writeString(os, mSymbolName);	
	util::writeString(os, mPrototypeSymbolName);

	util::writeValue(os, zCPar_Symbol_Bitfield);
	util::writeValue(os, idx);

	util::writeString(os, name);
	util::writeString(os, nameID);

	util::writeValue(os, hp);
	util::writeValue(os, hp_max);
	util::writeValue(os, mainflags);
	util::writeValue(os, flags);
	util::writeValue(os, weight);
	util::writeValue(os, value);

	// -- weapons
	util::writeValue(os, damageType);
	util::writeValue(os, damageTotal);

	for (int i = 0; i < 8; ++i)
	{
		util::writeValue(os, damage[i]);
	}

	// -- armor 
	util::writeValue(os, wear);
	
	for (int i = 0; i < 8; ++i)
	{
		util::writeValue(os, protection[i]);
	}

	// -- food
	util::writeValue(os, nutrition);

	// -- use attributes
	for (int i = 0; i < 3; ++i)
	{
		util::writeValue(os, cond_atr[i]);
	}

	for (int i = 0; i < 3; ++i)
	{
		util::writeValue(os, cond_value[i]);
	}

	// -- attributes that will be changed on equip
	for (int i = 0; i < 3; ++i)
	{
		util::writeValue(os, change_atr[i]);
	}

	for (int i = 0; i < 3; ++i)
	{
		util::writeValue(os, change_value[i]);
	}

	// -- parser functions
	util::writeString(os, magic);
	util::writeString(os, on_equip); //TODO use symbol name
	util::writeString(os, on_unequip); //TODO use symbol name

	for (int i = 0; i < 4; ++i)
	{
		util::writeString(os, on_state[i]);
	}

	// -- owner		
	util::writeString(os, owner); //	owner: npc instance
	util::writeValue(os, ownerGuild); //	owner: guild
	util::writeValue(os, disguiseGuild);

	// -- visual
	util::writeString(os, visual);

	// -- change of mesh on equip
	util::writeString(os, visual_change);
	util::writeString(os, effect);

	util::writeValue(os, visual_skin);

	util::writeString(os, scemeName);

	util::writeValue(os, material);
	util::writeString(os, munition); //	Instance of ammunition

	util::writeValue(os, spell);
	util::writeValue(os, range);

	util::writeValue(os, mag_circle);

	util::writeString(os, description);

	for (int i = 0; i < 6; ++i)
	{
		util::writeString(os, text[i]);
	}

	for (int i = 0; i < 6; ++i)
	{
		util::writeValue(os, count[i]);
	}

	// -- inventory presentation
	util::writeValue(os, inv_zbias); //  far plane (how far the item goes into the room by the z-axis)
	util::writeValue(os, inv_rotx);
	util::writeValue(os, inv_roty);
	util::writeValue(os, inv_rotz); // rotation around z - axis(in degrees)
	util::writeValue(os, inv_animate); //  rotate the item ?

	util::writeValue(os, amount);
	util::writeValue(os, c_manipulation); // int ?
	util::writeValue(os, last_manipulation); //zReal?
	util::writeValue(os, magic_value); // int ?

	dii_userData.serialize(os);
}


void DynInstance::deserialize(std::istream& is)
{
	//util::getBool(*is, reusable);
	util::readAndTrim(is, mSymbolName);
	util::readAndTrim(is, mPrototypeSymbolName);

	util::readValue(is, zCPar_Symbol_Bitfield);
	util::readValue(is, idx);

	util::readString(is, name);
	util::readString(is, nameID);

	util::readValue(is, hp);
	util::readValue(is, hp_max);
	util::readValue(is, mainflags);
	util::readValue(is, flags);
	util::readValue(is, weight);
	util::readValue(is, value);

	// -- weapons		
	util::readValue(is, damageType);
	util::readValue(is, damageTotal);

	for (int i = 0; i < 8; ++i)
	{
		util::readValue(is, damage[i]);
	}

	// -- armor 
	util::readValue(is, wear);

	for (int i = 0; i < 8; ++i)
	{
		util::readValue(is, protection[i]);
	}

	// -- food
	util::readValue(is, nutrition);

	// -- use attributes
	for (int i = 0; i < 3; ++i)
	{
		util::readValue(is, cond_atr[i]);
	}

	for (int i = 0; i < 3; ++i)
	{
		util::readValue(is, cond_value[i]);
	}

	// -- attributes that will be changed on equip
	for (int i = 0; i < 3; ++i)
	{
		util::readValue(is, change_atr[i]);
	}

	for (int i = 0; i < 3; ++i)
	{
		util::readValue(is, change_value[i]);
	}

	// -- parser functions
	util::readString(is, magic);
	util::readString(is, on_equip); // TODO symbol string!
	util::readString(is, on_unequip); // TODO symbol string!

	for (int i = 0; i < 4; ++i)
	{
		util::readString(is, on_state[i]);
	}

	// -- owner
	util::readString(is, owner); // TODO symbol string!
	util::readValue(is, ownerGuild);
	util::readValue(is, disguiseGuild);

	// -- visual
	util::readString(is, visual);

	// -- change of mesh on equip
	util::readString(is, visual_change);
	util::readString(is, effect);

	util::readValue(is, visual_skin);

	util::readString(is, scemeName);
	util::readValue(is, material);
	util::readString(is, munition); // TODO symbol string!

	util::readValue(is, spell);
	util::readValue(is, range);
	util::readValue(is, mag_circle);

	util::readString(is, description);

	for (int i = 0; i < 6; ++i)
	{
		util::readString(is, text[i]);
	}

	for (int i = 0; i < 6; ++i)
	{
		util::readValue(is, count[i]);
	}

	// -- inventory presentation
	util::readValue(is, inv_zbias);
	util::readValue(is, inv_rotx);
	util::readValue(is, inv_roty);
	util::readValue(is, inv_rotz);
	util::readValue(is, inv_animate);

	util::readValue(is, amount);
	util::readValue(is, c_manipulation);
	util::readValue(is, last_manipulation);
	util::readValue(is, magic_value);

	dii_userData.deserialize(is);
}

int DynInstance::getParserSymbolBitfield()
{
	return zCPar_Symbol_Bitfield;
}

void DynInstance::setParserSymbolBitfield(int bitfield)
{
	zCPar_Symbol_Bitfield = bitfield;
}

BYTE* DynInstance::getUserData()
{
	return dii_userData.userData.pMemory;
}

bool DynInstance::getDoNotStore() const
{
	return mDoNotStore;
}

void DynInstance::setDoNotStore(bool doNotStore)
{
	mDoNotStore = doNotStore;
}

//void DynInstance::copyUserData(DynInstance& source)
//{
	/*for (int i = 0; i < dii_userData.userData.intAmount; ++i)
	{
		dii_userData.userData.getIntBegin()[i] = source.dii_userData.userData.getIntBegin()[i];

		//TODO strings!!!
	}*/
//}


DII_UserData::DII_UserData()
{
	createMemory(getIntAmount(), getStringAmount());
}

DII_UserData::~DII_UserData()
{
	std::stringstream ss; ss <<"Called ~DII_UserData()" << std::endl;
	util::debug(ss);
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
	util::writeValue(os, userData.intAmount);
	util::writeValue(os, userData.strAmount);

	//save int array
	for (int i = 0; i < userData.intAmount; ++i)
	{
		util::writeValue(os, userData.getIntBegin()[i]);
	}

	//os << userData.strAmount;

	stringstream ss;
	ss << "strAmount: " << userData.strAmount << std::endl;
	util::debug(ss);

	//save bytestream
	int indexBegin = userData.intAmount * sizeof(int);

	//save the first string
	zSTRINGSerialized* ptr;

	for (int i = 0; i < userData.strAmount; ++i)
	{
		ptr = (zSTRINGSerialized*)((userData.pMemory) + indexBegin + i*sizeof(zSTRINGSerialized));
		string data;
		if (ptr->ptr == NULL)
		{
			data = "";
		}
		else
		{
			data = string(ptr->ptr);
		}

		util::writeString(os, data);
	}
}

void DII_UserData::deserialize(std::istream& is)
{
	util::readValue(is, userData.intAmount);
	util::readValue(is, userData.strAmount);
	assertEqualAmounts();

	//init int array
	for (int i = 0; i < userData.intAmount; ++i)
	{
		util::readValue(is, userData.getIntBegin()[i]);
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
		util::debug(ss);

	}
}

int DII_UserData::getIntAmount()
{
	auto* symbol = UTIL_GET_SYMBOL_WITH_CHECKS(INT_AMOUNT_DAEDALUS_VAR);
	return symbol->content.data_int;
}

int DII_UserData::getStringAmount()
{
	auto* symbol = UTIL_GET_SYMBOL_WITH_CHECKS(STR_AMOUNT_DAEDALUS_VAR);
	return symbol->content.data_int;
}

void DII_UserData::assertEqualAmounts()
{
	const auto expectedIntAmount = getIntAmount();
	const auto expectedStrAmount = getStringAmount();

	if (expectedIntAmount != userData.intAmount) {

		std::stringstream mLogStream;
		mLogStream << "DII_UserData::assertEqualAmounts: int amount (" << userData.intAmount <<
				") doesn't match defined int amount (" << expectedIntAmount << ")"<< endl;
		util::logFatal(mLogStream);
	}

	if (expectedStrAmount != userData.strAmount) {

		std::stringstream mLogStream;
		mLogStream << "DII_UserData::assertEqualAmounts: string amount (" << userData.strAmount <<
			") doesn't match defined string amount (" << expectedStrAmount << ")" << endl;
		util::logFatal(mLogStream);
	}
}

void * DII_UserData::gothic2OperatorNew(size_t size)
{
	XCALL(0x00565F50);
}

void DII_UserData::createMemory(int intAmount, int strAmount)
{
	std::stringstream ss; ss << "DII_UserData::createMemory: strAmount: " << strAmount << std::endl;
	util::debug(ss);
	int byteSize = intAmount * sizeof(int) + strAmount * sizeof(zSTRINGSerialized);
	BYTE* memory = (BYTE*)gothic2OperatorNew(byteSize);
	memset(memory, 0, byteSize);

	userData.intAmount = intAmount;
	userData.strAmount = strAmount;
	userData.pMemory = memory;
}