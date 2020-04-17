// ***********************************************************************					
// Represents additional memory for dynamic item instances (dii).
// This additional memory is created for each dii and can be freely used.						
// ***********************************************************************

/* 
 * This constants are used to determine by the DII-Library how DII_USER_DATA is defined.
 * Define as many integers and strings as you like, but keep in mind that integers have to be defined before
 * the string members. DII will load the members assuming that order!
 */
 
const int DII_USER_DATA_INTEGER_AMOUNT = 2;
const int DII_USER_DATA_STRING_AMOUNT = 5;

//indices used for magic weapons
const int MAGICWEAPON_ENCHANTEDWEAPON = 0;
const int MAGICWEAPON_TYPE = 1;

CLASS DII_USER_DATA
{
	// At first the integer members
	var int data [DII_USER_DATA_INTEGER_AMOUNT];
	
	//next the string members
	//string arrays don't work properly;
	var string magicWeaponNewDesc;
	var string magicWeaponOldDesc;
	var string magicWeaponNewEffect;
	var string magicWeaponOldEffect;
	var string magicWeaponOldInstanceName;
};

const int DII_SLOT_COUNT = 9;
const string DII_SLOTS[DII_SLOT_COUNT] = {
	"ZS_HELMET",
	"ZS_LEFTHAND",
	"ZS_LEFTARM",
	"ZS_RIGHTHAND",
	"ZS_CROSSBOW",
	"ZS_LONGSWORD",
	"ZS_SHIELD",
	"ZS_BOW",
	"ZS_SWORD"
};

// ************************************************************
// Provides the instance id for an instance given by its string representation.
// The instance id is an index into the parser symbol table.
// ************************************************************
FUNC INT DII_GetInstanceID(var string instanceName) {
	return MEM_GetSymbolIndex(instanceName);
};

FUNC STRING DII_GetSymbolName(var int symbolIndex) {
	var zCPar_Symbol symb;
	var int ptr; ptr = MEM_GetSymbolByIndex(symbolIndex);
	if (ptr == 0) {return "";};
	symb = _^(ptr);
	return symb.name;
};


// *********************************************************************
// Adds a new proxy. All newly created items with instance 'sourceInstance' will have instance 'targetInstance'
// after calling this function.
// @return : TRUE, if the proxy is successfully setup. Otherwise FALSE.
// *********************************************************************
FUNC INT DII_AddProxy (var string sourceInstanceName, var string targetInstanceName) {
    if (!NECPACK_Initialized) {
        MEM_Warn("DII_AddProxy: Library isn't initialized!");
        return FALSE;
    };
	
    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_AddProxy");
	
	CALL_zStringPtrParam(targetInstanceName);
	CALL_zStringPtrParam(sourceInstanceName);
	
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
	return +ret;
};


//DII_ApplyInstanceChangesToAll(desc.instanceName);
func void DII_ApplyInstanceChangesToAll(var string instanceName) {
	
	if (!NECPACK_Initialized) {
        MEM_Warn("DII_ApplyInstanceChangesToAll: Library isn't initialized!");
        return;
    };
	
	var int adr; 
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_ApplyInstanceChangesToAll");
	CALL_zStringPtrParam(instanceName);
	CALL__cdecl(adr);
};


// *********************************************************************
// Adds a new proxy. All newly created items with instance 'sourceInstance' will have instance 'targetInstance'
// after calling this function.
// @return : TRUE, if the proxy is successfully setup. Otherwise FALSE.
// *********************************************************************
FUNC void DII_RemoveProxy (var string sourceInstanceName) {
    if (!NECPACK_Initialized) {
        MEM_Warn("DII_RemoveProxy: Library isn't initialized!");
        return;
    };
	
	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_RemoveProxy");
	CALL_zStringPtrParam(sourceInstanceName);
	CALL__cdecl(adr);
};


// ************************************************************
// Creates a new item.
//
// ATTENTION: The item has to be destroyed by DII_DeleteItem.
// The item won't be stored into a savegame and is only valid for 
// the current session (until gothic is terminated).
// ************************************************************
FUNC C_ITEM DII_CreateNewItem (var string instanceName) {
	if (!NECPACK_Initialized) {
		MEM_Warn("DII_CreateNewItem: Library isn't initialized!");
		return;
	};
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
	var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_CreateNewItem");
	CALL_IntParam(instanceParserSymbolID);
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
	MEM_PtrToInst(ret);
};


// *********************************************************************
// Creates a new dynamic item instance (dii) on the base of the item itm.
// Objects having this new dii will have identic properties like itm.
// @return : The instance name of the newly created dii.
// *********************************************************************
FUNC STRING DII_CreateNewInstanceStr (var c_item itm) {
    if (!NECPACK_Initialized) {
        MEM_Warn("DII_CreateNewInstanceStr: Library isn't initialized!");
        return "";
    };
	
    var int ptr; ptr = _@(itm);
    var int ret;

	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_CreateNewInstanceStr");
	CALL_IntParam(ptr);
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
	return MEM_ReadString(ret);
};


