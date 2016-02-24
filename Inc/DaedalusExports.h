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



#ifndef __DaedalusExportsH__
#define __DaedalusExportsH__

#include "Module.h"
#include "DynInstance.h"

class oCItem;
class zCParser;

/** 
 * A module for providing an interface to use this library in Daedalus script language.
 */
class DaedalusExports : public Module
{
public:

	/**
	 * Creates a new DaedalusExport object.
	 */
	DaedalusExports();

	/**
	 * Default virtual destructor.
	 */
	virtual ~DaedalusExports();

    /*! @copydoc Module::hookModule()
	 */
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	 */
	virtual void unHookModule() override;

	/**
	 * An external for creating an item with a specific instance id.
	 * Daedalus syntax: Func c_item DII_CreateNewItem(var int instanceId)
	 * \param instanceId The instance id of the oCItem to be created.
	 * \return The created item. If no item could be created, NULL will be returned.
	 */
	static void __cdecl  DII_CreateNewItem(int index, int instanceId);

	/**
	 * An external for creating a new instance id from a given c_item.
	 * Daedalus syntax: Func int DII_CreateNewInstance(C_Item item)
	 * \param item The oCItem to use as reference for creating a new dynamic instance.
	 * \return The id of the created instance. If no instance id
	 * could be created, NULL will be returned.
	 */
	static int DII_CreateNewInstance(oCItem* item);

	static void DII_EquipItem(oCNpc* npc, int instanceId);

	/**
	 * An external for checking if a given c_item has a dynamic instance id.
	 * Daedalus syntax: Func int DII_IsDynamic(C_Item item)
	 * \param item The oCItem to check
	 * \return Is the instance id of the provided item dynamic?
	 */
	static int DII_IsDynamic(oCItem* item);

	/**
	 * An external for getting additional memory of the instance of the given c_item.
	 * Only for dynamic instances such a memory will be created. If the provided c_item
	 * hasn't a dynamic instance id, NULL will be set as return result.
	 * Daedalus syntax: Func DII_UserData DII_GetUserData(C_Item item)
	 * \param instanceId The instance id to get the DII_UserData from
	 * \return The user data of the provided instance id
	 */
	static DII_UserData::Data* DII_GetUserData(int instanceId);

	/**
	 * Provides the version of the library.
	 */
	static float DII_GetLibVersion();

	static void DII_DoStatistics();

	static void DII_UpdateInstance(oCItem* item);

	static void DII_AssignInstanceId(oCItem* item, int instanceId);

public:

	/**
	 * The version of this library.
	 */
	static const float LIB_VERSION;
};

#endif __DaedalusExportsH__