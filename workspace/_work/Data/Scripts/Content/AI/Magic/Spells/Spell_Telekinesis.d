// ***************
// SPL_Telekinesis
// ***************

const int SPL_Cost_Telekinesis		= 10;
const int STEP_Telekinesis			= 1;
const int SPL_Damage_Telekinesis 		= 0;
const int TELEKINESIS_UPMOVEMENT = 50; // (in cm) the focus item is moved upwards when starting the spell.
const int TELEKINESIS_SPEED = 150; // (in cm/s) the traveling speed of the focus item.
const int TELEKINESIS_MOVEMENT_DELAY = 2000; // (in ms) time to wait before the focus item travels

const string PRINT_TO_FAR_AWAY = "Mmh, das Objekt kann ich nicht sehen...";


INSTANCE Spell_Telekinesis (C_Spell_Proto)
{
	time_per_mana			= 0; // Zeit pro investierten Manapunkt (ms)
	spelltype 				= SPELL_NEUTRAL;
	damage_per_level		= SPL_Damage_Telekinesis;		
	damageType				= DAM_MAGIC;				
	canTurnDuringInvest     = FALSE;
	
	canChangeTargetDuringInvest	=	FALSE;
	isMultiEffect				=	FALSE;
	targetCollectAlgo			=	TARGET_COLLECT_FOCUS_FALLBACK_NONE; //TARGET_COLLECT_FOCUS_FALLBACK_NONE;//TARGET_COLLECT_FOCUS_FALLBACK_NONE;
	targetCollectType			=	TARGET_TYPE_ITEMS;
	targetCollectRange			=	10000;		
	targetCollectAzi			=	85;
	targetCollectElev			=	85;
};

/**
 * Holds data needed for the telekinese interpolation.
 */
class Spell_Telekinesis_Data {
	var int pTarget; // the target item
	var int pInterpolator; // handle to the telekinese Interpolator
};

instance Spell_Telekinesis_Data@(Spell_Telekinesis_Data);

var Spell_Telekinesis_Data _TelekinesisData;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
////// NECLIB EXTERNALS
//////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 /**
  * Creates a new telekinesis interpolator object.
  * @param pVobPosition : (pointer to a zVEC3) position of the vob that should be moved.
  * @param pNpcPosition : (pointer to a zVEC3) position where the vob should travel to.
  * @param upMoveAmount : (in cm) before the vob travels, it is moved upwards.
  * @param speed : The travelling speed (in centimeters per second).
  * @param speed : A delay before the interplation starts (in milliseconds)
  */
func int TELEKINESIS_CreateInterpolator(var int pVobPosition, var int pNpcPosition, var int upMoveAmount, var int speed, var int delay) {
	if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
        MEM_SendToSpy(zERR_TYPE_WARN, "neclib: TELEKINESIS_CreateInterpolator: Telekinesis module isn't initialized!");
        return 0;
    };

    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_CreateInterpolator");
	CALL_IntParam(delay);
	CALL_IntParam(speed);
	CALL_IntParam(upMoveAmount);
	CALL_IntParam(pNpcPosition);
	CALL_IntParam(pVobPosition);
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
    return +ret;
};

/**
 * Provides the current calculated position for a travelling vob.
 * @param pTelekinesisInterpolator : (pointer to a telekinesis interpolator)
 * @param pDestVec3 : (pointer to zVEC3) will hold the calculated position.
 */
func void TELEKINESIS_GetInterpolatedVec(var int pTelekinesisInterpolator, var int pDestVec3) {
    if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
		MEM_SendToSpy(zERR_TYPE_WARN, "neclib: TELEKINESIS_GetInterpolatedVec: Telekinesis module isn't initialized!");
        return;
    };

    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_GetInterpolatedVec");
	CALL_IntParam(pDestVec3);
	CALL_IntParam(pTelekinesisInterpolator);
	CALL__cdecl(adr);
};

/**
 * Deletes a telekinesis interpolator.
 * @param pTelekinesisInterpolator : (pointer to a telekinesis interpolator)
 */
func void TELEKINESIS_DeleteInterpolator(var int pTelekinesisInterpolator) {
    if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
		MEM_SendToSpy(zERR_TYPE_WARN, "neclib: TELEKINESIS_DeleteInterpolator: Telekinesis module isn't initialized!");
        return;
    };

    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_DeleteInterpolator");
	CALL_IntParam(pTelekinesisInterpolator);
	CALL__cdecl(adr);
};

/**
 * Sets the position of a zCVob to the currently calculated position of a telekinesis interpolator.
 * @param pTelekinesisInterpolator : (pointer to a telekinesis interpolator)
 * @param zVec3Ptr : (pointer to a zVEC3) the calculated, interpolated position
 */
func void TELEKINESIS_Interpolate(var int pTelekinesisInterpolator, var int zVec3Ptr) {
    if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
		MEM_SendToSpy(zERR_TYPE_WARN, "neclib: TELEKINESIS_Interpolate: Telekinesis module isn't initialized!");
        return;
    };

	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_Interpolate");
	CALL_IntParam(zVec3Ptr);
	CALL_IntParam(pTelekinesisInterpolator);
	CALL__cdecl(adr);
};

