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
#include <Logger.h>

std::stringstream DynInstance::logStream;

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

void DynInstance::setUserData(int index, int data)
{
	dii_userData.userData.data[index] = data;
}

int DynInstance::getUserData(int index)
{
	return dii_userData.userData.data[index];
}

DII_UserData::Data* DynInstance::getUserData()
{
	return &dii_userData.userData;
}

void DynInstance::copyUserData(DynInstance& source)
{
	for (int i = 0; i < DII_UserData::MAX_USER_DATA; ++i)
	{
		dii_userData.userData.data[i] = source.dii_userData.userData.data[i];		
	}
};


DII_UserData::DII_UserData()
{
	ZeroMemory(userData.data, sizeof(int) * MAX_USER_DATA);
}

DII_UserData::DII_UserData(zCParser* parser, int instance)
{
	ZeroMemory(userData.data, sizeof(int) * MAX_USER_DATA);
	parser->CreateInstance(instance, &userData);
}

DII_UserData::~DII_UserData()
{
}