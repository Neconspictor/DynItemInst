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

#ifndef __EXTERNALS_H__
#define __EXTERNALS_H__

#include "Module.h"

class zCParser;

/**
 * A module for extending the set of daedalus external functions.
 */
class Externals : public Module
{
public:

	/**
	 * Creates a new External object.
	 */
	Externals();

	/**
	 * Default virtual destructor.
	 */
	virtual ~Externals();

	/**
	 * Extends functionality of __thiscall oCGame::DefineExternals_Ulfi(zCParser*).
	 * It is used for registering the additional externals.
	 * \param pThis A pointer to the current oCGame object.
	 * \param parser The current parser instance
	 */
	static void __thiscall oCGameDefineExternals_UlfiHook (void* pThis, zCParser* parser);

    /*! @copydoc Module::hookModule()
	 */
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	 */
	virtual void unHookModule() override;

private:

	/**
	 * An external for creating an item with a specific instance id.
	 * Daedalus syntax: Func c_item DII_CreateNewItem(var int instanceId)
	 * \return Properly not used by gothic 2
	 */
	static int DII_CreateNewItem();

	/**
	 * An external for creating a new instance id from a given c_item.
	 * Daedalus syntax: Func int DII_CreateNewInstance(C_Item item)
	 * \return Properly not used by gothic 2
	 */
	static int DII_CreateNewInstance();

	/**
	 * An external for checking if a given c_item has a dynamic instance id.
	 * Daedalus syntax: Func int DII_IsDynamic(C_Item item)
	 * \return Properly not used by gothic 2
	 */
	static int DII_IsDynamic();

	/**
	 * An external for removing a c_item from the current world and deleting it.
	 * Daedalus syntax: Func void DII_DeleteItem(C_Item item)
	 * \return Properly not used by gothic 2
	 */
	static int DII_DeleteItem();

	/**
	 * An external for getting additional memory of the instance of the given c_item.
	 * Only for dynamic instances such a memory will be created. If the provided c_item
	 * hasn't a dynamic instance id, NULL will be set as return result.
	 * Daedalus syntax: Func DII_UserData DII_GetUserData(C_Item item)
	 * \return Properly not used by gothic 2
	 */
	static int DII_GetUserData();

private:

	/**
	 * The address of the function __thiscall oCGame::DefineExternals_Ulfi(zCParser*)
	 */
	static const int OCGAME_DEFINE_EXTERNALS_ULFI = 0x006D4780;
};

#endif __EXTERNALS_H__