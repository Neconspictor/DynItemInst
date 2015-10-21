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

#ifndef __MODULE_H
#define __MODULE_H
#include <sstream>

/**
* A Module is an interface for pooling function hooks belonging together 
* and extending Gothic 2 functionality.
*/
class Module
{
public:
	/**
	 * Default constructor.
	 */
	Module()
	{	
	};

	/**
	 * Default virtual destructor.
	 */
	virtual ~Module()
	{
	};

	/**
	 * Initializes and hooks target functions needed by this module. 
	 */
	virtual void hookModule() = 0;

	/**
	 * Releases this module and rolls back function hooks. 
	 */
	virtual void unHookModule() = 0;

protected:

	/**
	 *  A description of this module. Should be unique per module.
	 */
	std::string moduleDesc;

	/**
	 * This stream is intended to be used for logging purposes.
	 */
	static std::stringstream logStream;
};

#endif __MODULE_H