/**
 * Neclib telekinesis module
 */
 
 
 const int TARGET_TYPE_ITEM_TELEKINESIS = 256;
 
 /**
  * Creates a new telekinesis interpolator object.
  * @param pVobPosition : (pointer to a zVEC3) position of the vob that should be moved.
  * @param pNpcPosition : (pointer to a zVEC3) position where the vob should travel to.
  * @param upMoveAmount : (in cm) before the vob travels, it is moved upwards.
  * @param speed : The travelling speed.
  */
func int TELEKINESIS_CreateInterpolator(var int pVobPosition, var int pNpcPosition, var int upMoveAmount, var int speed) {
	if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
        MEM_Warn("TELEKINESIS_CreateInterpolator: Telekinesis module isn't initialized!");
        return 0;
    };

    var int ret;
	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_CreateInterpolator");
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
		MEM_Warn("TELEKINESIS_GetInterpolatedVec: Telekinesis module isn't initialized!");
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
		MEM_Warn("TELEKINESIS_DeleteInterpolator: Telekinesis module isn't initialized!");
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
 * @param vobPtr : (pointer to zCVob)
 */
func void TELEKINESIS_Interpolate(var int pTelekinesisInterpolator, var int vobPtr) {
    if (!(NEC_Init_Modules & NEC_TELEKINESIS)) {
		MEM_Warn("TELEKINESIS_Interpolate: Telekinesis module isn't initialized!");
        return;
    };

	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_Interpolate");
	CALL_IntParam(vobPtr);
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
		MEM_Warn("TELEKINESIS_IsVobSeeable: Telekinesis module isn't initialized!");
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
		MEM_Warn("TELEKINESIS_ClearInterpolators: Telekinesis module isn't initialized!");
        return;
    };

	var int adr;
	adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "TELEKINESIS_ClearInterpolators");
	CALL__cdecl(adr);
};




/*
 * Enable focusing mob when using the spell
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
    
	const int item_range2_backup = 42; // 42 == not initialized yet
	const int item_azi_backup = 42; // 42 == not initialized yet
	const int item_elevup_backup = 42; // 42 == not initialized yet
	const int item_elevdo_backup = 42; // 42 == not initialized yet
	const int item_prio_backup = 42; // 42 == not initialized yet
	
	
    if (item_prio_backup == 42) {
		item_range2_backup = castToIntf(Focus_Magic.item_range2);
		item_azi_backup = castToIntf(Focus_Magic.item_azi);
		item_elevup_backup = castToIntf(Focus_Magic.item_elevup);
		item_elevdo_backup = castToIntf(Focus_Magic.item_elevdo);
		item_prio_backup = Focus_Magic.item_prio;
    };

    var int spellID; spellID = MEM_ReadInt(/*oCSpell*/ECX+/*spellID*/84);
    if (spellID == SPL_Telekinesis) {
        // Adjust the global(!) focus priorities temporarily(!)
		// Note: item_range1 and item_range2 are stored in squared 
		Focus_Magic.item_range2 = castFromIntf(mulf(castToIntf(3500.0), castToIntf(3500.0)));
		Focus_Magic.item_azi = 90.0;	
		Focus_Magic.item_elevup = 90.0;	
		Focus_Magic.item_elevdo = -89.0;
		Focus_Magic.item_prio = 1;
		MEM_Info("Spell_Telekinesis_Prio: changed Focus_Magic");
		
    } else if (item_prio_backup != 42) {
        // Reset the focus priorities for all other spells!
        Focus_Magic.item_range2 = castFromIntf(item_range2_backup);
		Focus_Magic.item_azi = castFromIntf(item_azi_backup);
		Focus_Magic.item_elevup = castFromIntf(item_elevup_backup);
		Focus_Magic.item_elevdo = castFromIntf(item_elevdo_backup);
		Focus_Magic.item_prio = item_prio_backup;
		MEM_Info("Spell_Telekinesis_Prio: resetted Focus_Magic");
    };
	
	
	var int range1; range1 = truncf(castToIntf(Focus_Magic.item_range1));
	var int range2; range2 = truncf(castToIntf(Focus_Magic.item_range2));
	
	MEM_Info(ConcatStrings("Spell_Telekinesis_Prio: Focus_Magic.item_range1 = ", IntToString(range1)));
	MEM_Info(ConcatStrings("Spell_Telekinesis_Prio: Focus_Magic.item_range2 = ", IntToString(range2)));
	MEM_Info(ConcatStrings("Spell_Telekinesis_Prio: Focus_Magic.item_prio = ", IntToString(Focus_Magic.item_prio)));
};


/*
 * Make the focus check mob specific (disallow NPC)
 */
func void Spell_Telekinesis_Focus() {
    // Constructed case that will only happen for Spell_Telekinesis
    if (ECX == TARGET_TYPE_ITEM_TELEKINESIS) {
	
		//MEM_Info("Spell_Telekinesis_Focus: called");
        var int vobPtr; vobPtr = MEM_ReadInt(ESP+4);
		var C_Item itm; itm = _^(vobPtr);
        if (Hlp_IsValidItem(itm)) {
			MEM_Info(ConcatStrings("Spell_Telekinesis_Focus: itm.name = ", itm.name));
            ECX = 1;
        };
    };
};


/**
 * Inits the telekinesis module.
 */ 
func void TELEKINESIS_Init() 
{
	const int oCSpell__Setup_G1                = 4703664; //0x47C5B0
    const int oCSpell__Setup_G2                = 4737328; //0x484930
	const int oCSpell__IsTargetTypeValid_G1    = 4709316; //0x47DBC4
    const int oCSpell__IsTargetTypeValid_G2    = 4743108; //0x485FC4


    HookEngineF(oCSpell__Setup_G2, 7, Spell_Telekinesis_Prio);
	HookEngineF(+MEMINT_SwitchG1G2(oCSpell__IsTargetTypeValid_G1,
                                   oCSpell__IsTargetTypeValid_G2),        5, Spell_Telekinesis_Focus);
	
	// Make sure Focus_Magic is initialized (necessary for Spell_Telekinesis_Prio). For details see GothicFreeAim
    const int oCNpcFocus__focuslist_G1         =  9283120; //0x8DA630
    const int oCNpcFocus__focuslist_G2         = 11208440; //0xAB06F8


    var int fMagicPtr; fMagicPtr = MEM_ReadIntArray(+MEMINT_SwitchG1G2(oCNpcFocus__focuslist_G1,
																		oCNpcFocus__focuslist_G2),  5); ///Focus_Magic
    if (fMagicPtr) {
        MEM_Info("TELEKINESIS_Init: Reinitializing Focus_Magic instance");
        Focus_Magic = _^(fMagicPtr);
		
    };
	
	TELEKINESIS_ClearInterpolators();
};