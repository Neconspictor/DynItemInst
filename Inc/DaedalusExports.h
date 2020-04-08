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
#include "Telekinesis.hpp"

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
	static oCItem* __cdecl  DII_CreateNewItem(int parserSymbolIndex);

	static void __cdecl DII_ReleaseItem(int parserSymbolIndex);

	static void __cdecl DII_DeleteItem(oCItem* item);
	/**
	 * An external for creating a new instance id from a given c_item.
	 * Daedalus syntax: Func int DII_CreateNewInstance(C_Item item)
	 * \param item The oCItem to use as reference for creating a new dynamic instance.
	 * \return The id of the created instance. If no instance id
	 * could be created, NULL will be returned.
	 */
	static int DII_CreateNewInstance(oCItem* item);
	static zSTRING* DII_CreateNewInstanceStr(oCItem* item);
	static int DII_CreateNewInstanceStr2(oCItem* item, const char* instanceName);

	static void DII_EquipItem(oCNpc* npc, int parserSymbolIndex);

	/**
	 * An external for checking if a given c_item has a dynamic instance id.
	 * Daedalus syntax: Func int DII_IsDynamic(C_Item item)
	 * \param item The oCItem to check
	 * \return Is the instance id of the provided item dynamic?
	 */
	static int DII_IsDynamic(oCItem* item);

	static int DII_IsInstanceDynamic(int parserSymbolIndex);

	/**
	 * An external for getting additional memory of the instance of the given c_item.
	 * Only for dynamic instances such a memory will be created. If the provided c_item
	 * hasn't a dynamic instance id, NULL will be set as return result.
	 * Daedalus syntax: Func DII_UserData DII_GetUserData(C_Item item)
	 * \param instanceId The instance id to get the DII_UserData from
	 * \return The user data of the provided instance id
	 */
	static BYTE* DII_GetUserData(int parserSymbolIndex);

	/**
	 * Provides the version of the library.
	 */
	static float DII_GetLibVersion();

	static void DII_DoStatistics();

	static void DII_UpdateInstance(oCItem* item);

	//TODO: is this function used?
	static void DII_AssignInstanceId(oCItem* item, int parserSymbolIndex);

	static void DII_GetItemByInstanceId(int index, int parserSymbolIndex);

	//Asiigns all items with instance id targetId the new id newId
	static void DII_ChangeItemsInstanceId(int targetParserSymbolIndex, int newParserSymbolIndex);

	static void DII_ToggleLevitation();

	static void DII_SetHeroFocusMode(int mode);

	static void DII_Test(oCItem* item, int mode);

	static void DII_TransformationTest(zCVob* vob);

	static void DII_TelekineseTest();

	static bool DII_Npc_CanTalk(oCNpc* npc);

	static TelekinesisInterpolator* DII_Telekinesis_createInterpolator(const zVEC3* vobPosition,
		const zVEC3* npcPosition, 
		int upMoveAmount,  // cm
		int speed // cm / s
	);

	static void DII_Telekinesis_GetInterpolatedVec(TelekinesisInterpolator* interpolatorPtr, zVEC3* dest);

	static void DII_Telekinesis_deleteInterpolator(TelekinesisInterpolator* interpolatorPtr);

	static void DII_Telekinesis_Interpolate(TelekinesisInterpolator* interpolatorPtr, oCItem* item);

	static int DII_Npc_CanSeeVob(oCNpc* npc, zCVob* vob);

	static void DII_DrobVob(oCNpc* npc, zCVob* vob);

public:

	/**
	 * The version of this library.
	 */
	static const float LIB_VERSION;

	static std::vector<std::unique_ptr<TelekinesisInterpolator>> interpolators;
};

#endif __DaedalusExportsH__