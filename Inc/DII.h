#pragma once
/*////////////////////////////////////////////////////////////////////////////

This file is part of neclib.

Copyright © 2015-2020 David Goeth

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


#include "Module.h"
#include "api/g2/ztypes.h"
#include <api/g2/zcpar_symbol.h>
#include <vector>
#include <api/g2/oCItemExtended.h>
#include <ocgameExtended.h>
#include <map>
#include <list>

typedef void(__thiscall* OCItemInsertEffect)(oCItem*);

class oCMobContainer;
class oCNpcInventory;

struct zTBBox2D
{
	float p1x;
	float p1y;
	float p2x;
	float p2y;
};

/**
 * A module for generating gothic 2 item instances at runtime. 
 */
class DII : public Module
{

public:

	/**
	 * Creates a new DynItemInst module.
	 */
	DII();

	/**
	 * Default virtual destructor.
	 */
	virtual ~DII();

	static void oCItemGetValueHookNaked();
	static void loadSavegameHookNaked();
	static void writeSavegameHookNaked();
	static void createInstanceHookNaked();
	static void oCGameLoadGameHookNaked();
	static void oCItemMulitSlotHookNaked();
	static void zCParserGetIndexHookNaked();
	static void zCPar_SymbolTableGetSymbolStringHookNaked();
	//static void zCPar_SymbolTableGetSymbolHookNaked();
	static void zCPar_SymbolTableGetIndexHookNaked();

	static OCItemInsertEffect oCItemInsertEffect;
	
	static int __fastcall zCCameraScreenProjectionTouchesPortalHookNaked(void* pThis, zTBBox3D const &, zTBBox2D const &);

	/**
	 * Extends functionality of oCItem::GetValue().
	 * It is needed for displaying ingame the right item value for a dynamic instance.
	 * \param pThis A pointer to a valid oCItem object.
	 * \return The value of the oCItem
	 */
	static int __thiscall oCItemGetValueHook(void* pThis);

	//static void oCItemGetValueHookNaked();

	/**
	 * Extends functionality of oCGame::loadSavegame(int,int)
	 * It is used for loading dynamic instances from savegame and recreating them.
	 * \param pThis A pointer to a valid oCGame object.
	 * \param saveGameSlotNumber The slot number of the savegame to be loaded.
	 * \param b 
	 */
	static void __thiscall loadSavegameHook(void* pThis,int saveGameSlotNumber, int b);

	/**
		 * Extends functionality of oCGame::writeSavegame(int,int)
		 * It is used for saving dynamic instances.
		 * \param pThis A pointer to a valid oCGame object.
		 * \param saveGameSlotNumber The slot number of the savegame to be written to.
		 * \param b 
		 */
	static void __thiscall writeSavegameHook(void* pThis,int saveGameSlotNumber, int b);

	/**
	 * Extends functionality of zCPar_SymbolTable::GetSymbol(int)
	 * It is used for getting the zCParSymbol of a dynamic instance.
	 * \param pThis A pointer to a valid zCPar_SymbolTable object.
	 * \param index The index of the wished symbol.
	 * \result The found symbol. If no symbol was found, NULL will be returned.
	 */
	static zCPar_Symbol* __thiscall zCPar_SymbolTableGetSymbolHook(void* pThis, int index);

	/**
	 * Extends functionality of zCPar_SymbolTable::GetSymbolString(zSTRING const &)
	 * It is used for getting the zCParSymbol of a dynamic instance.
	 * \param pThis A pointer to a valid zCPar_SymbolTable object.
	 * \param symbolName The name of the symbol to get the index of
	 * \result The found symbol. If no symbol was found, NULL will be returned.
	 */
	static zCPar_Symbol* __thiscall zCPar_SymbolTableGetSymbolStringHook(void* pThis, zSTRING const & symbolName);
	
	/**
	 * Extends functionality of zCPar_SymbolTable::GetIndex(zSTRING const &)
	 * It is used for getting the index (the instance id) of a dynamic instance.
	 * \param pThis A pointer to a valid zCPar_SymbolTable object.
	 * \param symbolName The name of the symbol to get the index of
	 * \result The index of the symbol. If the symbol wasn't found, NULL will be returned.
	 */
	static int __thiscall zCPar_SymbolTableGetIndexHook(void* pThis, zSTRING const & symbolName);

	/**
	 * Extends functionality of zCParser::GetIndex(zSTRING const &)
	 * It is used for getting the index (the instance id) of a dynamic instance.
	 * \param pThis A pointer to a valid zCParser object.
	 * \param symbolName The name of the symbol to get the index of
	 * \result The index of the symbol. If the symbol wasn't found, NULL will be returned.
	 */
	static int __thiscall zCParserGetIndexHook(void* pThis, zSTRING const & symbolName);

