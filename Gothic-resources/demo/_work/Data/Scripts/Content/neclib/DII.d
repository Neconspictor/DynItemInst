// ***********************************************************************					
// Represents additional memory for dynamic item instances (dii).
// This additional memory is created for each dii and can be freely used.						
// ***********************************************************************

INSTANCE DII_Item(C_Item);

// ************************************************************
// Provides the instance id for an instance given by its string representation.
// The instance id is an index into the parser symbol table.
// ************************************************************
FUNC INT DII_GetInstanceID(var string instanceName) {
	return MEM_GetSymbolIndex(instanceName);
};

// ************************************************************
// Convenient function for retrieving the name of an instance id
// ************************************************************
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
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_AddProxy: DII Module isn't initialized!");
        return FALSE;
    };
	
    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_AddProxy");
	
	CALL_zStringPtrParam(targetInstanceName);
	CALL_zStringPtrParam(sourceInstanceName);
	
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
	return +ret;
};


// *********************************************************************
// If an dii was updated (using DII_UpdateInstance), the items in the world do not have been changed.
// This function can be used for applying the changes to all items in the world (and containers, npcs)
// that have the specified instance id.
// @param instanceName : The name of the instance id.
// *********************************************************************
func void DII_ApplyInstanceChangesToAll(var string instanceName) {
	
	if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_ApplyInstanceChangesToAll: DII Module isn't initialized!");
        return;
    };
	
	var int adr; 
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_ApplyInstanceChangesToAll");
	CALL_zStringPtrParam(instanceName);
	CALL__cdecl(adr);
};


// *********************************************************************
// Removes a proxy.
// NOTE: I encourage you not to use it since a previously proxied instance 
// can lead to hard to track bugs if this function is not used properly.
// And usually it isn't needed. 
// *********************************************************************
FUNC void DII_RemoveProxy (var string sourceInstanceName) {
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_RemoveProxy: DII Module isn't initialized!");
        return;
    };
	
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_RemoveProxy");
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
	if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_CreateNewItem: DII Module isn't initialized!");
        return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
	var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_CreateNewItem");
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
FUNC STRING DII_CreateNewInstance (var c_item itm) {
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_CreateNewInstance: DII Module isn't initialized!");
        return "";
    };
	
    var int ptr; ptr = _@(itm);
    var int ret;

	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_CreateNewInstance");
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
FUNC INT DII_CreateNewInstanceStr (var c_item itm, var string instanceName) {
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_CreateNewInstanceStr: DII Module isn't initialized!");
        return FALSE;
    };

    var int ptr; ptr = _@(itm);
    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_CreateNewInstanceStr");
	
	CALL_zStringPtrParam(instanceName);
	CALL_IntParam(ptr);	
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
	return +ret;
};


// ***************************************************************
//  Deletes a dynamic item instance by its instance name.
// ***************************************************************
/*func void DII_DeleteDII (var string instanceName) {
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_DeleteDII: DII Module isn't initialized!");
        return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_DeleteDII");
	CALL_IntParam(instanceParserSymbolID);
	CALL__cdecl(adr);
};*/


// ***************************************************************
//  Removes an item from the current world and deletes it.
// ***************************************************************
func void DII_DeleteItem (VAR C_ITEM itm) {
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_DeleteItem: DII Module isn't initialized!");
        return;
    };

    var int ptr; ptr = _@(itm);
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_DeleteItem");
	CALL_IntParam(ptr);
	CALL__cdecl(adr);
};


// ************************************************************
// Checks whether a given item itm has a dynamic item instance.
// If the item is dynamic, TRUE (1) otherwise FALSE (0) will
// be returned.
// ************************************************************
FUNC INT DII_IsDynamic(var c_item itm) {
	if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_IsDynamic: DII Module isn't initialized!");
        return FALSE;
    };

	var int ptr; ptr = _@(itm);
	var int ret;

	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_IsDynamic");
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
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_IsInstanceDynamic: DII Module isn't initialized!");
        return FALSE;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_IsInstanceDynamic");
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
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_UpdateInstance: DII Module isn't initialized!");
        return FALSE;
    };
	
    var int ptr; ptr = _@(itm);
	var int ret;
    var int adr;
    adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_UpdateInstance");
	
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
    if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_GetUserData: DII Module isn't initialized!");
        MEM_NullToInst();
		return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_GetUserData");
	CALL_IntParam(instanceParserSymbolID);
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
	if (!ret) {
	 MEM_NullToInst();
	 return;
	};
		
    MEM_PtrToInst(ret);
	return;
};

// **********************************************************************
// Changes the instance of all items that have instance are assigned to 'sourceInstanceName'.
// @param sourceInstanceName : Items having this instance will be changed.
// @param targetInstanceName : the instance the items should assigned to.
// **********************************************************************
func void DII_ChangeInstanceForAll(var string sourceInstanceName, var string targetInstanceName) {
	
	if (!(NEC_Init_Modules & NEC_DII)) {
        MEM_Warn("neclib: DII_ChangeInstanceForAll: DII Module isn't initialized!");
        return;
    };
	
	var int adr;
    adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "DII_ChangeInstanceForAll");
	
	CALL_zStringPtrParam(targetInstanceName);
	CALL_zStringPtrParam(sourceInstanceName);
    CALL__cdecl(adr);
};