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

#pragma once

#include "DynInstance.h"
#include <HookManager.h>
#include <map>
#include <queue>
#include "oCMobContainer.h"
#include "zCPar_SymbolTable.h"
#include <functional>
#include "api/g2/ocnpcinventory.h"
#include <unordered_map>
#include <memory>

/**
 * This class is responsible for managing DynInstance and AdditMemory objects.
 */

typedef DWORD (__thiscall* ZCPAR_SYMBOL_CONSTRUCTOR)(void* thizz);
static ZCPAR_SYMBOL_CONSTRUCTOR zCPar_SymbolConstructor = (ZCPAR_SYMBOL_CONSTRUCTOR)0x007A1690;

class ObjectManager {
public:

	/**
	 * A structure for storing relevant zCPar_Symbol information. Additionally it  provides 
	 * with the container field a field for associating a new instance.
	 */
	struct ParserInfo
	{
		const char* newSymbolName;
		const char* oldSymbolName;
		int bitfield;
		DynInstance* container;
	};
public:

	/**
	 * Creates a new object manager.
	 */
	ObjectManager();

	/**
	 * Sets up a proxy from a source instance to a target instance.
	 */
	bool addProxy(const zSTRING& sourceInstance, const zSTRING& targetInstance);

	bool isProxied(const zSTRING& instanceName) const;

	int getUnProxiedInstanceID(const zSTRING& instanceName) const;

	/**
	 * Removes a proxy.
	 */
	void removeProxy(const zSTRING& sourceInstance);

	/**
	 * Resolves proxying of an instance id.
	 */
	int resolveProxying(int instanceID) const;

	/**
	 * Resolves proxying for symbol names.
	 */
	const char* resolveProxying(const zSTRING& symbolName) const;

	/**
	 * \return the current instance of this class.
	 */
	static ObjectManager* getObjectManager();

	static void release();

	/**
	 * Calls oCItem::InitByScript(int, int), which initializes the given oCItem by its instance id.
	 * \param item The item to be initialized
	 * \param inst The instance id the item should be initialized with
	 * \param amount The amount of instances the item should have.
	 */
	static void __thiscall oCItemInitByScript (oCItem* item, int inst, int amount);

	/**
	 * \return The list of all oCMobContainers the current oCGame contains.
	 */
	static std::list<oCMobContainer*> getMobContainers();

	/**
	 * \return true if a new instance could be created. Otherwise false.
	 */
	bool createNewInstanceWithoutExistingId(oCItem* item, int key, const std::string& instanceName);

	//void createNewInstanceForExistingId(oCItem* item, int instanceId);
	/**
			 * Creates a new oCItem instance which will be initialized with the members of the provided oCItem.
			 * \param item The item to use for instance creation.
			 * \return The instance id respectively the index of the new created zCPar_Symbol. Zero is returned if the instance couldn't be created.
			 */
	int createNewInstanceId(oCItem* item, const std::string& instanceName);

	/**
	 * Deletes a DII by its parser symbol table index.
	 * \param parserSymbolIndex The parser symbol table index referring to the DII.
	 * \throws std::invalid_argument If the parserSymbolIndex doesn't referr to a valid DII.
	 */
	void deleteDII(int parserSymbolIndex);

	/**
	 * Assigns a given Item to a given instance id but only if the Item wasn't assigned to one id already.
	 */
	void registerInstance(int instanceIdParserSymbolIndex, std::unique_ptr<DynInstance> item);

	/**
	 * Assigns an oCItem to an specified instance id. True will be returned if the assignment
	 * was successful, i.e. the instance id was found in the map of all registered instance ids.
	 * After assignment the item will be initialized with its new instance id.
	 * \param item The item which should be assigned
	 * \param id The instance id the item should be assigned to.
	 * \return Was the assignment successful?
	 */
	bool assignInstanceId(oCItem* item, int instanceIdParserSymbolIndex);
	
	
	static void oCItemOperatorDelete(oCItem* item);

	static bool assignInstanceId2(oCItem* item, int instanceIdParserSymbolIndex);

	/**
	 * Provides the instance id of a given oCItem.
	 * \param item The item to get the dynamic instance from.
	 * \return The dynamic instance id of the given item. If the oCItem has no dynamic instance, 
	 * NULL will be returned.
	 */
	int getDynInstanceId(oCItem* item);