	/**
	 * Extends functionality of zCParser::createInstance(int, void*)
	 * It is used for initializing an oCItem with a dynamic instance.
	 * \param pThis A pointer to a valid zCParser object
	 * \param instanceId The instance id to create an instance of.
	 * \param source A pointer to a memory region of the size of an oCItem.
	 * \return 0 if creation wasn't successful.
	 */
	static int __thiscall createInstanceHook(void* pThis, int instanceId, void* source);

	/**
	 * Extends functionality of oCGame::LoadGame(int, zSTRING const &)
	 * LoadGame is called when the user starts a new gothic 2 game. 
	 * This hook is then used for resetting internal module state. 
	 * \param pThis A pointer to a valid oCGame object.
	 * \param second 
	 * \param worldName The name of the starting world
	 */
	static void __thiscall oCGameLoadGameHook(void* pThis, int second, zSTRING const & worldName);


	/**
	 * Extends functionality of oCItem::MulitSlot()
	 * which checks if the item is stackable or not.
	 * While writing a savegame it is not desired that items with a dynamic instance id
	 * are stackable. So the function MulitSlot() denies multi slotting for all items if the member
	 * DynItemInst::denyMultiSlot is set to true.
	 * \param pThis A pointer to a valid oCItem object.
	 */
	static int __thiscall oCItemMulitSlotHook(void* pThis);


	/**
	 * Calls the gothic 2 function zCVisual::LoadVisual(zSTRING const &)
	 * \param name The name of the visual to load
	 * \return The loaded zCVisual
	 */
	static zCVisual* __cdecl zCVisualLoadVisual(zSTRING const & name);

	/**
	 * Provides an sublist of the given inventory. At its head position the list has an oCItem
	 * which has an instance id equal to that one that was provided to this function.
	 * \param inventory The inventory to get the sub list of.
	 * \param instanceId The instance id which describes the starting point of the sublist.
	 * \return A sublist of the provided inventory. At the head of this list is an oCItem with
	 * the same instance id as provided to this function. If the inventory doesn't contain 
	 * an oCItem with the provided instance id, an empty list will be returned.
	 */
	static zCListSort<oCItem>* getInvItemByInstanceId(oCNpcInventory* inventory, int instanceId);

	static oCItem* getInvItemByInstanceId2(oCNpcInventory* inventory, int instanceId);

	static void __thiscall zCCameraSetFarClipZHook(void* pThis, float value);

	static void __thiscall zCVobUpdatePhysicsHook(void* pThis);

	//.text:0064B260 public: virtual void __thiscall zCRnd_D3D::DrawPoly(class zCPolygon *) proc near
	static void __thiscall zCRnd_D3DDrawPolyHook(void* pThis, void* poly);

	//.text:0064AC30 public: virtual void __thiscall zCRnd_D3D::DrawPolySimple(class zCTexture *, struct zTRndSimpleVertex *, int) proc near
	static void __thiscall zCRnd_D3DDrawPolySimpleHook(void* pThis, void* second, void* third, int fourth);

	//.text:00650CF0 public: virtual void __thiscall zD3D_alphaPoly::Draw(int) proc near
	static void __thiscall zCRnd_alphaPolyDrawHook(void* pThis, int second);

	//.text:006B5810 protected: void __thiscall oCAniCtrl_Human::CheckFallStates(void) proc near
	static void __thiscall oCAniCtrl_HumanCheckFallStatesHook(void* oCAniCtrl_Human);

	//.text:00529DD0 public: int __thiscall zCPolygon::RenderPoly(int) proc near
	static int __thiscall zCPolygonRenderPolyHook(void* pThis, int second);

	//.text:005B7B20 public: int __fastcall zCPolygon::ClipToFrustum(int) proc near
	static int __fastcall zCPolygonClipToFrustumHook(void* pThis, int second);

	//.text:00534B70 private: void __thiscall zCBspSector::ActivateSectorRecIndoor(struct zTBBox2D const &, class zCBspSector *, int) proc near
	static void __thiscall zCBspSectorActivateSectorRecIndoorHook(void* pThis, zTBBox2D const & second, void* third, int fourth);

	//.text:0061CBC0 ; void __thiscall zCVob::SetHeadingAtWorld(zCVob *this, const struct zVEC3 *)
	static void __thiscall zCVobSetHeadingAtWorldHook(void* pThis, zVEC3* vec);

	static void __thiscall DoSurfaceAlignmentHook(void* pThis);

	//.text:0050D4D0 ; int __thiscall zCAIPlayer::CheckFloorSliding(zCAIPlayer *__hidden this)
	static int __thiscall zCAIPlayerCheckFloorSlidingHook(void* pThis);

	static void __thiscall zCVobSetPhysicsEnabledHook(void* pThis, int second);


    /*! @copydoc Module::hookModule()
	 */
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	 */
	virtual void unHookModule() override;





	/**
	 * Adds a proxy from one instance to another instance.
	 * \return true if the proxy is successfully setup.
	 */
	static bool DII_AddProxy(const zSTRING& sourceInstanceName, const zSTRING& targetInstanceName);

	static void DII_ApplyInstanceChangesToAll(const zSTRING& instanceName);

