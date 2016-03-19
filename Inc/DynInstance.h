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

/////////////////////////////////////////////////////////////////////////////**/

#ifndef __DYN_INSTANCE_H__
#define __DYN_INSTANCE_H__

#include "ISerialization.h"
#include <boost/serialization/array.hpp>
#include "oCItemExtended.h"
#include "Util.h"
#include <sstream>


class zCParser;

/*Serialized zSTRING version*/
struct zSTRINGSerialized
{
	int _vtbl; //immer 0
	int _allocater; //immer 0
	char* ptr; //pointer zu den Daten
	int len; //Länge des Strings
	int res; //Anzahl allozierter Bytes
};

/**
 * Stores additional memory the user can use to give a dynamic instance additional attributes.
 * This class will be mainly used over the Daedalus script language.
 */
class DII_UserData : public ISerialization {
public:
	DII_UserData();
	//DII_UserData(zCParser* parser, int instance);
	~DII_UserData();

	/*! @copydoc ISerialization::myser(boost::archive::text_iarchive &ia)
	 */
	virtual void myser(boost::archive::text_iarchive &ia) override
	{ia & *this;}

	/*! @copydoc ISerialization::myser(boost::archive::text_oarchive &oa)
	 */
	virtual void myser(boost::archive::text_oarchive &oa) override
	{oa & *this;}

public:

	/**
	 * Specifies the size for additional memory.
	 */
	static const int MAX_USER_DATA = 100;

	static const int MAX_STRING_DATA = 1;

	/**
	 * Stores the user data.
	 */
/*	struct Data
	{
		int data[MAX_USER_DATA];
		zSTRINGSerialized stringData[MAX_STRING_DATA];
	};*/

	struct MemoryData
	{
		int intAmount;
		int strAmount;
		BYTE* pMemory;

		int* getIntBegin()
		{
			return (int*)pMemory;
		}

		zSTRINGSerialized* getStr(int index)
		{
			return (zSTRINGSerialized*)pMemory[intAmount * sizeof(int) + index * sizeof(zSTRINGSerialized)];
		}
	} userData;


private:
	friend class boost::serialization::access;

	void createMemory(int intAmount, int strAmount);


	/*! @copydoc ISerialization::serialize(Archive & ar, const unsigned int version)
	 */
    template<class Archive> void serialize(Archive & ar, const unsigned int version)
    {
	    // serialize base class information
        ar & boost::serialization::base_object<ISerialization>(*this);
		ar & userData.intAmount;
		ar & userData.strAmount;

		if (Archive::is_loading::value)
		{
			//assert(userData.pMemory == nullptr);
			//int byteSize = userData.intAmount * sizeof(int) + userData.strAmount * sizeof(zSTRINGSerialized);
			//userData.pMemory = new BYTE[byteSize];
		}

		//save int array
		for (int i = 0; i < userData.intAmount; ++i)
		{
			ar & userData.getIntBegin()[i];
		}

		std::stringstream ss; 
		ss << "strAmount: " << userData.strAmount << std::endl;
		util::debug(&ss);

		//save bytestream
		int indexBegin = userData.intAmount * sizeof(int);
		/*for (int i = 0; i < byteAmount; ++i)
		{
			ar & userData.pMemory[indexBegin + i];
		}*/

		//save the first string
		zSTRINGSerialized* ptr;  
		
		for (int i = 0; i < userData.strAmount; ++i)
		{
			ptr = (zSTRINGSerialized*)((userData.pMemory) + indexBegin + i*sizeof(zSTRINGSerialized));
			ar & ptr->_vtbl;
			ar & ptr->_allocater;

			if (Archive::is_loading::value)
			{
				//assert(userData.getStr(i)->ptr == nullptr);
				std::string data;
				ar & data;
				//&userData.pMemory[indexBegin] = new char[userData.getStr(i)->len];
				ptr->len = data.size();
				ptr->ptr = new char[ptr->len];
				strcpy(ptr->ptr, data.c_str());

				ss << "string loaded: " << std::string(ptr->ptr) << std::endl;
				util::debug(&ss);
			}
			else
			{
				std::string data;
				if (ptr->ptr == nullptr)
				{
					data = "";
				}
				else
				{
					data = std::string(ptr->ptr);
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
				} else
				{
					ss << "ptr->ptr: null" << std::endl;
					util::debug(&ss);
				}

				ss << "ptr->len: " << ptr->len << std::endl;
				util::debug(&ss);

				ss << "ptr->res: " << ptr->res << std::endl;
				util::debug(&ss);
				ar & data;
			}

			ar & ptr->res;
		}
    }
};