	/**
	 * Sets the instance id of a given oCItem. If the oCItem has no dynamic instance, 
	 * no changes will be performed.
	 * \param item The item, the instance id of which should be set.
	 * \param The dynamic instance id the item should be assigned to.
	 */
	void setDynInstanceId(oCItem* item, int instanceIdParserSymbolIndex);

	/**
	 * \param instanceId The instance id to get the dynamic instance from.
	 * \return the Item which is associated with the given instance id. NULL will be returned
	 * if no connection could be found.
	 */
	DynInstance* getInstanceItem(int instanceIdParserSymbolIndex);

	/**
	 * Stores all new created instances at a directory described by 'directoryPath'. If the provided 
	 * directory path doesn't exists, the directory structure will be created.
	 * The Savegame will have the name defined by 'filename'.
	 */
	void saveNewInstances(char* directoryPath, char* filename);

	/**
	 * Loads new instances from a file defined by filePath.
	 * \param filePath The path to the file to be loaded.
	 */
	void loadNewInstances(char* filePath);

	/**
	 * Removes all new created instances and resets internal state.
	 */
	void releaseInstances();

	/**
	 * Sets the parent instance id for a given instance id.
	 * For definition of parent instance id see: \ref symIndInst
	 * \param instanceId The instance id of which the parent instance id should be set
	 * \param value The the parent instance id to be set
	 */
	void setPrototypeSymbolName(int instanceParserSymbolID, const std::string& parentInstanceSymbolName);

	/**
	 * Provides the parent instance id for a given instance id.
	 * For definition of parent instance id see: \ref symIndInst
	 * \param instanceId The instance id of which the parent instance id should be returned.
	 * \return the parent instance id of the given instance id.
	 */
	const std::string& getPrototypeSymbolName(int instanceParserSymbolID);

	/**
	 * Sets the instance id for a given oCItem.
	 * For definition of an instance id see: \ref symIndInst
	 * \param item The item of which the instance id should be set.
	 * \param instanceId The new instance id
	 */
	static void setInstanceId(oCItem* item, int instanceParserSymbolID);

	/**
	 * Provides the instance id for a given oCItem.
	 * For definition of an instance id see: \ref symIndInst
	 * \param item The item to get the instane id from.
	 * \return The instance id of the given item
	 */
	static int getInstanceId(oCItem& item);

	/**
	 * Provides the directory path of a savegame folder with a given slot number.
	 * \param saveGameSlotNumber The slot number to get the directory path from.
	 * \return The directory path from the savegame directory having the provided slot number.
	 */
	static std::string getSaveGameDirectoryPath(int saveGameSlotNumber);

	/**
	 * \return The directory path of the savegame folder named 'current'. 
	 */
	static std::string getCurrentDirectoryPath();

	/**
	 * Inits a given oCItem with a new instance id. The return value indicates if initialization
	 * was successful.
	 * \param item The item to initialized with a dynamic instance.
	 * \param index The zCPar_Symbol index of the dynamic instance. For more information about
	 * the connection between symbol indices and instance ids see: \ref symIndInst
	 * \return Was the initialization successful?
	 */
	bool InitItemWithDynInstance(oCItem* item, int index);

	/**
	 * Checks whether the given oCItem has a dynamic instance.
	 * \param item The item to check
	 * \return Has the given item a dynamic instance?
	 */
	bool isAssignedToDII(oCItem* item);

	/**
	 * Provides the zCPar_Symbol with index 'index' if the provided index refers to a dynamic instance.
	 * Otherwise NULL will be returned.
	 * For more information between the connection of parser symbols and dynamic instances see: \ref symIndInst
	 * \param index The index of the parser symbol.
	 * \return The parser symbol of the given index.
	 */
	zCPar_Symbol* getSymbolByIndex(int parserSymbolID);

	/**
	 * Provides the zCPar_Symbol with name  'symbolName'. Symbols stored by this object manager 
	 * represent dynamic instances. If no associated parser symbol could be found, 
	 * NULL will be returned.
	 * For more information on the connection between parser symbols and dynamic instances see: \ref symIndInst
	 * \param name The name of the parser symbol.
	 * \return The parser symbol of the given index.
	 */
	zCPar_Symbol* getSymbolByName(const zSTRING& symbolName);

	/**
	 * Provides the index of an given symbol name if an associated zCPar_Symbol is found.
	 * Otherwise NULL will be returned.
	 * For more information on the connection between parser symbols and dynamic instances see: \ref symIndInst
	 * \param symbolName The name of the parser symbol.
	 * \return The index of the symbol with the provided symbol name.
	 */
	int getIndexByName(const zSTRING& symbolName);