	/**
	 * Removes a proxy.
	 */
	static void DII_RemoveProxy(const zSTRING& sourceInstanceName);

	/**
	 * An external for creating an item with a specific instance id.
	 * Daedalus syntax: Func c_item DII_CreateNewItem(var int parserSymbolIndex)
	 * \param parserSymbolIndex The instance id of the oCItem to be created.
	 * \return The created item. If no item could be created, NULL will be returned.
	 */
	static oCItem* __cdecl  DII_CreateNewItem(int parserSymbolIndex);

	/**
	 * An external for deleting a dynamic instance by its parser symbol table index.
	 * Daedalus syntax: Func void DII_DeleteDII(var string instanceName)
	 * \param parserSymbolIndex The instance id to be deleted.
	 */
	static void __cdecl DII_DeleteDII(int parserSymbolIndex);

	/**
	 * An external for deleting an oCItem object by its pointer.
	 * Daedalus syntax: Func void DII_DeleteItem(var oCItem itm)
	 * \param item The oCItem to be deleted.
	 */
	static void __cdecl DII_DeleteItem(oCItem* item);
	/**
	 * An external for creating a new instance id from a given c_item.
	 * Daedalus syntax: Func int DII_CreateNewInstance(C_Item item)
	 * \param item The oCItem to use as reference for creating a new dynamic instance.
	 * \return The id of the created instance. If no instance id
	 * could be created, NULL will be returned.
	 */
	static int DII_CreateNewInstanceInt(oCItem* item);
	static zSTRING* DII_CreateNewInstance(oCItem* item);
	static int DII_CreateNewInstanceStr(oCItem* item, const zSTRING& instanceName);

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
	static BYTE* DII_GetUserData(int instanceIdParserSymbolIndex);

	static bool DII_UpdateInstance(const zSTRING& instanceName, oCItem* item);

	//TODO: is this function used?
	static void DII_AssignInstanceId(oCItem* item, int instanceIdParserSymbolIndex);

	static void DII_GetItemByInstanceId(int itemParserSymbolIndex, int instanceIdParserSymbolIndex);

	//Asiigns all items with instance id targetId the new id newId
	static void DII_ChangeInstanceForAll(const zSTRING& sourceName, const zSTRING& targetName);


public:
	static const std::string SAVE_ITEM_FILE_EXT;
	static const std::string SAVE_ITEM_INSTANCES;
	static const std::string FILE_PATERN;

private:
	static std::string getClearedWorldName(zSTRING const & worldName);
	static void loadDynamicInstances(int saveGameSlotNumber);

private:

	static const int LOAD_SAVEGAME_ADDRESS = 0x006C67D0;
	static const int WRITE_SAVEGAME_ADDRESS = 0x006C5250;
	static const int OCITEM_GET_VALUE_ADDRESS = 0x00712650;
	static const int ZCPAR_SYMBOL_TABLE_GETSYMBOL = 0x007A3EE0;
	static const int ZCPAR_SYMBOL_TABLE_GETSYMBOL_STRING = 0x007A3E40;
	static const int ZCPAR_SYMBOL_TABLE_GETINDEX = 0x007A3B60;    
	static const int ZCPARSER_GETINDEX = 0x00793470;
	static const int ZCPARSER_CREATE_INSTANCE = 0x00792FA0;
	static const int OCGAME_LOAD_GAME_ADDRESS = 0x006C65A0;

	static const int OCNPC_UNEQUIP_ITEM = 0x007326C0;
	static const int OCGAME_CHANGE_LEVEL = 0x006C7290;

	static const int OCGAME_LOAD_WORLD_ADDRESS = 0x006C90B0;
	static const int OCGAME_CHANGE_LEVEL_ADDRESS = 0x006C7290;
	static const int OCITEM_MULTI_SLOT = 0x007125A0;
	static const int OCMOB_CONTAINER_OPEN = 0x00726500;
	static const int ZCVISUAL_LOAD_VISUAL = 0x00606AD0;


	//void __thiscall zCVob::SetHeadingAtWorld(zCVob *this, const struct zVEC3 *)
	static const int ZCVOB_SET_HEADING_AT_WORLD = 0x0061CBC0;

	static bool showExtendedDebugInfo;

	class DII_InstanceNameNotFoundException : protected std::exception {
	private:
		std::string err_msg;

	public:
		DII_InstanceNameNotFoundException(const char *msg) : err_msg(msg) {};
		~DII_InstanceNameNotFoundException() throw() {};
		const char *what() const throw() { return this->err_msg.c_str(); };
	};



	struct UPDATE_INSTANCE_PARAMS {
		int index;
	};

	class ItemUpdater {
	public:

		struct UpdateItemData {

			/**
			 * Only updates items having this instance id
			 */
			int expectedInstanceID;

			/**
			 * The new instance id for items to be updated.
			 */
			int newInstanceID;
		};

		static void updateItem(void* obj, void* param, oCItem* itm);

		static void updateItemInstance(void* obj, void* param, oCItem* itm);

	};

};