/**
 * Checks if the vob is seeable (by a npc) for starting telekinesis spell.
 * @param pNpc : (pointer to oCNpc) the npc
 * @param vobPtr : (pointer to zCVob) the vob
 * @return : TRUE if the vob is seeable.
 */
func int TELEKINESIS_IsVobSeeable(var int pNpc, var int pVob) {
	if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
		MEM_SendToSpy(zERR_TYPE_WARN, "neclib: TELEKINESIS_IsVobSeeable: Telekinesis module isn't initialized!");
        return 0;
    };

    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_IsVobSeeable");
	CALL_IntParam(pVob);
	CALL_IntParam(pNpc);
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
	
    return +ret;
};


/**
 * Clears any not deleted interpoaltors.
 */
func void TELEKINESIS_ClearInterpolators() {
    if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
		MEM_SendToSpy(zERR_TYPE_WARN, "neclib: TELEKINESIS_ClearInterpolators: Telekinesis module isn't initialized!");
        return;
    };

	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_ClearInterpolators");
	CALL__cdecl(adr);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * Checks the state of the telekinesis interpolator and stops the spell
 */
func void _Spell_Telekinesis_MoveTarget() {

    // Another safety check
    if (!Hlp_Is_oCItem(_TelekinesisData.pTarget)) {
        return;
    };

    var oCItem oCTarget; oCTarget = _^(_TelekinesisData.pTarget);

    // Continue movement
	var int interpolatedPosition[3]; //same memory layout as zVEC3
	
    TELEKINESIS_Interpolate(_TelekinesisData.pInterpolator, _@(interpolatedPosition));
	
	// Update position of item
	zCVobSetPositionWorld(_TelekinesisData.pTarget, _@(interpolatedPosition));
};

func int Spell_Logic_Telekinesis (var int manaInvested)
{
	
	if (manaInvested == 0 && self.attribute[ATR_MANA]<SPL_Cost_Telekinesis) 
	{
		return SPL_DONTINVEST;	
	};
	
	if (!Hlp_IsValidNpc(self)) {return SPL_DONTINVEST;};
	
	
	var oCNpc oCSelf; oCSelf = _^(_@(self));
	
	if (oCSelf.focus_vob == 0) {return SPL_DONTINVEST;};
	
	var oCItem oCFocus; oCFocus = _^(oCSelf.focus_vob);
	
	if (!Hlp_IsValidItem(oCFocus)) {return SPL_DONTINVEST;};
	
	var int canSee; canSee = TELEKINESIS_IsVobSeeable(_@(self), oCSelf.focus_vob);
	//MEM_Warn(ConcatStrings("canSee = ", IntToString(canSee)));
	
	if (!canSee && (manaInvested == 0)) {
		Print(PRINT_TO_FAR_AWAY);
		return SPL_DONTINVEST;
	};
	
	if (manaInvested == 0) {
	
		// we substract one mana point less for suppressing auto cast causing spell to stop prematurely 
		// Spell_Cast_Telekinesis substracts that missing point.
		self.attribute[ATR_MANA] = (self.attribute[ATR_MANA] - SPL_Cost_Telekinesis + 1);

		if (self.attribute[ATR_MANA]<0) 
		{
			self.attribute[ATR_MANA]=0;
		};
				

        _TelekinesisData.pTarget = oCSelf.focus_vob;
		
		
		var oCItem oCTarget; oCTarget = _^(_TelekinesisData.pTarget);
		
		var int vobPosition[3];
        vobPosition[0] = oCTarget._zCVob_trafoObjToWorld[3];
        vobPosition[1] = oCTarget._zCVob_trafoObjToWorld[7];
        vobPosition[2] = oCTarget._zCVob_trafoObjToWorld[11];
        
        var int npcPosition[3];
        npcPosition[0] = oCSelf._zCVob_trafoObjToWorld[3];
        npcPosition[1] = oCSelf._zCVob_trafoObjToWorld[7];
        npcPosition[2] = oCSelf._zCVob_trafoObjToWorld[11];
        
		// Delete the old interpolator
		// Note: interpolators are handles. Thus, it is ok if the interpolator isn't valid, 
		// since neclib checks the handle
		TELEKINESIS_DeleteInterpolator(_TelekinesisData.pInterpolator);
        _TelekinesisData.pInterpolator = TELEKINESIS_createInterpolator(_@(vobPosition[0]), 
                                                            _@(npcPosition[0]),
                                                            TELEKINESIS_UPMOVEMENT,
                                                            TELEKINESIS_SPEED,
															TELEKINESIS_MOVEMENT_DELAY);
		
		// ensure that npcs react if hero tries to steal
		if (Npc_IsPlayer(self)) {
			var int itemBak; itemBak = _@(item); 

			item = _^(_TelekinesisData.pTarget);			
			Npc_SendPassivePerc(self, PERC_ASSESSTHEFT, self, self);
			MEM_AssignInstSuppressNullWarning = TRUE;
			item = _^(itemBak);
			MEM_AssignInstSuppressNullWarning = FALSE;
		};
		
	} else {
		_Spell_Telekinesis_MoveTarget();
	};
	
	return SPL_NEXTLEVEL;//SPL_NEXTLEVEL;
};

