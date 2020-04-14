// **********************************************************************************
// Loads and inits the library DynItemInst_Ikarus.DLL. If the loaded library version
// doesn't conform to the expected one, no initialization will be performed and a
// error message will be appear to the user in form of a message box.
// **********************************************************************************

func void NECPACK_InitPerceptions()
{
    var int expectedLibVersion;
    var int libVersion;
    var string msg;
    expectedLibVersion = _NECPACK_GetExpectedLibVersion();
    libVersion = NECPACK_GetLibVersion();

    //Library couldn't be loaded?
    if (!libVersion) {
        NECPACK_Initialized = false;
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
        NECPACK_Initialized = false;
        if (!NECPACK_SILENT) {
            MEMINT_HandleError(zERR_TYPE_FATAL, msg);
        } else {
            MEM_Error(msg);
        };
        return;
    };
	
	NECPACK_InitSlots();

    var int adr;
    adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "Hook");
    CALL__stdcall(adr);
	
    NECPACK_Initialized = true;
};

func void NECPACK_INIT_GLOBAL() {
	LEVITATION_Init();
};