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

#ifndef __ObjectManager_H__
#define __ObjectManager_H__

#include "DynInstance.h"
#include <HookManager.h>
#include <map>
#include <queue>
#include "oCMobContainer.h"
#include "AdditMemory.h"

/**
 * This class is responsible for managing DynInstance and AdditMemory objects.
 */
class ObjectManager {
public:
	/**
	 * New instance ids will have numbers which will start at this number in ascending order.
	 */
	//static const int INSTANCE_BEGIN = 5000000;

	/**
	 * New additional memory for hero items will have keys with a value beginning at this constant.
	 */
	static const int HERO_ADDIT_BEGIN = 2000000000;

	/**
	* New additional memory for special items will have keys with a value beginning at this constant.
	*/
	static const int SPECIAL_ADDIT_BEGIN = 1500000000;

	/**
	 * This number will be used to mark zCPar_Symbols of new instances.
	 */
	static const int ZCPAR_SYMBOL_MARK_ID = -6666666;

	/**
	 * A structure for storing relevant zCPar_Symbol information. Additionally it  provides 
	 * with the container field a field for associating a new instance.
	 */
	struct ParserInfo
	{
		int newInstanceId;
		int oldInstanceId;
		std::string name;
		int bitfield;
		DynInstance* container;
	};
public:

	/**
	 * \return the current instance of this class.
	 */
	static ObjectManager* getObjectManager();

	/**
	 * Deletes the current object manager (if one exists) and removes all dynamic instances.
	 */
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
	std::list<oCMobContainer*>* getMobContainers();
	void changeKeyIfFreeIdAvailable(int* key, int indexCount);
	void createNewInstanceWithoutExistingId(oCItem* item, int key);

	void createNewInstanceForExistingId(oCItem* item, int instanceId);
	/**
			 * Creates a new oCItem instance which will be initialized with the members of the provided oCItem.
			 * \param item The item to use for instance creation.
			 * \return The instance id respectively the index of the new created zCPar_Symbol.
			 */
	int createNewInstanceId(oCItem* item);

	/**
	 * Assigns a given Item to a given instance id but only if the Item wasn't assigned to one id already.
	 */
	void createInstanceById(int id, DynInstance* item);

	/**
	 * Assigns an oCItem to an specified instance id. True will be returned if the assignment
	 * was successful, i.e. the instance id was found in the map of all registered instance ids.
	 * After assignment the item will be initialized with its new instance id.
	 * \param item The item which should be assigned
	 * \param id The instance id the item should be assigned to.
	 * \return Was the assignment successful?
	 */
	bool assignInstanceId(oCItem* item, int id);

	bool assignInstanceId2(oCItem* item, int id);

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
	void setDynInstanceId(oCItem* item, int id);

	/**
	 * \param instanceId The instance id to get the dynamic instance from.
	 * \return the Item which is associated with the given instance id. NULL will be returned
	 * if no connection could be found.
	 */
	DynInstance* getInstanceItem(int instanceId);

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
	void setParentInstanceId(int instanceId, int value);

	/**
	 * Provides the parent instance id for a given instance id.
	 * For definition of parent instance id see: \ref symIndInst
	 * \param instanceId The instance id of which the parent instance id should be returned.
	 * \return the parent instance id of the given instance id.
	 */
	int getParentInstanceId(int instanceId);

	/**
	 * Sets the instance id for a given oCItem.
	 * For definition of an instance id see: \ref symIndInst
	 * \param item The item of which the instance id should be set.
	 * \param instanceId The new instance id
	 */
	void setInstanceId(oCItem* item, int instanceId);

	/**
	 * Provides the instance id for a given oCItem.
	 * For definition of an instance id see: \ref symIndInst
	 * \param item The item to get the instane id from.
	 * \return The instance id of the given item
	 */
	int getInstanceId(oCItem& item);

	/**
	 * Provides the directory path of a savegame folder with a given slot number.
	 * \param saveGameSlotNumber The slot number to get the directory path from.
	 * \return The directory path from the savegame directory having the provided slot number.
	 */
	std::string getSaveGameDirectoryPath(int saveGameSlotNumber);

	/**
	 * \return The directory path of the savegame folder named 'current'. 
	 */
	std::string getCurrentDirectoryPath();

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
	bool IsModified(oCItem* item);

