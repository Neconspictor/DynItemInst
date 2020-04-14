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
	
	DII_InitSlots();

    var int adr;
    adr = GetProcAddress (LoadLibrary (DII_relativeLibraryPath), "Hook");
    CALL__stdcall(adr);
	
    dii_Initialized = true;
};