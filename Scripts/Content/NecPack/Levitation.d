// public
// Note: You can change the variables values but do not change their names - they are partially needed by NecPack library

const int LEVITATION_HOVER_DISTANCE = 50; 					// hover distance (in cm/s)
const int LEVITATION_SPEED_VERTICAL = 60; 					// up/down levitation speed (in cm/s)
const int LEVITATION_SPEED_FORWARD = 500; 					// forward levitation speed (in cm/s)
const int LEVITATION_SPEED_BACKWARD = 100; 					// backward levitation speed (in cm/s)
const int LEVITATION_SPEED_TURN = 90;						// turn speed (in degree angles/s)
const int LEVITATION_MAX_LIFETIME = 13000; 					// maximum lifetime of the levitation state (in ms)
const int LEVITATION_GRAVITY = 10;							// levitation gravity (in cm/s). Set it to 0 for no gravity.

const string LEVITATION_OVERLAY = "Humans_Levitate.mds";	// levitation overlay

// user control
const int LEVITATION_CONTROL_FORWARD = KEY_UPARROW; 		// forward movement KEY_UPARROW
const int LEVITATION_CONTROL_BACKWARD = KEY_DOWNARROW; 		// backward movement KEY_DOWNARROW
const int LEVITATION_CONTROL_LEFT = KEY_LEFTARROW; 			// left turn KEY_LEFTARROW
const int LEVITATION_CONTROL_RIGHT = KEY_RIGHTARROW; 		// right turn KEY_RIGHTARROW
const int LEVITATION_CONTROL_UP = KEY_Y; 			// up movement KEY_LBRACKET
const int LEVITATION_CONTROL_DOWN = KEY_X; 		// down movement KEY_APOSTROPHE
const int LEVITATION_CONTROL_TERMINATE = KEY_RETURN; 		// terminates levitation state KEY_RETURN

// private
var int _LEVITATION_barHandle; 								// lifetime bar of the levitation state
var int _LEVITATION_consumedLifetme; 						// spend lifetime of the levitation state (in ms)
var int _LEVITATION_IsActive; 								// indicates that the NecPack library should activate levitation state for hero


/**
 * Checks if the game is currently paused.
 * @return : TRUE if the game is paused otherwise FALSE.
 */
func int LEVITATION_IsGamePaused() {
	if (!NECPACK_Initialized) {
		MEM_Warn("LEVITATION_IsGamePaused: Library isn't initialized!");
        return FALSE;
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

/**
 * Initializes the levitation module.
 */
func void LEVITATION_Init() {
	
	if (!_LEVITATION_barHandle) {
		_LEVITATION_barHandle = Bar_Create(GothicBar@);
		Bar_Hide(_LEVITATION_barHandle);
	};
};

/**
 * Starts the levitation state.
 */
func void LEVITATION_Begin() {
	if (!NECPACK_Initialized) {
        MEM_Warn("LEVITATION_Begin: Library isn't initialized!");
        return;
    };
	
	_LEVITATION_IsActive = TRUE;
	Bar_Show(_LEVITATION_barHandle);
	Bar_SetPromille(_LEVITATION_barHandle, 1000); // fill lifetime
	Mdl_ApplyOverlayMds	(hero, LEVITATION_OVERLAY); 
	_LEVITATION_consumedLifetme = FLOATNULL;
	FF_ApplyOnce(LEVITATION_ConsumeLifetime);
};

/**
 * Ends the levitation state.
 */
func void LEVITATION_End() {
	if (!NECPACK_Initialized) {
        MEM_Warn("LEVITATION_End: Library isn't initialized!");
        return;
    };
	
	_LEVITATION_IsActive = FALSE;
	Bar_Hide(_LEVITATION_barHandle);
	Mdl_RemoveOverlayMDS(hero, "Humans_Levitate.mds");
	FF_Remove(LEVITATION_ConsumeLifetime);
};

/**
 * Toggles the levitation state.
 */
func void LEVITATION_Toggle() {
	if (!NECPACK_Initialized) {
        MEM_Warn("LEVITATION_Toggle: Library isn't initialized!");
        return;
    };
		
	if (_LEVITATION_IsActive) {
		LEVITATION_End();
	} else {
		LEVITATION_Begin();
	};
};

/**
 * Updates levitation lifetime for the current frame.
 */
func void LEVITATION_ConsumeLifetime() {

	// do nothing if game is currently paused.
	if (LEVITATION_IsGamePaused()) {return;};
	
	const int THOUSAND = 1000;
	
	// specifies the time (in seconds) it takes for the continous lifetime substraction 
	// to consume the whole levitation lifetime.
	const int CONTINOUS_LIFETIME_SUBSTRACTION_TIME = 120;

	// we add 1/CONTINOUS_LIFETIME_SUBSTRACTION_TIME to the invested time as a continous contribution.
	_LEVITATION_consumedLifetme = addf(_LEVITATION_consumedLifetme, 
		mulf(MEM_Timer.frameTimeFloat, fracf(LEVITATION_MAX_LIFETIME, CONTINOUS_LIFETIME_SUBSTRACTION_TIME * THOUSAND)));

	// if levitation up movement is active we substract more lifetime.
	if (MEM_KeyState(LEVITATION_CONTROL_UP) == KEY_HOLD) {
		_LEVITATION_consumedLifetme =  addf(_LEVITATION_consumedLifetme, MEM_Timer.frameTimeFloat);
	};
	
	// Update levitation lifetime bar
	var int barPromille; 
	barPromille = subf(mkf(LEVITATION_MAX_LIFETIME), _LEVITATION_consumedLifetme);
	barPromille = divf(barPromille, mkf(LEVITATION_MAX_LIFETIME));
	barPromille = mulf(barPromille, mkf(THOUSAND));
	barPromille = truncf(barPromille);

	if (barPromille < 0) {
		barPromille = 0;
	};
	
	Bar_SetPromille(_LEVITATION_barHandle, barPromille);
	
	// end levitation state if lifetime is consumed or the user exits on purpose
	if (MEM_KeyState(LEVITATION_CONTROL_TERMINATE) == KEY_PRESSED || barPromille == 0) {
		LEVITATION_End();
	};
};