// *********************************************************************
// Creates a new dynamic item instance (dii) on the base of the item itm
// and an instance name for the new dii.
// Objects having this new dii will have identic properties like itm.
// @return : TRUE, if the instance was successfully created. Otherwise FALSE.
// *********************************************************************
FUNC INT DII_CreateNewInstanceStr2 (var c_item itm, var string instanceName) {
    if (!NECPACK_Initialized) {
        MEM_Warn("DII_CreateNewInstanceStr: Library isn't initialized!");
        return 0;
    };

    var int ptr; ptr = _@(itm);
    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_CreateNewInstanceStr2");
	
	CALL_zStringPtrParam(instanceName);
	CALL_IntParam(ptr);	
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
	return +ret;
};


// ***************************************************************
//  Deletes a dynamic item instance by its instance name.
// ***************************************************************
func void DII_DeleteDII (var string instanceName) {
    if (!NECPACK_Initialized) {
		MEM_Warn("DII_DeleteDII: Library isn't initialized!");
        return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_DeleteDII");
	CALL_IntParam(instanceParserSymbolID);
	CALL__cdecl(adr);
};


// ***************************************************************
//  Removes an item from the current world and deletes it.
// ***************************************************************
func void DII_DeleteItem (VAR C_ITEM itm) {
    if (!NECPACK_Initialized) {
		MEM_Warn("DII_DeleteItem: Library isn't initialized!");
        return;
    };

    var int ptr; ptr = _@(itm);
	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_DeleteItem");
	CALL_IntParam(ptr);
	CALL__cdecl(adr);
};


// ************************************************************
// Checks whether a given item itm has a dynamic item instance.
// If the item is dynamic, TRUE (1) otherwise FALSE (0) will
// be returned.
// ************************************************************
FUNC INT DII_IsDynamic(var c_item itm) {
	if (!NECPACK_Initialized) {
		MEM_Warn("DII_IsDynamic: Library isn't initialized!");
		return 0;
	};

	var int ptr; ptr = _@(itm);
	var int ret;

	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_IsDynamic");
	CALL_IntParam(ptr);
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);

	return +ret;
};

// ************************************************************
// Checks if an instance is a dynamic item instance.
// @param instanceName : the name of the instance.
// ************************************************************
FUNC INT DII_IsInstanceDynamic(var string instanceName) {
    if (!NECPACK_Initialized) {
        MEM_Warn("DII_IsInstanceDynamic: Library isn't initialized!");
        return 0;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_IsInstanceDynamic");
	CALL_IntParam(instanceParserSymbolID);
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
    return +ret;
};


// ************************************************************
// Initializes the properties of a dynamic item instance (dii) with the properties of an item.
// @param instanceName : the instance name of the dii.
// @param itm : The item to be used for initializing the dii.
// @return : True if the dii was successfully initialized.
// ************************************************************
func INT DII_UpdateInstance(var string instanceName, var c_item itm) {
    if (!NECPACK_Initialized) {
        MEM_Warn("NECPACK_SyncDII: Library isn't initialized!");
        return FALSE;
    };
	
    var int ptr; ptr = _@(itm);
	var int ret;
    var int adr;
    adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_UpdateInstance");
	
	CALL_IntParam(ptr);
	CALL_zStringPtrParam(instanceName);
	CALL_PutRetValTo(_@(ret));
    CALL__cdecl(adr);
	
	return +ret;
};


// **********************************************************************
// Provides access to the user data of a given dynamic item instance
// which has the instance name instanceName. If the given instanceName isn't a dii,
// th null instance will be returned.
// **********************************************************************

FUNC DII_USER_DATA DII_GetUserData (var string instanceName) {
    if (!NECPACK_Initialized) {
        MEM_Warn("DII_GetUserData: Library isn't initialized!");
        return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_GetUserData");
	CALL_IntParam(instanceParserSymbolID);
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
		
    MEM_PtrToInst(ret);
};

/*func void DII_AssignInstanceId (var c_item itm, var string instanceName) {
    if (!NECPACK_Initialized) {
        MEM_Warn("DII_AssignInstanceId: Library isn't initialized!");
        return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
	var int ptr;
    ptr = MEM_InstToPtr(itm);
	
    const int call = 0;
    var int ret;
    //var int symb; symb = MEM_ReadIntArray (currSymbolTableAddress, itm);//symb = itm;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_AssignInstanceId");
        CALL_IntParam(_@(instanceParserSymbolID));
        CALL_IntParam(_@(ptr));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};*/


func void DII_ChangeItemsInstance(var string sourceInstanceName, var string targetInstanceName) {
	
	if (!NECPACK_Initialized) {
		MEM_Warn("DII_ChangeItemsInstance: Library isn't initialized!");
        return;
    };
	
	var int adr;
    adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "DII_ChangeItemsInstance");
	
	CALL_zStringPtrParam(targetInstanceName);
	CALL_zStringPtrParam(sourceInstanceName);
    CALL__cdecl(adr);
};