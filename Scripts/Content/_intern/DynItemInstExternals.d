// ***************************************************
// Is used to specify whether the library is loaded.
// ***************************************************



const int dii_Initialized = 0;
const int DII_SILENT = 0;
const string DII_relativeLibraryPath = ".\DII\DII.DLL";

// Callback function before oCNpc::EquipWeapon(oCItem*) is called.
// Important: The specified function must have exact two int params and must be of return type 'void' (no return value!).
const string OCNPC_EQUIP_CALLBACK_NAME = "oCNpcEquip_Callback";

//Callback function for zCParser::CreateInstance(int, void*); needs one int param
const string ZCPARSER_CREATE_INSTANCE_CALLBACK = "zCParserCreateInstance_Callback";

//Callback function for oCNpc::IsMunitionAvailable(oCItem*); needs two int params
const string OCNPC_ISMUNITIONAVAILABLE_CALLBACK_NAME = "oCNpcIsMunitionAvailable_Callback";

// *****************************************************************
// Provides the version number of the DynItemInst_Ikarus library.
// Important: The version number is a float digit, that is returned
// as an integer. Therefore you should use this function along with
// e.g. the float package of Ikarus.
// *****************************************************************
FUNC INT DII_GetLibVersion() {
    var int adr;
    adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_GetLibVersion");

    //DII library couldn't be found?
    if (!adr) {
        return 0;
    };

    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        CALL_RetValIsFloat();
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};

// ****************************************************************
// Returns the expected version of the used DynItemInst_Ikarus.dll.
// The return value will be float stored in an integer.
// ****************************************************************
FUNC INT _DII_GetExpectedLibVersion() {
    //expected lib version: 1.02
    var int integral;
    var int fraction;

    integral = mkf(1);
    fraction = divf(2, 100);
    return addf(integral, fraction);
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
	symb = _^(MEM_GetSymbolByIndex(symbolIndex));
	return symb.name;
};

// ************************************************************
// Creates a new item.
//
// ATTENTION: The item has to be destroyed by DII_DeleteItem.
// The item won't be stored into a savegame and is only valid for 
// the current session (until gothic is terminated).
// ************************************************************
FUNC C_ITEM DII_CreateNewItem (var string instanceName) {
	if (!dii_Initialized) {
		MEM_Warn("DII_CreateNewItem: Library isn't initialized!");
		return;
	};
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
	const int call = 0;
	var int ret;
	if (CALL_Begin(call)) {
		var int adr;
		adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_CreateNewItem");
		CALL_IntParam(_@(instanceParserSymbolID));
		CALL_PutRetValTo(_@(ret));
		CALL__cdecl(adr);
		call = CALL_End();
	};
	MEM_PtrToInst(ret);
};


// *********************************************************************
// Creates a new dynamic item instance (dii) on the base of the item itm.
// Objects having this new dii will have identic properties like itm.
// @return : The instance name of the newly created dii.
// *********************************************************************
FUNC STRING DII_CreateNewInstanceStr (var c_item itm) {
    if (!dii_Initialized) {
        MEM_Warn("DII_CreateNewInstanceStr: Library isn't initialized!");
        return "";
    };
    const int call = 0;
    var int ptr;
    var int ret;
    ptr = MEM_InstToPtr(itm);
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_CreateNewInstanceStr");
        CALL_IntParam(_@(ptr));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
	
	return MEM_ReadString(ret);
};