	void updateIkarusSymbols();

	void callForAllItems(bool(*func)(void* obj, void* param, oCItem*), void* obj, void* param);
	void callForAllNpcItems(bool(*func)(void* obj, void* param, oCItem*), void* obj, void* param);
	void callForAllContainerItems(bool(*func)(void* obj, void* param, oCItem*), void* obj, void* param);
	void callForAllWorldItems(bool(*func)(void* obj, void* param, oCItem*), void* obj, void* param);

	static int * getParserInstanceCount();

	/**
	* Checks whether the specified oCItem is in the game world's list registered
	* \param item The oCItem to check
	* \return true if the item is added into the world
	*/
	static bool isItemInWorld(oCItem* item);

	/**
	 * Searches an oCItem by its instance id in the world, in all containers and all inventories.
	 * \param instanceId the instance id for searching the oCItem
	 * \return The first oCItem found
	 */
	oCItem* getItemByInstanceId(int instanceIdParserSymbolIndex);

	static void oCItemSaveInsertEffect(oCItem* item);
	static void oCItemSaveRemoveEffect(oCItem* item);
	bool isDynamicInstance(int instanceIdParserSymbolIndex);
	static int* getRefCounter(oCItem* item);


	static zCListSort<oCItem>* getInvItemByInstanceId(oCNpcInventory * inventory, int instanceIdParserSymbolIndex);
	
	int getSlotNumber(oCNpcInventory* inventory, oCItem* item);
	
	oCItem* searchItemInInvbyInstanzValue(oCNpcInventory* inventory, int searchValue);

	static g2ext_extended::zCPar_SymbolTable* zCParserGetSymbolTable(void* parser);

	/**
	* Inits an oCItem by its (previously) assigned (new) instance id. The return value indicates
	* if the initialization was successful. E.g. if no registered instance id was found, this method
	* will return false.
	* \param item The item to initialize
	* \return Was the initialization successful?
	*/
	bool initByNewInstanceId(oCItem* item);

private:

	struct ARCHIVE_HEADER {
	public:

		static constexpr float VERSION = 1.0f;
		static constexpr const char DESC[] = "ARCHIVE_VERSION";

		ARCHIVE_HEADER() {
			std::memcpy(desc, DESC, sizeof(DESC));
		}

		char desc[sizeof(DESC)];
		float version = VERSION;
	};

	// <int instanceId, Item* item>
	std::unordered_map<int, std::unique_ptr<DynInstance>> mNewInstanceMap;
	std::unordered_map<int, zCPar_Symbol*> mNewInstanceToSymbolMap;
	std::unordered_map<std::string, zCPar_Symbol*> mNameToSymbolMap;
	std::unordered_map<std::string, int> mNameToInstanceMap;

	std::map<int, int> mProxies;
	std::unordered_map<std::string, std::string> mProxiesNames;
	std::unordered_map<std::string, int> mUnresolvedNamesToInstances;

	std::stringstream mLogStream;

	static std::unique_ptr<ObjectManager> mInstance;
private:

	static zCPar_Symbol* createNewInstanceSymbol(const ParserInfo* old);
	bool addSymbolToSymbolTable(zCPar_Symbol* symbol);
	DynInstance * createNewInstanceItem(int instanceIdParserSymbolIndex);
	void updateContainerItem(const ObjectManager::ParserInfo* info);
	/**
	 * Logs the current status of the provided zCPar_Symbol pointer.
	 * \param sym The symbol to log.
	 */
	void logSymbolData(zCPar_Symbol* sym);

	/**
	 * Creates a zCPar_Symbol from a ParserInfo object and adds it to the parser.
	 * @return: The index of the new created zCPar_Symbol into the symbol table of the parser.
	 */
	int createParserSymbol(const ParserInfo& info);

	zCPar_Symbol * createNewInstanceSymbol(int instanceParserSymbolID, zCPar_Symbol * prototype, const std::string& symbolName) const;
};


class SlotInfo {
public:

	static constexpr const char* DII_SLOT_COUNT = "DII_SLOT_COUNT";
	static constexpr const char* DII_SLOTS = "DII_SLOTS";

	static int getSlotCount();
	static const zSTRING& getSlotName(int index);

};