/**
 * A class for representing a dynamic instance of an oCItem. It contains all relevant information for 
 * initializing and storing oCItems and additional memory which can be freely used by the user.
 */
class DynInstance : public ISerialization {

public:

	/**
	 * Creates a new dynamic instance.
	 */
	DynInstance();

	/**
	 * Creates a new instance and initializes it with the given one.
	 */
	DynInstance(oCItem& item);

	/**
	 * Default destructor.
	 */
	~DynInstance();

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
	DII_UserData dii_userData;

	//specifies whether this instance can be reassigned
	bool notUsed;

	/**
	 * Provides the Item's instance id.
	 */
	int getInstanceID() {
		return instanceID;	
	}

	/**
	 * Sets the this' instance id to the given one.
	 */
	void setInstanceID(int newID) {
		instanceID = newID;
	}

	/**
	 * Initializes this dynamic instance with the given oCItem.
	 * \param item The oCItem used to initialize this dynamic instance.
	 */
	void store(oCItem& item);

	/**
	 * Inits the provided oCItem with this dynamic instance and sets the oCItem's instance id 
	 * to the second parameter.
	 * \param item The item to be initialized
	 * \param index The symbol index of the dynamic instance.
	 */
	void init(oCItem* item, int index);

	/**
	 * Provides the parent's instance id of this dynamic instance. The Parent instance id is the parser 
	 * index of the base class of this dynamic instance.
	 */
	int getParentInstanceId();

	/**
	 * Sets this' parent instance id. The Parent instance id is the parser index of the base class of
	 * this dynamic instance.
	 */
	void setParentInstanceId(int parentInstanceId);

	/**
	 * \return The name of the zCPar_Symbol associated with this dynamic instance
	 */
	std::string getZCPar_SymbolName();

	/**
	 * Sets the name of the zCPar_Symbol associated with this dynamic instance.
	 * \param name The name of the zCPar_Symbol
	 */
	void setZCPar_SymbolName(std::string name);

	/*! @copydoc ISerialization::myser(boost::archive::text_iarchive &ia)
	 */
	virtual void myser(boost::archive::text_iarchive &ia) override
	{ia & *this;}

	/*! @copydoc ISerialization::myser(boost::archive::text_oarchive &oa)
	 */
	virtual void myser(boost::archive::text_oarchive &oa) override
	{oa & *this;}

	/**
	 * \return The content of the bitfield member of the parser symbol associated with this class. 
	 */
	int getParserSymbolBitfield();

	/**
	 * Sets the parser symbol's bitfield meber associated with this class. 
	 * \param bitfield The bitefield parameter to be set
	 */
	void setParserSymbolBitfield(int bitfield);

	/**
	 * Sets the user data of a certain index.
	 * \param index The index for the user data
	 * \param data The value to be set
	 */
	//void setUserData(int index, int data);

	/**
	 * Provides the user data of a certain index.
	 * \param index The index of the user data.
	 */
	//int getUserData(int index);

	/**
	 * Provides access to the user data member of this class.
	 * \return The user data of this class.
	 */
	BYTE* getUserData();

	/**
	 * Makes the user data of this class to a copy of the content of the user data a given dynamic instance has.
	 * \param source The dynamic instance to copy the user data from.
	 */
	void copyUserData(DynInstance& source);

private:
	// friend modifier needed by the boost library!
	friend class boost::serialization::access;

	/*! @copydoc ISerialization::serialize(Archive & ar, const unsigned int version)
	 */
    template<class Archive> void serialize(Archive & ar, const unsigned int version)
    {
		// serialize base class information
        ar & boost::serialization::base_object<ISerialization>(*this);
		ar & notUsed;
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

		ar & dii_userData;
    }

	static std::stringstream logStream;

};

#endif __DYN_INSTANCE_H__