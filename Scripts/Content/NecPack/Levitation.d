var int LEVITATION_IsActive;
const int LEVITATION_HOVER_DISTANCE = 40; // hover distance (in cm/s)
const int LEVITATION_SPEED_VERTICAL = 60; // up/down levitation speed (in cm/s)
const int LEVITATION_SPEED_FORWARD = 500; // forward levitation speed (in cm/s)
const int LEVITATION_SPEED_BACKWARD = 100; // backward levitation speed (in cm/s)
const int LEVITATION_MAX_UP_MOVEMENT = 13000; // max up movement (in ms)
const int LEVITATION_GRAVITY = 10; // levitation gravity (in cm/s)
var int LEVITATION_barHandle;
var int LEVITATION_startTime;
var int LEVITATION_investedTimeUpMovement;


func int LEVITATION_IsGamePaused() {
	if (!NECPACK_Initialized) {
		MEM_Warn("LEVITATION_IsGamePaused: Library isn't initialized!");
        return 0;
    };
    const int call = 0;
    var int ret;
    if (CALL_Begin(call)) {
        var int adr;
        adr = GetProcAddress (LoadLibrary (NECPACK_relativeLibraryPath), "LEVITATION_IsGamePaused");
        CALL_PutRetValTo(_@(ret));
        CALL__cdecl(adr);
        call = CALL_End();
    };
    return +ret;
};

func void LEVITATION_Init() {
	
	if (!LEVITATION_barHandle) {
		LEVITATION_barHandle = Bar_Create(GothicBar@);
		Bar_Hide(LEVITATION_barHandle);
	};
	
};


func void LEVITATION_Begin() {
	if (!NECPACK_Initialized) {
        MEM_Warn("LEVITATION_Begin: Library isn't initialized!");
        return;
    };
	
	LEVITATION_IsActive = TRUE;
	Bar_Show(LEVITATION_barHandle);
	Bar_SetPromille(LEVITATION_barHandle, 1000);
	Mdl_ApplyOverlayMds	(hero, "Humans_Levitate.mds"); 
	LEVITATION_investedTimeUpMovement = FLOATNULL;
	FF_ApplyOnce(LEVITATION_InvestUpMovement);
};

func void LEVITATION_End() {
	if (!NECPACK_Initialized) {
        MEM_Warn("LEVITATION_End: Library isn't initialized!");
        return;
    };
	
	LEVITATION_IsActive = FALSE;
	Bar_Hide(LEVITATION_barHandle);
	Mdl_RemoveOverlayMDS(hero, "Humans_Levitate.mds");
	FF_Remove(LEVITATION_InvestUpMovement);
};


func void LEVITATION_Toggle() {
	if (!NECPACK_Initialized) {
        MEM_Warn("LEVITATION_Toggle: Library isn't initialized!");
        return;
    };
		
	if (LEVITATION_IsActive) {
		LEVITATION_End();
	} else {
		LEVITATION_Begin();
	};
};

func void LEVITATION_InvestUpMovement() {

	// do nothing if game is currently paused.
	if (LEVITATION_IsGamePaused()) {return;};

	// we add 1/120 to the invested time as constant contribution.
	LEVITATION_investedTimeUpMovement = addf(LEVITATION_investedTimeUpMovement, 
		mulf(MEM_Timer.frameTimeFloat, fracf(LEVITATION_MAX_UP_MOVEMENT, 120 * 1000)));

	if (MEM_KeyState(KEY_LBRACKET) == KEY_HOLD) {
		LEVITATION_investedTimeUpMovement =  addf(LEVITATION_investedTimeUpMovement, MEM_Timer.frameTimeFloat);
	};
	
		var int barPromille; 
		barPromille = subf(mkf(LEVITATION_MAX_UP_MOVEMENT), LEVITATION_investedTimeUpMovement);
		barPromille = divf(barPromille, mkf(LEVITATION_MAX_UP_MOVEMENT));
		barPromille = mulf(barPromille, mkf(1000));
		barPromille = truncf(barPromille);
		//barPromille = NECPACK_convertFloatToInt(barPromille);


		if (barPromille < 0) {
			barPromille = 0;
		};
		
		//barPromille = castToIntf(barPromille);
		Bar_SetPromille(LEVITATION_barHandle, barPromille);
		//MEM_Warn(ConcatStrings("LEVITATION_InvestUpMovement: ", IntToString(barPromille)));
	
	if (MEM_KeyState(KEY_RETURN) == KEY_PRESSED || barPromille == 0) {
		LEVITATION_End();
	};
};