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


#ifndef __AdditMemory_H__
#define __AdditMemory_H__

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "ISerialization.h"

/**
 * A class for storing additional information for oCItems. This class is used 
 * to store the value field of an oCItem having a dynamic instance id. The value field is used 
 * to mark the oCItem as dynamic (important while loading a savegame) and for storing
 * a key to its additional memory. So additional memory is needed.
 */
class AdditMemory : public ISerialization {

private:
	 friend class boost::serialization::access;

    /*! @copydoc ISerialization::serialize(Archive & ar, const unsigned int version)
	 */
    template<class Archive> void serialize(Archive & ar, const unsigned int version)
    {
		// serialize base class information
        ar & boost::serialization::base_object<ISerialization>(*this);    
		ar & additId;
		ar & instanceId;
		ar & instanz;
		ar & flags;
		ar & activeSpellItem;
		ar & spellKey;
    }
public:

	/**
	*  Creates new additional memory.
	*/
	AdditMemory() {
		additId = 0;
		instanceId = 0;
		instanz = 0;
		flags = 0;
		activeSpellItem = 0;
		spellKey = -1;

		referenceCount = 0;	//don't serialize it!
		visited = false;
	}

	int additId;
	int instanceId;
	int instanz;
	int flags;
	bool activeSpellItem;
	int spellKey;
	int referenceCount;	//don't serialize it!
	bool visited;

	/*! @copydoc ISerialization::myser(boost::archive::text_iarchive &ia)
	 */
	virtual void myser(boost::archive::text_iarchive &ia) override
	{ia & *this;}

	/*! @copydoc ISerialization::myser(boost::archive::text_oarchive &oa)
	 */
	virtual void myser(boost::archive::text_oarchive &oa) override
	{oa & *this;}
};

#endif __AdditMemory_H__
