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
    var int pCaster; // the spell caster
	var int pTarget; // the target item
	var int pInterpolator; // pointer to the telekinese Interpolator
};

instance Spell_Telekinesis_Data@(Spell_Telekinesis_Data);


/*
 * Checks the state of the telekinesis interpolator and stops the spell
 */
func int _Spell_Telekinesis_MoveTarget(var int ptr) {
    var Spell_Telekinesis_Data data; data = _^(ptr);

    // Another safety check
    if (!Hlp_Is_oCNpc(data.pCaster)) || (!Hlp_Is_oCItem(data.pTarget)) {
        return FALSE;
    };

    var oCNpc caster; caster = _^(data.pCaster);  
    var oCItem oCTarget; oCTarget = _^(data.pTarget);
    var int spellLevel; spellLevel = caster.aiscriptvars[AIV_SpellLevel];

    // Begin movement
    if (!data.pInterpolator) {
        var int vobPosition[3];
        vobPosition[0] = oCTarget._zCVob_trafoObjToWorld[3];
        vobPosition[1] = oCTarget._zCVob_trafoObjToWorld[7];
        vobPosition[2] = oCTarget._zCVob_trafoObjToWorld[11];
        
        var int npcPosition[3];
        npcPosition[0] = caster._zCVob_trafoObjToWorld[3];
        npcPosition[1] = caster._zCVob_trafoObjToWorld[7];
        npcPosition[2] = caster._zCVob_trafoObjToWorld[11];
        
        data.pInterpolator = TELEKINESIS_createInterpolator(_@(vobPosition[0]), 
                                                            _@(npcPosition[0]),
                                                            TELEKINESIS_UPMOVEMENT,
                                                            TELEKINESIS_SPEED);

        return TRUE;
    };

    // End movement
    if (spellLevel > 0) {
        caster.aiscriptvars[AIV_SpellLevel] = 0;
        
        zCVobSetSleeping(data.pTarget, FALSE);
        zCVobSetPhysicsEnabled(data.pTarget, TRUE);
        
        TELEKINESIS_DeleteInterpolator(data.pInterpolator);
        MEM_Info("neclib: _Spell_Telekinesis_MoveTarget():: End of movement.");
        
        return FALSE;
    };

    // Continue movement
    TELEKINESIS_Interpolate(data.pInterpolator, data.pTarget);

    return TRUE;
};




/**
 * Starts the focus item to move.
 */
func void _Spell_Telekinesis_MoveTargetStart(var int hndl) {

	
	var Spell_Telekinesis_Data data; data = get(hndl);
	var zCVob target; target = _^(data.pTarget);
	var oCNpc caster; caster = _^(data.pCaster);
	
	var int vobPosition[3];
	vobPosition[0] = target.trafoObjToWorld[3];
	vobPosition[1] = target.trafoObjToWorld[7];
	vobPosition[2] = target.trafoObjToWorld[11];
	
	var int npcPosition[3];
	npcPosition[0] = caster._zCVob_trafoObjToWorld[3];
	npcPosition[1] = caster._zCVob_trafoObjToWorld[7];
	npcPosition[2] = caster._zCVob_trafoObjToWorld[11];
	
	data.pInterpolator = TELEKINESIS_createInterpolator(_@(vobPosition[0]), 
															_@(npcPosition[0]),
															TELEKINESIS_UPMOVEMENT,
															TELEKINESIS_SPEED);
																				
	
	FF_ApplyData(_Spell_Telekinesis_MoveTarget, hndl);
};

func int Spell_Logic_Telekinesis (var int manaInvested)
{
	
	//MEM_Warn("Spell_Logic_Telekinesis called!");
	MEM_Info("neclib: Spell_Logic_Telekinesis: called");

	if (manaInvested == 0 && self.attribute[ATR_MANA]<SPL_Cost_Telekinesis) 
	{
		MEM_Info("neclib: Spell_Logic_Telekinesis: Not enough mana");
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
				
		var int ptr; ptr = create(Spell_Telekinesis_Data@);
        var Spell_Telekinesis_Data data; data = _^(ptr);
        data.pCaster = _@(oCSelf);
        data.pTarget = oCSelf.focus_vob;
        data.pInterpolator = 0; // Mark as non-initialized

        MEM_ArrayInsert(_Spell_Telekinesis_InstanceArrayPtr, ptr);
		
		
		// ensure that npcs react if hero tries to steal
		if (Npc_IsPlayer(self)) {
			var int itemBak; itemBak = _@(item); 

			item = _^(data.pTarget);			
			Npc_SendPassivePerc(self, PERC_ASSESSTHEFT, self, self);
			MEM_AssignInstSuppressNullWarning = TRUE;
			item = _^(itemBak);
			MEM_AssignInstSuppressNullWarning = FALSE;
		};
		
	};
	
	return SPL_NEXTLEVEL;//SPL_NEXTLEVEL;
};

func void Spell_Cast_Telekinesis(var int spellLevel)
{	
	self.aivar[AIV_SpellLevel] = 1; // tells _Spell_Telekinesis_MoveTarget to stop
	
	// substract the last missing mana point.
	self.attribute[ATR_MANA] = (self.attribute[ATR_MANA] - 1);

	if (self.attribute[ATR_MANA]<0) 
	{
		self.attribute[ATR_MANA]=0;
	};
	
	MEM_Info("neclib: Spell_Cast_Telekinesis: called.");
};







/*
 * This is the "global" FrameFunction handling all active telekinesis spells
 */
func void _Spell_Telekinesis_Handler() {
    if (!_Spell_Telekinesis_InstanceArrayPtr) {
        return;
    };

    var zCArray arr; arr = _^(_Spell_Telekinesis_InstanceArrayPtr);
    if (!arr.numInArray) {
        return;
    };

    // Iterate over all instances and possibly delete them
    var int i; i = 0;
    while(i < arr.numInArray);
        var int ele; ele = MEM_ReadIntArray(arr.array, i);
        if (_Spell_Telekinesis_MoveTarget(ele)) {
            i += 1;
        } else {
            MEM_ArrayRemoveIndex(_Spell_Telekinesis_InstanceArrayPtr, i);
            MEM_Free(ele);
        };
    end;
};