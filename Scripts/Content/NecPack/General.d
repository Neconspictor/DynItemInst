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

func int NECPACK_Npc_CanTalk(var C_NPC npc) {
if (!NECPACK_Initialized) {
        MEM_Warn("NECPACK_Npc_CanTalk: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "NECPACK_Npc_CanTalk");
		var int ptr; ptr = _@(npc);
		CALL_IntParam(_@(ptr));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};

//void NECPACK_DrobVob(oCNpc* npc, zCVob* vob)
func void NECPACK_DrobVob(var int pNpc, var int pVob) {
    if (!NECPACK_Initialized) {
		MEM_Warn("NECPACK_DrobVob: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "NECPACK_DrobVob");
        CALL_IntParam(_@(pVob));
		CALL_IntParam(_@(pNpc));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};