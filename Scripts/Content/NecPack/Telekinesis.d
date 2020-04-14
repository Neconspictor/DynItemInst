func int TELEKINESIS_CreateInterpolator(var int pVobPosition, var int pNpcPosition, var int upMoveAmount, var int speed) {
	if (!NECPACK_Initialized) {
        MEM_Warn("TELEKINESIS_CreateInterpolator: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "TELEKINESIS_CreateInterpolator");
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

func void TELEKINESIS_GetInterpolatedVec(var int pTelekinesisInterpolator, var int pDestVec3) {
    if (!NECPACK_Initialized) {
		MEM_Warn("TELEKINESIS_GetInterpolatedVec: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "TELEKINESIS_GetInterpolatedVec");
		CALL_IntParam(_@(pDestVec3));
        CALL_IntParam(_@(pTelekinesisInterpolator));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func void TELEKINESIS_DeleteInterpolator(var int pTelekinesisInterpolator) {
    if (!NECPACK_Initialized) {
		MEM_Warn("TELEKINESIS_DeleteInterpolator: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "TELEKINESIS_DeleteInterpolator");
        CALL_IntParam(_@(pTelekinesisInterpolator));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

//void TELEKINESIS_Interpolate(TelekinesisInterpolator* interpolatorPtr, oCItem* item);
func void TELEKINESIS_Interpolate(var int pTelekinesisInterpolator, var int itemPtr) {
    if (!NECPACK_Initialized) {
		MEM_Warn("TELEKINESIS_Interpolate: Library isn't initialized!");
        return;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "TELEKINESIS_Interpolate");
        CALL_IntParam(_@(itemPtr));
		CALL_IntParam(_@(pTelekinesisInterpolator));
        CALL__cdecl(adr);
        call = CALL_End();
    };
};

func int TELEKINESIS_Npc_CanSeeVob(var int pNpc, var int pVob) {
if (!NECPACK_Initialized) {
		MEM_Warn("TELEKINESIS_Npc_CanSeeVob: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "TELEKINESIS_Npc_CanSeeVob");
		CALL_IntParam(_@(pVob));
		CALL_IntParam(_@(pNpc));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;

};