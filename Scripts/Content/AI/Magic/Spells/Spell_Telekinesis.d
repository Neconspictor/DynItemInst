// ***************
// SPL_Telekinesis
// ***************

const int SPL_Cost_Telekinesis		= 1;
const int STEP_Telekinesis			= 1;
const int SPL_Damage_Telekinesis 		= 0;


func int Spell_Telekinesis_focus_remover()
{
  //var oCNpc her; her = _^(_@(hero)); // Todo: use this for investagting ikarus undefined behaviour in MEM_PtrToInst (if ptr is 0); 
  return FALSE;
};

INSTANCE Spell_Telekinesis (C_Spell_Proto)
{
	time_per_mana			= 0; // Zeit pro investierten Manapunkt (ms)
	spelltype 				= SPELL_NEUTRAL;
	damage_per_level		= SPL_Damage_Telekinesis;		
	damageType				= DAM_MAGIC;				
	canTurnDuringInvest     = FALSE;
	
	canChangeTargetDuringInvest	=	Spell_Telekinesis_focus_remover();
	isMultiEffect				=	0;				// Effect Class is oCVisFX_MultiTarget if set to 1 (e.g. the effect can have multiple trajectorys (massdeath)
	targetCollectAlgo			=	TARGET_COLLECT_FOCUS_FALLBACK_NONE; //TARGET_COLLECT_FOCUS_FALLBACK_NONE;//TARGET_COLLECT_FOCUS_FALLBACK_NONE;
	targetCollectType			=	TARGET_TYPE_ITEMS;
	targetCollectRange			=	10000;		
	targetCollectAzi			=	85;
	targetCollectElev			=	85;
};

class Spell_Telekinesis_Data {
    var int pCaster;
	var int pTarget;
	var int pInterpolator;
};
instance Spell_Telekinesis_Data@(Spell_Telekinesis_Data);


//.text:0061BB70 ; public: void __thiscall zCVob::SetPositionWorld(class zVEC3 const &)
const int zCVob__SetPositionWorld = 6404976; //0x5EE650  //0x0061BB70



func void _Spell_Telekinesis_MoveTarget(var int hndl) {
	var Spell_Telekinesis_Data data; data = get(hndl);
	var oCNpc oCSelf; oCSelf = _^(data.pCaster);
	
	var oCItem oCTarget; oCTarget = _^(data.pTarget);
	//MEM_WARN(ConcatStrings("oCTarget.name = ", oCTarget.name));
	
	if (oCSelf.aiscriptvars[AIV_SelectSpell] != 0 ) {
		oCSelf.aiscriptvars[AIV_SelectSpell] = 0;
		
		NECPACK_DrobVob(data.pCaster, data.pTarget);
		TELEKINESIS_DeleteInterpolator(data.pInterpolator);
		FF_RemoveData(_Spell_Telekinesis_MoveTarget, hndl);
		delete(hndl);
		MEM_WARN("_Spell_Telekinesis_MoveTarget():: Detected end of target move!");
		
		return;
	};
	
	var int vobPosition[3];
	
	TELEKINESIS_Interpolate(data.pInterpolator, data.pTarget);
	//DII_Telekinesis_GetInterpolatedVec(data.pInterpolator, _@(vobPosition[0]));
	
	
	//oCTarget._zCVob_bitfield[0] = oCTarget._zCVob_bitfield[0] & ~zCVob_bitfield0_physicsEnabled;
	//oCTarget._zCVob_bitfield[0] = oCTarget._zCVob_bitfield[0] & ~zCVob_bitfield0_collDetectionStatic;
	//oCTarget._zCVob_bitfield[0] = oCTarget._zCVob_bitfield[0] & ~zCVob_bitfield0_collDetectionDynamic;
	
	//oCTarget._zCVob_trafoObjToWorld[3] = vobPosition[0];
	//oCTarget._zCVob_trafoObjToWorld[7] = vobPosition[1];
	//oCTarget._zCVob_trafoObjToWorld[11] = vobPosition[2];
	
	/*var int posPtr; posPtr = _@(vobPosition[0]);
	
	
	// Reposition the vob
	const int call = 0;
	if (CALL_Begin(call)) {
		//CALL_PtrParam(_@(vobPosition));
		CALL_IntParam(_@(posPtr));
		CALL__thiscall(_@(data.pTarget), zCVob__SetPositionWorld);
		call = CALL_End();
	};*/
};