func void Spell_Cast_Telekinesis(var int spellLevel)
{		
	MEM_Info("neclib: Spell_Cast_Telekinesis: called.");

	// substract the last missing mana point.
	self.attribute[ATR_MANA] = (self.attribute[ATR_MANA] - 1);

	if (self.attribute[ATR_MANA]<0) 
	{
		self.attribute[ATR_MANA]=0;
	};
	
	// Only do something if the player uses the spell
	if (!Npc_IsPlayer(self)) {
		return;
	};
	
	// delete telekinesis data	
	TELEKINESIS_DeleteInterpolator(_TelekinesisData.pInterpolator);
	
	var oCItem target; target = _^(_TelekinesisData.pTarget);
	
	if (!Hlp_IsValidItem(target)) {return;};
	
	zCVobSetSleeping(_TelekinesisData.pTarget, FALSE);
    zCVobSetPhysicsEnabled(_TelekinesisData.pTarget, TRUE);    
};



/*
 * Reset Focus_Magic instance
 * This function is called if (and only if) the active spell changes
 * Source: https://forum.worldofplayers.de/forum/threads/1547129
 */
func void Spell_Telekinesis_ResetFocus() {
    var oCMag_Book mb; mb = _^(ECX);
    if (!mb.owner) {
        return;
    };
    var C_Npc slf; slf = _^(mb.owner);
    if (!Npc_IsPlayer(slf)) {
        return;
    };

    // Should never happen, but safety first
    if (!_@(Focus_Magic)) {
        return;
    };

    // Backup/reset Focus_Magic completely
    const int focusCopy = 0;
    const int sizeof_oCNpcFocus = 80;
    if (!focusCopy) {
        // Create one-time backup per session
        MEM_Info("neclib: Spell_Telekinesis: Backing up original Focus_Magic values");
        focusCopy = MEM_Alloc(sizeof_oCNpcFocus);
        MEM_CopyBytes(_@(Focus_Magic), focusCopy, sizeof_oCNpcFocus);
    } else {
        // Reset on every spell change
        MEM_CopyBytes(focusCopy, _@(Focus_Magic), sizeof_oCNpcFocus);
    };
};



/*
 * Enable focusing item when using the spell
 * Modified from https://forum.worldofplayers.de/forum/threads/1547129
 */
func void Spell_Telekinesis_Prio() {
    var int caster; caster = MEM_ReadInt(ESP+4);
    if (!caster) {
        return;
    };
    var C_Npc slf; slf = _^(caster);
    if (!Npc_IsPlayer(slf)) {
        return;
    };

    var int spellID; spellID = MEM_ReadInt(/*oCSpell*/ECX+/*spellID*/84);
    if (spellID == SPL_Telekinesis) {
        // Adjust the global(!) focus priorities temporarily(!) = until the active spell changes
        Focus_Magic.npc_prio = -1;
        Focus_Magic.item_azi = 90.0;    
        Focus_Magic.item_elevup = 90.0; 
        Focus_Magic.item_elevdo = -89.0;
        Focus_Magic.item_prio = 1;
    };
};


/**
 * Inits the telekinesis module.
 */ 
func void TELEKINESIS_Init() 
{
	if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
        MEM_SendToSpy(zERR_TYPE_FAULT, "neclib: LEVITATION_Init: Telekinesis module isn't specified to be initialized!");
        return;
    };


	// Setup hooks for focus
    const int oCSpell__Setup_G2            = 4737328; //0x484930
    const int oCMag_Book__SetFrontSpell_G2 = 4688320; //004789C0
    HookEngineF(oCSpell__Setup_G2, 7, Spell_Telekinesis_Prio);
    HookEngineF(oCMag_Book__SetFrontSpell_G2, 7, Spell_Telekinesis_ResetFocus);

    // Ensure that Focus_Magic is not empty (necessary for Spell_Telekinesis_Prio). For details see GothicFreeAim
    const int oCNpcFocus__focuslist_G2     = 11208440; //0xAB06F8
    var int fMagicPtr; fMagicPtr = MEM_ReadIntArray(oCNpcFocus__focuslist_G2, /*Focus_Magic*/ 5);
    if (fMagicPtr) {
        MEM_Info("neclib: Spell_Telekinesis: Reassigning Focus_Magic instance");
        Focus_Magic = _^(fMagicPtr);
    };
	
	// just for safety. Shouldn't be necessary
	TELEKINESIS_ClearInterpolators();
	
	// _TelekinesisData is a global variable we only need to initialize once
	if (!_@(_TelekinesisData)) {
		_TelekinesisData = _^(create(Spell_Telekinesis_Data@));
		MEM_Info("neclib: TELEKINESIS_Init: assigned _TelekinesisData to a new Spell_Telekinesis_Data@ instance");
	};
	
	MEM_Info("neclib: TELEKINESIS_Init: done.");
};