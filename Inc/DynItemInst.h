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



#ifndef __DYN_ITEM_INST_H
#define __DYN_ITEM_INST_H

#include "Module.h"
#include "api/g2/ztypes.h"
#include <api/g2/zcpar_symbol.h>
#include <vector>
#include <oCItemExtended.h>
#include <ocgameExtended.h>
#include <map>
#include <AdditMemory.h>
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
class DynItemInst : public Module
{

public:

	struct LevelChangeBean
	{
		oCItem* item;
		int dynamicInstanceId;
		int original_on_equip;
		int original_on_unequip;
		int effectVob;
		int weaponMode;
		AdditMemory* addit;
	};

	struct InstanceNames
	{
		std::string base;
		std::string nearFight;
		std::string distanceFight;
		std::string rune;
		std::string other;

		int nearFightCounter;
		int distanceFightCounter;
		int runeCounter;
		int otherCounter;
	};

	/**
	 * Creates a new DynItemInst module.
	 */
	DynItemInst();

	/**
	 * Default virtual destructor.
	 */
	virtual ~DynItemInst();

	static void oCItemGetValueHookNaked();
	static void loadSavegameHookNaked();
	static void writeSavegameHookNaked();
	static void createInstanceHookNaked();
	static void oCGameLoadGameHookNaked();
	static void oCGameChangeLevelHookNaked();
	static void oCItemMulitSlotHookNaked();
	static void oCMobContainerOpenHookNaked();
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
			 * Extends functionality of oCGame::ChangeLevel(int, zSTRING const &, zSTRING const &)
			 * Because equipped items of hero won't properly restored after a level change, all 
			 * equiped items will be unequipped and equipped again after the original level change.
			 * Additionally will all items with a dynamic instance restored in the target world and in the
			 * player's inventory.
			 * \param pThis A pointer to a valid oCGame object.
			 * \param sourceWorld The world to be leaved.
			 * \param targetWorld The world to be went to.
			 */
	static void __thiscall oCGameChangeLevelHook(void* pThis, zSTRING const & sourceWorld, zSTRING const & targetWorld);

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
	 * Extends functionality of oCMobCOntainer::Open()
	 * Additionally by calling the original function this function initializes all items in the container
	 * which have a dynamic instance id.
	 * \param pThis A pointer to a valid oCMobContainer object.
	 * \param npc The npc that wants to open the mob container.
	 */
	static void __thiscall oCMobContainerOpenHook(void* pThis, oCNpc* npc);



	static void __thiscall oCMag_BookSetFrontSpellHook(void* pThis, int number);


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

	/**
							 * Restores a given oCItem from savegame if it was previously modified by modifyItemForSaving(oCItem* item, bool isHeroItem).
							 * If the provided item isn't modified nothing will be done.
							 * \param item The item which should be restored.
							 * \param inventory The inventory the provided item is contained. If this field is NULL, it is expected
							 * that the item is located in the current world.
							 */
	static void restoreSpecificNpcItemStub( oCItem* item, 
											oCNpcInventory* inventory,
											AdditMemory* addit,
											std::map<int, oCItem*>* equippedSpells = NULL, 
											oCItem** activeSpellItem = NULL);

	/**
	 * Modifies a given oCItem for savegame writing if it has a dynamic instance id.
	 * If its instance id isn't dynamic nothing will be done.
	 * \param item The item which should be modified.
	 * \param isHeroItem is the item located in the player's inventory?
	 * that the item is located in the current world.
	 */
	static int modifyItemForSaving(oCItem* item, bool isHeroItem);

	/**
	 * Restores all items, previously modified through modifyItemForSaving(oCItem* item, bool isHeroItem).
	 * Recovery will be performed in the current world and in the inventories of all npcs.
	 * \param game The game instance the recovery should be performed to.
	 */
	static void restoreDynamicInstances(oCGame* game);

	static bool isSaveGameLoading();

	static oCItem* makeEquippedCopy(oCItem* item, oCNpcInventory* inventory);

	static bool itemsAreModified();

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


	static void checkReusableInstances();

	static void addToReusableLists(int instanceId, int previousId);

    /*! @copydoc Module::hookModule()
	 */
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	 */
	virtual void unHookModule() override;

public:
	static const std::string SAVE_ITEM_FILE_EXT;
	static const std::string SAVE_ITEM_INSTANCES;

	static const std::string SAVE_ITEM_ADDIT;
	static const std::string SAVE_ITEM_HERO_DATA;

	static const std::string FILE_PATERN;

	static InstanceNames instanceNames;

private:
	static std::string getClearedWorldName(zSTRING const & worldName);
	static void loadDynamicInstances();
	static void initAdditMemory();
	static void equipRangedWeapon(oCItem* item, oCNpcInventory* inventory, bool munitionUsesRightHand);
	static void resetInstanceNameStruct();

	static void restoreSelectedSpell(oCNpc* npc, oCItem* selectedSpellItem);
	static void restoreItemsOfNpc(oCNpc* npc);
	static void restoreSpecificNpcItem(void * obj, void * param, oCItem * itm);
	static void restoreWorldItem(void * obj, void * param, oCItem * itm);
	
	static oCItem* restoreItemAfterLevelChange(oCNpc* npc, LevelChangeBean* bean, int weaponMode, int readiedWeaponId,
		int munitionId, bool munitionUsesRightHand, std::map<int, oCItem*>* equippedSpells, 
		oCItem** selectedSpellItem);

	static void restoreEquippedItem(oCItem*, oCNpcInventory* inventory, AdditMemory* addit, int instanceId, 
		std::map<int, oCItem*>* equippedSpells, oCItem** activeSpellItem);

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

	static const int OCITEM_FLAG_ITEM_KAT_RUNE = 512;

	static std::vector<zCPar_Symbol*>* symbols;
	static bool showExtendedDebugInfo;
	static int saveGameSlotNumber;

	static bool denyMultiSlot;
	static bool levelChange;
	static bool saveGameIsLoading;
	static bool saveGameWriting;

	static std::list<std::pair<int, int>>reusableMarkList;

	class DII_InstanceNameNotFoundException : protected std::exception {
	private:
		std::string err_msg;

	public:
		DII_InstanceNameNotFoundException(const char *msg) : err_msg(msg) {};
		~DII_InstanceNameNotFoundException() throw() {};
		const char *what() const throw() { return this->err_msg.c_str(); };
	};

};

#endif __DYN_ITEM_INST_H
