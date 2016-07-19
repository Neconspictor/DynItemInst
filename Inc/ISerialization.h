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
#include <istream>
#include <sstream>


/**
 * ISerialization is an interface for objects that have to be serialized 
 * and can be serialized in text form.
 */
class ISerialization {
public:

	/**
	 * Initializes this object with a given input stream.
	 */
	virtual void deserialize(std::stringstream*) = 0;

	/**
	 * Serializes and stores this object in a given output stream.
	 */
	virtual void serialize(std::ostream&) const = 0;

	virtual ~ISerialization() {};

	void readString(std::stringstream* is, std::string& data)
	{
		while (is->peek() == ' ')
		{
			is->get();
		}

		int size = 0;
		getInt(*is, size);
		if (size)
		{
		data.resize(size + 1, '\0');
		is->get(&data[0], size + 1);
		
		} else
		{
			data = "";
		}
		//*is >> data;
	}

	static inline void writeString(std::ostream& os, const std::string& data)
	{
		os << data.size() << ' ';
		os << data.c_str();
	}

	void getInt(std::stringstream& ss, int& param)
	{
		while (ss.peek() == ' ')
		{
			ss.get();
		}

		std::string token;
		getline(ss, token, ' ');
		param = atoi(token.c_str());
	};

	void getBool(std::stringstream& ss, bool& param)
	{
		while (ss.peek() == ' ')
		{
			ss.get();
		}
		std::string token;
		getline(ss, token, ' ');
		param = atoi(token.c_str());
	};
};

#endif __ISERILIZATION_H__