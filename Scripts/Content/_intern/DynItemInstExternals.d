// ***************************************************
// Is used to specify whether the library is loaded.
// ***************************************************



const int dii_Initialized = 0;
const int DII_SILENT = 0;
const string DII_relativeLibraryPath = ".\DII\LoA_DII.DLL";

// Callback function before oCNpc::EquipWeapon(oCItem*) is called.
// Important: The specified function must have exact two int params and must be of return type 'void' (no return value!).
const string OCNPC_EQUIP_CALLBACK_NAME = "oCNpcEquip_Callback";

//Callback function for zCParser::CreateInstance(int, void*); needs one int param
const string ZCPARSER_CREATE_INSTANCE_CALLBACK = "zCParserCreateInstance_Callback";

//Callback function for oCNpc::IsMunitionAvailable(oCItem*); needs two int params
const string OCNPC_ISMUNITIONAVAILABLE_CALLBACK_NAME = "oCNpcIsMunitionAvailable_Callback";

// near fight weapon placeholders
INSTANCE  DII_DUMMY_ITEM_NF_1(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Item";
    mainflag             =    ITEM_KAT_NF;
    flags                 =    ITEM_SWD;
};

INSTANCE  DII_DUMMY_ITEM_NF_2(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Item";
    mainflag             =    ITEM_KAT_NF;
    flags                 =    ITEM_SWD;
};

INSTANCE  DII_DUMMY_ITEM_NF_3(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Item";
    mainflag             =    ITEM_KAT_NF;
    flags                 =    ITEM_SWD;
};

// distance fight weapon placeholders
INSTANCE  DII_DUMMY_ITEM_FF_1(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Item";
    mainflag             =    ITEM_KAT_FF;
    flags                 =    ITEM_BOW;
};

INSTANCE  DII_DUMMY_ITEM_FF_2(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Item";
    mainflag             =    ITEM_KAT_FF;
    flags                 =    ITEM_BOW;
};

INSTANCE  DII_DUMMY_ITEM_FF_3(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Item";
    mainflag             =    ITEM_KAT_FF;
    flags                 =    ITEM_BOW;
};

