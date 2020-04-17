const int NECPACK_Initialized = 0;
const int NECPACK_SILENT = 0;
const string NECPACK_relativeLibraryPath = ".\NecPack\NecPack.DLL";

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
FUNC INT NECPACK_GetLibVersion() {
    var int adr;
    adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "NECPACK_GetLibVersion");

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
FUNC INT _NECPACK_GetExpectedLibVersion() {
    //expected lib version: 1.02
    var int integral;
    var int fraction;

    integral = mkf(1);
    fraction = divf(2, 100);
    return addf(integral, fraction);
};

//.text:006BCEF0 ; public: int __thiscall oCNpc::CanTalk(void)
func int oCNpcCanTalk(var C_NPC npc) {
    const int adr = 7065328;
    var int ret;
	CALL_PutRetValTo(_@(ret));
	CALL__thiscall(_@(npc), adr);
    return +ret;
};

//.text:00602930 ; public: void __thiscall zCVob::SetSleeping(int)
func void zCVobSetSleeping(var int czvobPtr, var int setToSleep) {
	const int adr = 6302000;
	CALL_IntParam(setToSleep);
	CALL__thiscall(czvobPtr, adr);
};


//.text:0061D190 ; public: void __thiscall zCVob::SetPhysicsEnabled(int)
func void zCVobSetPhysicsEnabled(var int czvobPtr, var int enablePhysics) {
	const int adr = 6410640;
	CALL_IntParam(enablePhysics);
	CALL__thiscall(czvobPtr, adr);
};