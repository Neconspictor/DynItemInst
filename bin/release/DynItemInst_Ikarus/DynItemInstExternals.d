
// ***********************************************************************					
// Represents additional memory for dynamic item instances (dii).
// This additional memory is created for each dii and can be freely used.						
// ***********************************************************************
CLASS DII_USER_DATA
{
	VAR INT data	[100];
};

// ***************************************************					
// Is used to specify whether the library is loaded.						
// ***************************************************
var int dii_Initialized;


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

// ************************************************************					
// Creates a new dynamic item instance (dii) on the base of the item n0.
// Objects having this new dii will have identic attributes like n0.			
// ************************************************************
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

// ******************************************************************			
// Provides access to the user data of a given dynamic item instance 
// which has the instance id n0.			
// ******************************************************************
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

// *****************************************************************	
// Loads and inits the library DynItemInst_Ikarus.DLL			
// *****************************************************************
FUNC void DII_Init()
{
    var int adr;
    adr = GetProcAddress (LoadLibrary (".\DynItemInst_Ikarus\DynItemInst_Ikarus.DLL"), "Hook");
    CALL__stdcall(adr);
	dii_Initialized = true;
};