	/**
	 * Checks whether the given instance id is a dynamic one.
	 * \param instanceId The instance id to check
	 * \return Has the given item a dynamic instance?
	 */
	bool IsModified(int instanceId);

	/**
	 * Provides the zCPar_Symbol with index 'index' if the provided index refers to a dynamic instance.
	 * Otherwise NULL will be returned.
	 * For more information between the connection of parser symbols and dynamic instances see: \ref symIndInst
	 * \param index The index of the parser symbol.
	 * \return The parser symbol of the given index.
	 */
	zCPar_Symbol* getSymbolByIndex(int index);

	/**
	 * Provides the zCPar_Symbol with name  'symbolName'. Symbols stored by this object manager 
	 * represent dynamic instances. If no associated parser symbol could be found, 
	 * NULL will be returned.
	 * For more information on the connection between parser symbols and dynamic instances see: \ref symIndInst
	 * \param name The name of the parser symbol.
	 * \return The parser symbol of the given index.
	 */
	zCPar_Symbol* getSymbolByName(zSTRING symbolName);

	/**
	 * Provides the index of an given symbol name if an associated zCPar_Symbol is found.
	 * Otherwise NULL will be returned.
	 * For more information on the connection between parser symbols and dynamic instances see: \ref symIndInst
	 * \param symbolName The name of the parser symbol.
	 * \return The index of the symbol with the provided symbol name.
	 */
	int getIndexByName(zSTRING symbolName);

	/**
	 * creates an new AdditMemory and associates it to the specified oCItem. The arguments id specifies
	 * The instance id the additional memory should store and isHeroItem specifies whether the item is 
	 * contained in the player's inventory.
	 * \param item The item for which additional memory should be created.
	 * \param id The instance id the additional memory should store.
	 * \param isHeroItem Is the given item in the player's inventory?
	 */
	void createAdditionalMemory(oCItem* item, int id, bool isHeroItem = false, bool activeSpellItem = false, int spellKey = -1);

	/**
	 * Deletes AdditionalMemory on the base of its key.
	 * \param additId The key of the AdditMemory which should be deleted.
	 */
	void removeAdditionalMemory(int additId);

	/**
	 * Deletes all available additional memory.
	 */
	void removeAllAdditionalMemory();

	/**
	 * Deletes all elements of the provided AdditMemory list properly.
	 * \param list The list of AdditMemory which elements should be removed.
	 */
	void removeAdditList(std::list<AdditMemory*>* list);

	/**
	 * Provides additional memory for all items in the player's inventory and stores
	 * the result in the given AdditMemory list.
	 * \param list The list in which all AdditMemory objects of all items in the player's inventory
	 *  should be stored.
	 */
	void getHeroAddits(std::list<AdditMemory*>& list);

	/**
	 * Saves the given 'heroItemList' in the folder 'directoryPath' and names the file 'filename'.
	 * \param heroItemList list in which all AdditMemory objects of all items in the player's 
	 * inventory are stored.
	 * \param directoryPath The path to the savegame folder the savegame should be stored.
	 * \param filename The file name to which the additional memory should be stored.
	 */
	void saveHeroData(std::list<AdditMemory*> heroItemList, char* directoryPath, char* filename);

	/**
	 * Saves all items with a dynamic instance id into the folder 'directoryPath' and names the 
	 * created file 'filename'. 'heroItemSize' specifies the size of items with a dynamic instance id
	 * the player's inventory contains.
	 * \param heroitemSize The amount of additional memory objects of all items in the player's 
	 * inventory.
	 * \param directoryPath The path to the savegame folder the savegame should be stored.
	 * \param filename The file name to which the additional memory should be stored.
	 */
	void saveWorldObjects(int heroItemSize, char* directoryPath, char* filename);

	/**
	 * Loads all additional memory for world items from a specified filename.
	 * \param pathToFile The path to the file from which the additional memory of world objects 
	 * should to be loaded.
	 */
	void loadWorldObjects(char* pathToFile);

	/**
	 * Loads all additional memory for  items in the player's inventory from a specified filename.
	 * \param pathToFile The path to the file from which the additional memory of the player's items 
	 * should to be loaded.
	 */
	void loadHeroData(char* pathToFile);
	
