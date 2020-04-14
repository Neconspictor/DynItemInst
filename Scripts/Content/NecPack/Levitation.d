var int DII_LevitationIsActive;
const int LEVITATION_HOVER_DISTANCE = 40; // hover distance (in cm/s)
const int LEVITATION_SPEED_VERTICAL = 60; // up/down levitation speed (in cm/s)
const int LEVITATION_SPEED_FORWARD = 500; // forward levitation speed (in cm/s)
const int LEVITATION_SPEED_BACKWARD = 100; // backward levitation speed (in cm/s)
const int LEVITATION_MAX_UP_MOVEMENT = 13000; // max up movement (in ms)
const int LEVITATION_GRAVITY = 10; // levitation gravity (in cm/s)
var int levitationBarHandle;
var int levitationStartTime;
var int levitationInvestedTimeUpMovement;

func void initLevitation() {
	
	if (!levitationBarHandle) {
		levitationBarHandle = Bar_Create(GothicBar@);
		Bar_Hide(levitationBarHandle);
	};
	
};

func void DII_EndLevitation() {
	if (!dii_Initialized) {
        MEM_Warn("DII_EndLevitation: Library isn't initialized!");
        return;
    };
	
	DII_LevitationIsActive = FALSE;
	Bar_Hide(levitationBarHandle);
	Mdl_RemoveOverlayMDS(hero, "Humans_Levitate.mds");
	FF_Remove(investUpMovement);
};

func void investUpMovement() {

	// we add 1/120 to the invested time as constant contribution.
	levitationInvestedTimeUpMovement = addf(levitationInvestedTimeUpMovement, 
		mulf(MEM_Timer.frameTimeFloat, fracf(LEVITATION_MAX_UP_MOVEMENT, 120 * 1000)));

	if (MEM_KeyState(KEY_LBRACKET) == KEY_HOLD) {
		levitationInvestedTimeUpMovement =  addf(levitationInvestedTimeUpMovement, MEM_Timer.frameTimeFloat);
	};
	
		var int barPromille; 
		barPromille = subf(mkf(LEVITATION_MAX_UP_MOVEMENT), levitationInvestedTimeUpMovement);
		barPromille = divf(barPromille, mkf(LEVITATION_MAX_UP_MOVEMENT));
		barPromille = mulf(barPromille, mkf(1000));
		barPromille = truncf(barPromille);
		//barPromille = DII_convertFloatToInt(barPromille);


		if (barPromille < 0) {
			barPromille = 0;
		};
		
		//barPromille = castToIntf(barPromille);
		Bar_SetPromille(levitationBarHandle, barPromille);
		//MEM_Warn(ConcatStrings("investUpMovement: ", IntToString(barPromille)));
	
	if (MEM_KeyState(KEY_RETURN) == KEY_PRESSED || barPromille == 0) {
		DII_EndLevitation();
	};
};




func void DII_ToggleLevitation() {
	if (!dii_Initialized) {
        MEM_Warn("DII_ToggleLevitation: Library isn't initialized!");
        return;
    };
	
	DII_LevitationIsActive = !DII_LevitationIsActive;
	
	if (DII_LevitationIsActive) {
		Bar_Show(levitationBarHandle);
		Bar_SetPromille(levitationBarHandle, 1000);
		Mdl_ApplyOverlayMds	(hero, "Humans_Levitate.mds"); 
		levitationInvestedTimeUpMovement = FLOATNULL;
		FF_ApplyOnce(investUpMovement);
		
	} else {
		Bar_Hide(levitationBarHandle);
		Mdl_RemoveOverlayMDS(hero, "Humans_Levitate.mds");
		FF_Remove(investUpMovement);
	};
};