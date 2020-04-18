

const int _NEC_FLAGS = 0;

// **********************************************************************************
// Loads and inits the NecPack library. If the loaded library version
// doesn't conform to the expected one, no initialization will be performed and a
// error message will be appear to the user in form of a message box.
// NOTE: This function has to be called in InitPerceptions() !
// **********************************************************************************

func void NECPACK_InitPerceptions(var int flags)
{
    var int expectedLibVersion;
    var int libVersion;
    var string msg;
    expectedLibVersion = _NECPACK_GetExpectedLibVersion();
    libVersion = NECPACK_GetLibVersion();

    //Library couldn't be loaded?
    if (!libVersion) {
        NEC_Init_Modules = false;
        if (!NECPACK_SILENT) {
            MEMINT_HandleError(zERR_TYPE_FATAL, ConcatStrings(NECPACK_relativeLibraryPath, " couldn't be loaded!"));
        } else {
            msg = ConcatStrings("NECPACK_Init: ", NECPACK_relativeLibraryPath);
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
        NEC_Init_Modules = false;
        if (!NECPACK_SILENT) {
            MEMINT_HandleError(zERR_TYPE_FATAL, msg);
        } else {
            MEM_Error(msg);
        };
        return;
    };

    var int adr;
    adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "Hook");
	CALL_IntParam(flags);
	CALL_PutRetValTo(_@(_NEC_FLAGS));
    CALL__cdecl(adr);
	
	// NOTE: the other flags are set in NECPACK_INIT_GLOBAL
	NEC_Init_Modules = _NEC_FLAGS & NEC_DII;
	
};

// **********************************************************************************
// Initialization function for NecPack that has to be called in INIT_GLOBAL.
// **********************************************************************************
func void NECPACK_INIT_GLOBAL() {

	if (_NEC_FLAGS & NEC_LEVITATION) {
		NEC_Init_Modules = NEC_Init_Modules | NEC_LEVITATION;
		LEVITATION_Init();
	};
	
	if (_NEC_FLAGS & NEC_TELEKINESIS) {
		NEC_Init_Modules = NEC_Init_Modules | NEC_TELEKINESIS;
		TELEKINESIS_Init();
	};
};