// *********************************************************************
// Creates a new dynamic item instance (dii) on the base of the item itm
// and an instance name for the new dii.
// Objects having this new dii will have identic properties like itm.
// @return : TRUE, if the instance was successfully created. Otherwise FALSE.
// *********************************************************************
FUNC INT DII_CreateNewInstanceStr2 (var c_item itm, var string instanceName) {
    if (!dii_Initialized) {
        MEM_Warn("DII_CreateNewInstanceStr: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ptr;
    var int ret;
	var zSTRING zStr;
	zStr = _^(_@s(instanceName));
    ptr = MEM_InstToPtr(itm);
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_CreateNewInstanceStr2");
		
		CALL_IntParam(_@(zStr.ptr));
		CALL_IntParam(_@(ptr));
        
		CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
	
	return +ret;
};


// ***************************************************************
//  Deletes a dynamic item instance by its instance name.
// ***************************************************************
func void DII_DeleteDII (var string instanceName) {
    if (!dii_Initialized) {
		MEM_Warn("DII_IsInstanceDynamic: Library isn't initialized!");
        return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    const int call = 0;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_DeleteDII");
        CALL_IntParam(_@(instanceParserSymbolID));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};


// ***************************************************************
//  Removes an item from the current world and deletes it.
// ***************************************************************
func void DII_DeleteItem (VAR C_ITEM itm) {
    if (!dii_Initialized) {
		MEM_Warn("DII_IsInstanceDynamic: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ptr;
    ptr = MEM_InstToPtr(itm);
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_DeleteItem");
        CALL_IntParam(_@(ptr));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};


// ************************************************************
// Checks whether a given item itm has a dynamic item instance.
// If the item is dynamic, TRUE (1) otherwise FALSE (0) will
// be returned.
// ************************************************************
FUNC INT DII_IsDynamic(var c_item itm) {
	if (!dii_Initialized) {
		MEM_Warn("DII_IsDynamic: Library isn't initialized!");
		return 0;
	};
	const int call = 0;
	var int ptr;
	var int ret;
	ptr = MEM_InstToPtr(itm);
	if (CALL_Begin(call)) {
		var int adr;
		adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_IsDynamic");
		CALL_IntParam(_@(ptr));
		CALL_PutRetValTo(_@(ret));
		CALL__cdecl(adr);
		call = CALL_End();
	};
	return +ret;
};

// ************************************************************
// Checks if an instance is a dynamic item instance.
// @param instanceName : the name of the instance.
// ************************************************************
FUNC INT DII_IsInstanceDynamic(var string instanceName) {
    if (!dii_Initialized) {
        MEM_Warn("DII_IsInstanceDynamic: Library isn't initialized!");
        return 0;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_IsInstanceDynamic");
        CALL_IntParam(_@(instanceParserSymbolID));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};


// ************************************************************
// Initializes the properties of a dynamic item instance (dii) with the properties of an item.
// @param instanceName : the instance name of the dii.
// @param itm : The item to be used for initializing the dii.
// @return : True if the dii was successfully initialized.
// ************************************************************
func INT DII_UpdateInstance(var string instanceName, var c_item itm) {
    if (!dii_Initialized) {
        MEM_Warn("DII_SyncDII: Library isn't initialized!");
        return FALSE;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    var int ptr;
    ptr = MEM_InstToPtr(itm);
	var int ret;
    var int adr;
    adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_UpdateInstance");
	
	CALL_IntParam(ptr);
	CALL_IntParam(instanceParserSymbolID);
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
    if (!dii_Initialized) {
        MEM_Warn("DII_GetUserData: Library isn't initialized!");
        return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_GetUserData");
        CALL_IntParam(_@(instanceParserSymbolID));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    MEM_PtrToInst(ret);
};

// **********************************************************************************
// Loads and inits the library DynItemInst_Ikarus.DLL. If the loaded library version
// doesn't conform to the expected one, no initialization will be performed and a
// error message will be appear to the user in form of a message box.
// **********************************************************************************

func void DII_Init()
{
    var int expectedLibVersion;
    var int libVersion;
    var string msg;
    expectedLibVersion = _DII_GetExpectedLibVersion();
    libVersion = DII_GetLibVersion();

    //Library couldn't be loaded?
    if (!libVersion) {
        dii_Initialized = false;
        if (!DII_SILENT) {
            MEMINT_HandleError(zERR_TYPE_FATAL, ConcatStrings(DII_relativeLibraryPath, " couldn't be loaded!"));
        } else {
            msg = ConcatStrings("DII_Init: ", DII_relativeLibraryPath);
            msg = ConcatStrings(msg, " couldn't be loaded!");
            MEM_Error(msg);
        };
        return;
    };
    if (libVersion != expectedLibVersion) {
        msg = ConcatStrings("Expected lib version: ", toStringf(expectedLibVersion));
        msg = ConcatStrings(msg, " , loadded lib version: ");
        msg = ConcatStrings(msg, toStringf(libVersion));
        msg = ConcatStrings(msg, "; Library version doesn't conform to expected one! No initialization will be performed and DII won't work!");
        dii_Initialized = false;
        if (!DII_SILENT) {
            MEMINT_HandleError(zERR_TYPE_FATAL, msg);
        } else {
            MEM_Error(msg);
        };
        return;
    };

    var int adr;
    adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "Hook");
    CALL__stdcall(adr);
    dii_Initialized = true;
};


func void DII_AssignInstanceId (var c_item itm, var string instanceName) {
    if (!dii_Initialized) {
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
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_AssignInstanceId");
        CALL_IntParam(_@(instanceParserSymbolID));
        CALL_IntParam(_@(ptr));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};


func void DII_GetItemByInstanceId (var int itm, var string instanceName) {
    if (!dii_Initialized) {
		MEM_Warn("DII_GetItemByInstanceId: Library isn't initialized!");
        return;
    };
	
	var int instanceParserSymbolID;
	instanceParserSymbolID = DII_GetInstanceID(instanceName);
	
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_GetItemByInstanceId");
        CALL_IntParam(_@(instanceParserSymbolID));
        CALL_IntParam(_@(itm));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func void DII_ChangeItemsInstanceId (var int sourceInstanceParserSymbolID, var int targetInstanceParserSymbolID) {
    if (!dii_Initialized) {
		MEM_Warn("DII_ChangeItemsInstanceId: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_ChangeItemsInstanceId");
        CALL_IntParam(_@(targetInstanceParserSymbolID));
        CALL_IntParam(_@(sourceInstanceParserSymbolID));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};


func void DII_SetHeroFocusMode(var int mode) {
    if (!dii_Initialized) {
        MEM_Warn("DII_SetHeroFocusMode: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_SetHeroFocusMode");
        CALL_IntParam(_@(mode));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func void DII_Test(var int itemPtr, var int mode) {
    if (!dii_Initialized) {
		MEM_Warn("DII_Test: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_Test");
        CALL_IntParam(_@(mode));
        CALL_IntParam(_@(itemPtr));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};


func void DII_TransformationTest(var int vobPtr) {
    if (!dii_Initialized) {
		MEM_Warn("DII_TransformationTest: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_TransformationTest");
        CALL_IntParam(_@(vobPtr));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func void DII_ToggleLevitation() {
	if (!dii_Initialized) {
        MEM_Warn("DII_ToggleLevitation: Library isn't initialized!");
        return;
    };
	
	const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_ToggleLevitation");
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func void DII_TelekineseTest() {
if (!dii_Initialized) {
        MEM_Warn("DII_TelekineseTest: Library isn't initialized!");
        return;
    };
	
	const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_TelekineseTest");
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func int DII_Npc_CanTalk(var C_NPC npc) {
if (!dii_Initialized) {
        MEM_Warn("DII_Npc_CanTalk: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_Npc_CanTalk");
		var int ptr; ptr = _@(npc);
		CALL_IntParam(_@(ptr));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};


func int DII_Telekinesis_createInterpolator(var int pVobPosition, var int pNpcPosition, var int upMoveAmount, var int speed) {
	if (!dii_Initialized) {
        MEM_Warn("DII_Telekinesis_createInterpolator: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_Telekinesis_createInterpolator");
		CALL_IntParam(_@(speed));
		CALL_IntParam(_@(upMoveAmount));
		CALL_IntParam(_@(pNpcPosition));
		CALL_IntParam(_@(pVobPosition));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};

func void DII_Telekinesis_GetInterpolatedVec(var int pTelekinesisInterpolator, var int pDestVec3) {
    if (!dii_Initialized) {
		MEM_Warn("DII_Telekinesis_GetInterpolatedVec: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_Telekinesis_GetInterpolatedVec");
		CALL_IntParam(_@(pDestVec3));
        CALL_IntParam(_@(pTelekinesisInterpolator));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func void DII_Telekinesis_deleteInterpolator(var int pTelekinesisInterpolator) {
    if (!dii_Initialized) {
		MEM_Warn("DII_Telekinesis_deleteInterpolator: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_Telekinesis_deleteInterpolator");
        CALL_IntParam(_@(pTelekinesisInterpolator));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

//void DII_Telekinesis_Interpolate(TelekinesisInterpolator* interpolatorPtr, oCItem* item);
func void DII_Telekinesis_Interpolate(var int pTelekinesisInterpolator, var int itemPtr) {
    if (!dii_Initialized) {
		MEM_Warn("DII_Telekinesis_Interpolate: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_Telekinesis_Interpolate");
        CALL_IntParam(_@(itemPtr));
		CALL_IntParam(_@(pTelekinesisInterpolator));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func int DII_Npc_CanSeeVob(var int pNpc, var int pVob) {
if (!dii_Initialized) {
		MEM_Warn("DII_Npc_CanSeeVob: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_Npc_CanSeeVob");
		CALL_IntParam(_@(pVob));
		CALL_IntParam(_@(pNpc));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;

};

//void DII_DrobVob(oCNpc* npc, zCVob* vob)
func void DII_DrobVob(var int pNpc, var int pVob) {
    if (!dii_Initialized) {
		MEM_Warn("DII_DrobVob: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_DrobVob");
        CALL_IntParam(_@(pVob));
		CALL_IntParam(_@(pNpc));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};