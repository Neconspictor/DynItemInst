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

func int TELEKINESIS_IsVobSeeable(var int pNpc, var int pVob) {
	if (!NECPACK_Initialized) {
		MEM_Warn("TELEKINESIS_IsVobSeeable: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "TELEKINESIS_IsVobSeeable");
		CALL_IntParam(_@(pVob));
		CALL_IntParam(_@(pNpc));
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};

//.text:00732B60 ; public: void __thiscall oCNpc::SetFocusVob(class zCVob *)


func void oCNpcSetFocusVob(var int poCNpc, var int pVob) {
    const int call = 0;
	const int addr = 7547744;//00732B60
    if(CALL_Begin(call)) {
        CALL_IntParam(MEM_GetIntAddress(pVob));
        CALL__thiscall(MEM_GetIntAddress(poCNpc), addr);
        call = CALL_End();
    };
};

func void TELEKINESIS_AdjustHeroFocusVob() {
	var oCNpc oCSelf; oCSelf = _^(_@(hero));
	if (oCSelf.focus_vob == 0) {return;};
	
	var oCItem oCFocus; oCFocus = _^(oCSelf.focus_vob);
	if (!Hlp_IsValidItem(oCFocus)) {return;};
	
	var int canSee; canSee = TELEKINESIS_IsVobSeeable(_@(hero), oCSelf.focus_vob);
	
	if (!canSee) {
		//MEM_Warn("Cannot see focus vob!!!");
		oCSelf.focus_vob = 0;
		//oCNpcSetFocusVob(_@(oCSelf), 0);
	};
};


func void TELEKINESIS_Init() {
	//FF_ApplyOnce(TELEKINESIS_AdjustHeroFocusVob);
};