	/**
	 * Provides the key for the additonal memory created fot the given oCItem.
	 * \param item The item to get the key from.
	 * \return The key to the additional memory. If no additional memory was
	 * created for the item, NULL will be returned.
	 */
	int getAdditId(oCItem& item);

	/**
	 * Provides additional memory on the base of its key.
	 * \param additId The key for the desired AdditMemory object.
	 * \return The additional memory bound to the given key. If no additional memory was bound
	 * to the key, NULL will be returned.
	 */
	AdditMemory* getAddit(int additId); 

	/**
	 * Provides additional memory of the given oCItem.
	 * \param item The item to get additional memoyr from.
	 * \return The additional memory bound to the given item. If no additional memory was
	 * found, NULL will be returned.
	 */
	AdditMemory* getAddit(oCItem& item);

	/**
	 * Responses the question if the given oCItem has additional memory.
	 * \param item The item to check
	 * \return Has the given item additional memory?
	 */
	bool hasAdditAssignment(oCItem& item);

	void updateIkarusSymbols();

	void callForAllItems(std::function<void(oCItem*)> func, oCItem** stopIfNotNullItem = nullptr);

	int getInstanceBegin();

	int * getParserInstanceCount();

	int getIdForSpecialPurposes();

	void markAsReusable(int instanceId, int previousId);

	/**
	* Checks whether the specified oCItem is in the game world's list registered
	* \param item The oCItem to check
	* \return true if the item is added into the world
	*/
	bool isItemInWorld(oCItem* item);

	/**
	 * Searches an oCItem by its instance id in the world, in all containers and all inventories.
	 * \param instanceId the instance id for searching the oCItem
	 * \return The first oCItem found
	 */
	oCItem* getItemByInstanceId(int instanceId);

	void oCItemSaveInsertEffect(oCItem* item);
	void oCItemSaveRemoveEffect(oCItem* item);
	bool isDynamicInstance(int instanceId);
private:

	/**
	 * Stores relevant information for signing instances.
	 */
	struct InstanceInfo {
		int id;
		int parentId;
	};

	/**
	 * singleton of this class
	 */
	static ObjectManager* instanz;

	// <int instanceId, Item* item>
	std::map<int, DynInstance*> instanceMap;

	std::queue<int> nextInstances;
	std::vector<ParserInfo> indexZCParSymbolNameMap;
	std::map<int, zCPar_Symbol*> newInstanceToSymbolMap;
	std::map<std::string, zCPar_Symbol*> nameToSymbolMap;
	std::map<std::string, int> nameToIndexMap;
	std::list<int> reusableInstances;

	// <int additMemoryId, AdditMemory* additMemory>
	std::map<int, AdditMemory*> keyToAdditMap;
	std::queue<int> nextAdditKeys;

	std::stringstream logStream;
	int ObjectManager::instanceBegin = -1;

private:

	/**
	 * Creates a new object manager.
	 */
	ObjectManager();

	/**
	 * Inits an oCItem by its (previously) assigned (new) instance id. The return value indicates
	 * if the initialization was successful. E.g. if no registered instance id was found, this method
	 * will return false.
	 * \param item The item to initialize
	 * \return Was the initialization successful?
	 */
	bool initByNewInstanceId(oCItem* item);

	zCPar_Symbol* createNewSymbol(ParserInfo* old);
	bool addSymbolToSymbolTable(zCPar_Symbol* symbol);
	DynInstance * createNewInstanceItem(int instanceId);
	void updateContainerItem(ObjectManager::ParserInfo* info);
	/**
	 * Logs the current status of the provided zCPar_Symbol pointer.
	 * \param sym The symbol to log.
	 */
	void logSymbolData(zCPar_Symbol* sym);

	/**
	 * Creates for all registered instances zCPar_Symbols and registers the latter.
	 */
	void createParserSymbols();

	zCPar_Symbol * createNewSymbol(int instanceId, zCPar_Symbol * old);

	/**
	 * Calculates a new key for an AdditMemory object. The argument isHeroItem specifies whether
	 * the key is used for an item of the inventory of the player's character.
	 * \param isHeroItem Should the key be used for an item in the player's inventory?
	 * \return The new created additional memory key. 
	 */
	int calcAdditKey(bool isHeroItem);
};

#endif __ObjectManager_H__