func void _Spell_Telekinesis_MoveTargetStart(var int hndl) {

	//var int hndlCopy; hndlCopy = new(Spell_Telekinesis_Data@);
	//var Spell_Telekinesis_Data data; data = get(hndl);
	//var Spell_Telekinesis_Data dataCopy; dataCopy = get(hndlCopy);
	//dataCopy.pCaster = data.pCaster;
	//dataCopy.pTarget = data.pTarget;
	
	
	//int DII_Telekinesis_createInterpolator(var int pVobPosition, var int pNpcPosition, var int upMoveAmount, var int speed)
	//DII_Telekinesis_GetInterpolatedVec(var int pTelekinesisInterpolator, var int pDestVec3)
	
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
															50,
															100);
																				
	
	FF_ApplyData(_Spell_Telekinesis_MoveTarget, hndl);
};

func int Spell_Logic_Telekinesis (var int manaInvested)
{
	
	//MEM_Warn("Spell_Logic_Telekinesis called!");

	if (manaInvested == 0 && self.attribute[ATR_MANA]<SPL_Cost_Telekinesis) 
	{
		return SPL_DONTINVEST;	
	};
	
	if (!Hlp_IsValidNpc(self)) {return SPL_DONTINVEST;};
	
	
	var oCNpc oCSelf; oCSelf = _^(_@(self));
	
	if (oCSelf.focus_vob == 0) {return SPL_DONTINVEST;};
	
	var oCItem oCFocus; oCFocus = _^(oCSelf.focus_vob);
	
	if (!Hlp_IsValidItem(oCFocus)) {return SPL_DONTINVEST;};
	
	var int canSee; canSee = TELEKINESIS_Npc_CanSeeVob(_@(self), oCSelf.focus_vob);
	//MEM_Warn(ConcatStrings("canSee = ", IntToString(canSee)));
	
	if (!canSee && (manaInvested == 0)) {
		//MEM_Warn("Cannot see focus vob!!!");
		//oCNpcSetFocusVob(_@(self), 0);
		return SPL_DONTINVEST;
	};
	
	if (manaInvested == 0) {
		self.attribute[ATR_MANA] = (self.attribute[ATR_MANA] - SPL_Cost_Telekinesis);

		if (self.attribute[ATR_MANA]<0) 
		{
			self.attribute[ATR_MANA]=0;
		};
		
		var int hndl; hndl = new(Spell_Telekinesis_Data@);
		var Spell_Telekinesis_Data data; data = get(hndl);
		data.pCaster = _@(oCSelf);
		data.pTarget = oCSelf.focus_vob;
		
		FF_ApplyExtData(_Spell_Telekinesis_MoveTargetStart, 2000, 1, hndl);
	};
	
	//MEM_WARN(ConcatStrings("oCFocus.name = ", oCFocus.name));
	//MEM_Warn(ConcatStrings("AIV_SpellLevel = ", IntToString(self.aivar[AIV_SpellLevel])));
	
	//self.aivar[AIV_SpellLevel] = 2;
	return SPL_NEXTLEVEL;
};

func void Spell_Cast_Telekinesis(var int spellLevel)
{
	//self.attribute[ATR_MANA] = self.attribute[ATR_MANA] - SPL_Cost_Sleep;			// nicht drin, wegen Kommentar oben
	self.aivar[AIV_SelectSpell] += 1;
	MEM_WARN("Spell_Cast_Telekinesis: called.");
};