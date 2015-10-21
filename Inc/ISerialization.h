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

#ifndef __ISERILIZATION_H__
#define __ISERILIZATION_H__

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/**
 * ISerialization is an interface for objects that have to be serialized 
 * and can be serialized in text form.
 */
class ISerialization {
public:

	/**
	 * Initializes this object with a given serialized text_iarchive.
	 */
	virtual void myser(boost::archive::text_iarchive &ia) {ia & *this;}

	/**
	 * Serializes and stores this object in a given text_oarchive.
	 */
	virtual void myser(boost::archive::text_oarchive &oa) {oa & *this;}

	virtual ~ISerialization() {};

private:
	// friend modifier needed by the boost library!
	friend class boost::serialization::access;

	/**
	 * Serializes an object of type Archive.
     * When the class Archive corresponds to an output archive, the
     * & operator is defined similar to <<.  Likewise, when the class Archive
     * is a type of input archive the & operator is defined similar to >>.
	 */
    template<class Archive> void serialize(Archive & ar, const unsigned int version) {}
};

#endif __ISERILIZATION_H__