// rune placeholders
INSTANCE  DII_DUMMY_ITEM_RUNE_1(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Rune Item";
    mainflag             =    ITEM_KAT_RUNE;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_RUNE_2(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Rune Item";
    mainflag             =    ITEM_KAT_RUNE;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_RUNE_3(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Rune Item";
    mainflag             =    ITEM_KAT_RUNE;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_RUNE_4(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Rune Item";
    mainflag             =    ITEM_KAT_RUNE;
    flags                 =    0;
};


INSTANCE  DII_DUMMY_ITEM_RUNE_5(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Rune Item";
    mainflag             =    ITEM_KAT_RUNE;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_RUNE_6(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Rune Item";
    mainflag             =    ITEM_KAT_RUNE;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_RUNE_7(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Rune Item";
    mainflag             =    ITEM_KAT_RUNE;
    flags                 =    0;
};

// other  placeholders
INSTANCE  DII_DUMMY_ITEM_OTHER_1(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Other Item";
    mainflag             =    0;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_OTHER_2(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Other Item";
    mainflag             =    0;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_OTHER_3(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Other Item";
    mainflag             =    0;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_OTHER_4(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Other Item";
    mainflag             =    0;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_OTHER_5(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Other Item";
    mainflag             =    0;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_OTHER_6(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Other Item";
    mainflag             =    0;
    flags                 =    0;
};

INSTANCE  DII_DUMMY_ITEM_OTHER_7(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Other Item";
    mainflag             =    0;
    flags                 =    0;
};


//placeholder for all dii items being not equipped
INSTANCE  DII_DUMMY_ITEM(C_Item)
{
    name             =    "Don't use me for regular use...";
    description        =  "DynItemInst Dummy Item";
    mainflag             =    0;
    flags                 =    0;
};

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
// Creates a new item. The item will have the instance id n0.
// ************************************************************
/*func void DII_CreateNewItem (var int itm, var int n0) {
    if (!dii_Initialized) {
        MEM_ERROR("DII_CreateNewItem: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    //var int symb; symb = MEM_ReadIntArray (currSymbolTableAddress, itm);//symb = itm;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_CreateNewItem");
		
		//DII library couldn't be found?
		if (!adr) {
			MEM_ERROR("DII_CreateNewItem: ADDRESS	WASN'T FOUND!");
		};
		
        CALL_IntParam(_@(n0));
        CALL_IntParam(_@(itm));
        CALL__cdecl(adr);
        call = CALL_End();
    };

    //var int symb; symb = MEM_ReadIntArray (currSymbolTableAddress, itm);
    //var int symb2; symb2 = MEM_ReadInt (symb + zCParSymbol_offset_offset);
    //Print(ConcatStrings("item address: ", IntToString(symb2)));
    //var int value; value = MEM_ReadInt(symb + zCParSymbol_offset_offset);
    //symb = MEM_ReadIntArray (currSymbolTableAddress, itm);
    //var int symb2; symb2 = MEM_ReadInt (symb + zCParSymbol_offset_offset);
    //Print(ConcatStrings("item address: ", IntToString(symb2)));
    //MEM_AssignInst(item, symb2);
    //Print(ConcatStrings("name: ", itm.name));
    //MEM_WriteInt (item + zCParSymbol_offset_offset, value);
};*/

FUNC C_ITEM DII_CreateNewItem (VAR INT n0) {
	if (!dii_Initialized) {
		MEM_Warn("DII_CreateNewItem: Library isn't initialized!");
		return;
	};
	const int call = 0;
	var int ret;
	if (CALL_Begin(call)) {
		var int adr;
		adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_CreateNewItem");
		CALL_IntParam(_@(n0));
		CALL_PutRetValTo(_@(ret));
		CALL__cdecl(adr);
		call = CALL_End();
	};
	MEM_PtrToInst(ret);
};

// ************************************************************
// Releases an item previously allocated with DII_CreateNewItem.
// ************************************************************
func void DII_ReleaseItem (var int itm) {
    if (!dii_Initialized) {
        MEM_Warn("DII_ReleaseItem: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_ReleaseItem");
        CALL_IntParam(_@(itm));
        CALL__cdecl(adr);
        call = CALL_End();
    };
	
};

// *********************************************************************
// Creates a new dynamic item instance (dii) on the base of the item n0.
// Objects having this new dii will have identic properties like n0.
// *********************************************************************
FUNC INT DII_CreateNewInstance (var c_item n0) {
    if (!dii_Initialized) {
        MEM_Warn("DII_CreateNewInstance: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ptr;
    var int ret;
    ptr = MEM_InstToPtr(n0);
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_CreateNewInstance");
        CALL_IntParam(_@(ptr));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};

// ************************************************************
// Checks whether a given item n0 has a dynamic item instance.
// If the item is dynamic, TRUE (1) otherwise FALSE (0) will
// be returned.
// ************************************************************
FUNC INT DII_IsDynamic(var c_item n0) {
	if (!dii_Initialized) {
		MEM_Warn("DII_IsDynamic: Library isn't initialized!");
		return 0;
	};
	const int call = 0;
	var int ptr;
	var int ret;
	ptr = MEM_InstToPtr(n0);
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

FUNC INT DII_IsInstanceDynamic(var int instanceId) {
    if (!dii_Initialized) {
        MEM_Warn("DII_IsInstanceDynamic: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_IsInstanceDynamic");
        CALL_IntParam(_@(instanceId));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};



func void DII_UpdateInstance(var c_item n0) {
    if (!dii_Initialized) {
        MEM_Warn("DII_UpdateInstance: Library isn't initialized!");
        return;
    };
    var int ptr;
    ptr = MEM_InstToPtr(n0);
    var int adr;
    adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_UpdateInstance");
    CALL_IntParam(_@(n0));
    CALL__cdecl(adr);
};


// ***************************************************************
//  Removes an item from the current world and deletes it.
// ***************************************************************
func void DII_DeleteItem (VAR C_ITEM n0) {
    if (!dii_Initialized) {
		MEM_Warn("DII_IsInstanceDynamic: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ptr;
    ptr = MEM_InstToPtr(n0);
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_DeleteItem");
        CALL_IntParam(_@(ptr));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

// **********************************************************************
// Provides access to the user data of a given dynamic item instance
// which has the instance id n0. If the given instance id isn't dynamic,
// a new DII_USER_DATA object will be returned.
// **********************************************************************

FUNC DII_USER_DATA DII_GetUserData (var int n0) {
    if (!dii_Initialized) {
        MEM_Warn("DII_GetUserData: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_GetUserData");
        CALL_IntParam(_@(n0));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    MEM_PtrToInst(ret);
};



/*
func void DII_DoStatistics() {
    if (!dii_Initialized) {
        return;
    };

    var int adr;
    adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "DII_DoStatistics");
    CALL__stdcall(adr);
};*/

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

FUNC int DII_AreChangesPerformed() {
    if (!dii_Initialized) {
        MEM_Warn("DII_AreChangesPerformed: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_AreChangesPerformed");
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};

func void DII_AssignInstanceId (var c_item itm, var int instanceId) {
    if (!dii_Initialized) {
        MEM_Warn("DII_AssignInstanceId: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    //var int symb; symb = MEM_ReadIntArray (currSymbolTableAddress, itm);//symb = itm;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_AssignInstanceId");
        CALL_IntParam(_@(instanceId));
        CALL_IntParam(_@(itm));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func void DII_MarkAsReusable(var int instanceId, var int previousId) {
    if (!dii_Initialized) {
        MEM_Warn("DII_MarkAsReusable: Library isn't initialized!");
        return;
    };
    
    const int call = 0;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_MarkAsReusable");
        CALL_IntParam(_@(previousId));
        CALL_IntParam(_@(instanceId));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};


func void DII_GetItemByInstanceId (var int itm, var int instanceId) {
    if (!dii_Initialized) {
		MEM_Warn("DII_GetItemByInstanceId: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_GetItemByInstanceId");
        CALL_IntParam(_@(instanceId));
        CALL_IntParam(_@(itm));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func void DII_ChangeItemsInstanceId (var int targetId, var int newId) {
    if (!dii_Initialized) {
		MEM_Warn("DII_ChangeItemsInstanceId: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "DII_ChangeItemsInstanceId");
        CALL_IntParam(_@(newId));
        CALL_IntParam(_@(targetId));
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