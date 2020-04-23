
const int NEC_SILENT = FALSE;
const int _NEC_FLAGS = 0;

// **********************************************************************************
// Loads and inits the NEC library. If the loaded library version
// doesn't conform to the expected one, no initialization will be performed and a
// error message will be appear to the user in form of a message box.
// NOTE: This function has to be called in InitPerceptions() !
// **********************************************************************************

func void NEC_InitPerceptions(var int flags)
{
    var int expectedLibVersion;
    var int libVersion;
    var string msg;
    expectedLibVersion = _NEC_GetExpectedLibVersion();
    libVersion = NEC_GetLibVersion();

    //Library couldn't be loaded?
    if (!libVersion) {
        NEC_Init_Modules = false;
        if (!NEC_SILENT) {
            MEMINT_HandleError(zERR_TYPE_FATAL, ConcatStrings(NEC_relativeLibraryPath, " couldn't be loaded!"));
        } else {
            msg = ConcatStrings("neclib: ", NEC_relativeLibraryPath);
            msg = ConcatStrings(msg, " couldn't be loaded!");
            MEM_Error(msg);
        };
        return;
    };
    if (libVersion != expectedLibVersion) {
        msg = ConcatStrings("Expected lib version: ", toStringf(expectedLibVersion));
        msg = ConcatStrings(msg, " , loadded lib version: ");
        msg = ConcatStrings(msg, toStringf(libVersion));
        msg = ConcatStrings(msg, "; Library version doesn't conform to expected one! No initialization will be performed and neclib won't work as expected!");
        NEC_Init_Modules = false;
        if (!NEC_SILENT) {
            MEMINT_HandleError(zERR_TYPE_FATAL, msg);
        } else {
            MEM_Error(msg);
        };
        return;
    };

    var int adr;
    adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "Hook");
	CALL_IntParam(flags);
	CALL_PutRetValTo(_@(_NEC_FLAGS));
    CALL__cdecl(adr);
	
	if (flags != _NEC_FLAGS) {
		MEM_Error("neclib: Not all modules have been correctly hooked!");
	} else {
		MEM_Info("neclib: All modules have been hooked.");
	};
	
	// NOTE: the other flags are set in NEC_INIT_GLOBAL
	NEC_Init_Modules = _NEC_FLAGS & NEC_DII;
	
};

// **********************************************************************************
// Initialization function for NEC that has to be called in INIT_GLOBAL.
// **********************************************************************************
func void NEC_INIT_GLOBAL() {

	if (_NEC_FLAGS & NEC_LEVITATION) {
		NEC_Init_Modules = NEC_Init_Modules | NEC_LEVITATION;
		LEVITATION_Init();
	};
	
	if (_NEC_FLAGS & NEC_TELEKINESIS) {
		NEC_Init_Modules = NEC_Init_Modules | NEC_TELEKINESIS;
		TELEKINESIS_Init();
	};
	
	MEM_Info("neclib: NEC_INIT_GLOBAL: initialized.");
};