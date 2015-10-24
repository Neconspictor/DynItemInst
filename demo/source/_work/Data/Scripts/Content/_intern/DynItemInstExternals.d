// ***************************************************		
// Is used to specify whether the library is loaded.						
// ***************************************************
var int dii_Initialized;

INSTANCE  DII_DUMMY_ITEM(C_Item)  
{
	name 			=	"Don't use me for regular use...";
	description		=  "DynItemInst Dummy item";
	mainflag		= 0;
	flags 			= 0;
	
};

// *****************************************************************		
// Provides the version number of the DynItemInst_Ikarus library.
// Important: The version number is a float digit, that is returned 
// as an integer. Therefore you should use this function along with
// e.g. the float package of Ikarus.	
// *****************************************************************
FUNC INT DII_GetLibVersion() {
	var int adr;
	adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "DII_GetLibVersion");
	CALL_RetValIsFloat();
	CALL__cdecl(adr);
	CALL_RetValAsFloat();
};

// ****************************************************************
// Returns the expected version of the used DynItemInst_Ikarus.dll.
// The return value will be float stored in an integer.		
// ****************************************************************
FUNC INT _DII_GetExpectedLibVersion() {
	//expected lib version: 1.01
	var int integral;
	var int fraction;
	
	integral = mkf(1);
	fraction = divf(1, 100);
	return addf(integral, fraction);
};

// ************************************************************			
// Creates a new item. The item will have the instance id n0.				
// ************************************************************
FUNC C_ITEM DII_CreateNewItem (VAR INT n0) {
	if (!dii_Initialized) {
		return;
	};
	const int call = 0;
	var int ret;
	if (CALL_Begin(call)) {
		var int adr;
		adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "DII_CreateNewItem");
		CALL_IntParam(_@(n0));
		CALL_PutRetValTo(_@(ret));
		CALL__cdecl(adr);
		call = CALL_End();
	};
	MEM_PtrToInst(ret);
};

// *********************************************************************				
// Creates a new dynamic item instance (dii) on the base of the item n0.
// Objects having this new dii will have identic properties like n0.			
// *********************************************************************
FUNC INT DII_CreateNewInstance (VAR C_ITEM n0) {
	if (!dii_Initialized) {
		return 0;
	};
	const int call = 0;
	var int ptr;
	var int ret;
	ptr = MEM_InstToPtr(n0);
	if (CALL_Begin(call)) {
		var int adr;
		adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "DII_CreateNewInstance");
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
FUNC INT DII_IsDynamic(VAR C_ITEM n0) {
	if (!dii_Initialized) {
		return 0;
	};
	const int call = 0;
	var int ptr;
	var int ret;
	ptr = MEM_InstToPtr(n0);
	if (CALL_Begin(call)) {
		var int adr;
		adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "DII_IsDynamic");
		CALL_IntParam(_@(ptr));
		CALL_PutRetValTo(_@(ret));
		CALL__cdecl(adr);
		call = CALL_End();
	};
	return +ret;
};

// ***************************************************************	
// Deletes a provided item and removes it from the current world.			
// ***************************************************************
FUNC VOID DII_DeleteItem (VAR C_ITEM n0) {
	if (!dii_Initialized) {
		return;
	};
	const int call = 0;
	var int ptr;
	ptr = MEM_InstToPtr(n0);
	if (CALL_Begin(call)) {
		var int adr;
		adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "DII_DeleteItem");
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
FUNC DII_USER_DATA DII_GetUserData (VAR INT n0) {
	if (!dii_Initialized) {
		return;
	};
	const int call = 0;
	var int ret;
	if (CALL_Begin(call)) {
		var int adr;
		adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "DII_GetUserData");
		CALL_IntParam(_@(n0));
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
FUNC void DII_Init()
{
	var int expectedLibVersion;
	var int libVersion;
	expectedLibVersion = _DII_GetExpectedLibVersion();
	libVersion = DII_GetLibVersion();
	if (libVersion != expectedLibVersion) {
		MEM_MessageBox("DII_Init(): Library version doesn't conform to expected one! No initialization will be performed.", "Initialization error", 0);
		return;
	};
	
    var int adr;
    adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "Hook");
    CALL__stdcall(adr);
	dii_Initialized = true;
};