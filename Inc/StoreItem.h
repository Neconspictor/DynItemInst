#ifndef __StoreItem_H__
#define __StoreItem_H__

#include "api/g2/ocitem.h"
#include <iostream>

#include "ISerialization.h"

class StoreItem : public ISerialization {

public:
	static const int MAX_USER_DATA = 100;

	StoreItem();
	~StoreItem();
private:
	 friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		// serialize base class information
        ar & boost::serialization::base_object<ISerialization>(*this);
		ar & zCPar_Symbol_name;
		ar & zCPar_Symbol_Bitfield;
        ar & parentInstanceId;
		ar & instanceID;
		ar &		idx;
		ar & name;
		ar & nameID;
	    ar &		hp;
	    ar &		hp_max;
	    ar &     mainflags;
	    ar &		flags;
	    ar &		weight;
	    ar &     value;

	// -- weapons		
	    ar &     damageType;
	    ar &     damageTotal;
	    ar &     damage;

	// -- armor 
	    ar &     wear;
	    ar &     protection;

	// -- food
	    ar &     nutrition;

	// -- use attributes
	    ar &     cond_atr;
	    ar &     cond_value;

	// -- attributes that will be changed on equip
	    ar &     change_atr;
	    ar &     change_value;

	// -- parser functions
	    ar &     magic;
	    ar &     on_equip;
	    ar &     on_unequip;	
	    ar &     on_state;			

	// -- owner									
	    ar &		owner;			//	owner: npc instance
	    ar &		ownerGuild;		//	owner: guild
	    ar &		disguiseGuild;

	// -- visual
		ar &	visual;

	// -- change of mesh on equip
		ar & visual_change;	//	ASC file
		ar & effect;			//  Effect instance

	    ar &		visual_skin;

		ar &	scemeName;
	    ar &		material;	
	    ar &		munition;		//	Instance of ammunition

	    ar & 	spell;			
	    ar &		range;			

	    ar &		mag_circle;

		ar &	description;
		ar &	text;
	    ar &	count;

	// -- inventory presentation
	    ar &	    inv_zbias;								//  far plane (how far the item goes into the room by the z-axis)
	    ar &		inv_rotx;								//  rotation around x-axis (in degrees)
	    ar & 	inv_roty;								//  rotation around y-axis (in degrees)
	    ar & 	inv_rotz;								//  rotation around z-axis (in degrees)
	    ar & 	inv_animate;							//  rotate the item
	    ar & instanz;						//    ar & Symbolindex
	    ar & c_manipulation;					//int ?
	    ar & last_manipulation;				//zREAL ?
	    ar & magic_value;					//int ?
	    ar & effectVob;						//oCVisualFX*
	    ar & next;	

		ar & userData;

    }
public:

	std::string zCPar_Symbol_name;
	int zCPar_Symbol_Bitfield;
	int parentInstanceId;
	int instanceID;
	int		idx;
	std::string name;
	std::string nameID;
	int		hp;
	int		hp_max;
	int     mainflags;
	int		flags;
	int		weight;
	int     value;

	// -- weapons		
	int     damageType;
	int     damageTotal;
	int     damage[8];

	// -- armor 
	int     wear;	
	int     protection[8];

	// -- food
	int     nutrition;

	// -- use attributes
	int     cond_atr[3];
	int     cond_value[3];

	// -- attributes that will be changed on equip
	int     change_atr[3];
	int     change_value[3];		//51

	// -- parser functions
	int     magic;
	int     on_equip;
	int     on_unequip;	
	int     on_state[4];			

	// -- owner									
	int		owner;			//	owner: npc instance
	int		ownerGuild;		//	owner: guild
	int		disguiseGuild;

	// -- visual
	std::string	visual;

	// -- change of mesh on equip
	std::string visual_change;	//	ASC file
	std::string effect;			//  Effect instance

	int		visual_skin;

	std::string	scemeName;
	int		material;	
	int		munition;		//	Instance of ammunition

	int 	spell;			
	int		range;			//85		

	int		mag_circle;

	std::string	description;
	std::string	text[6];
	int		count[6];

	// -- inventory presentation
	int	    inv_zbias;								//  far plane (how far the item goes into the room by the z-axis)
	int		inv_rotx;								//  rotation around x-axis (in degrees)
	int 	inv_roty;								//  rotation around y-axis (in degrees)
	int 	inv_rotz;								//  rotation around z-axis (in degrees)
	int 	inv_animate;							//  rotate the item
	int instanz;						//int Symbolindex
	int c_manipulation;					//int ?
	int last_manipulation;				//zREAL ?
	int magic_value;					//int ?
	int effectVob;						//oCVisualFX*
	int next;							//138
	int userData[MAX_USER_DATA];

	int getInstanceID() {
		return instanceID;	
	}

	void setInstanceID(int newID) {
		instanceID = newID;
	}

	void store(oCItem& item);

	void init(oCItem* item, int index);

	int getParentInstanceId();
	void setParentInstanceId(int parentInstanceId);

	std::string getZCPar_SymbolName();
	void setZCPar_SymbolName(std::string name);

	virtual void myser(boost::archive::text_iarchive &ia) override
	{ia & *this;}
	virtual void myser(boost::archive::text_oarchive &oa) override
	{oa & *this;}

	int getParserSymbolBitfield();
	void setParserSymbolBitfield(int bitfield);

	void setUserData(int index, int data);
	int getUserData(int index);
	int* getUserData();

	void copyUserData(StoreItem& source);
};

#endif __StoreItem_H__