///   													XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
///   													XX  T E L E K I N E S I S  XX
///   													XXXXXXXXXXXXXXXXXXXXXXXXXXXXX


INSTANCE spellFX_telekinesis(CFx_Base_Proto)
{
		visname_S 			= "MFX_Telekinesis_INIT";
		emtrjmode_s 		= "TARGET";
		emTrjOriginNode 	= "ZS_RIGHTHAND";
		emtrjnumkeys 		= 2;
		emtrjnumkeysvar 	= 1;
		emtrjangleelevvar 	= 2.;
		emtrjangleheadvar 	= 0.;
		emtrjeasefunc_s 	= "LINEAR";
		emtrjloopmode_s 	= "HALT";
		emtrjdynupdatedelay = 0.;
		emFXInvestOrigin_S 	= "spellFX_Telekinesis_ORIGIN";
		//emFXInvestTarget_S 	= "spellFX_Telekinesis_TARGET";
		//lightPresetname 	= "POISON";
		emTrjTargetRange	= 0;
		emTrjTargetElev 	= 0;
};

INSTANCE spellFX_telekinesis_KEY_INIT (C_ParticleFXEmitKey)
{
		visname_s			= "MFX_Telekinesis_INIT";
		emtrjeasevel	  	= 0.01;
};

INSTANCE spellFX_telekinesis_KEY_INVEST_1	(C_ParticleFXEmitKey)
{
		visname_s			= "MFX_Telekinesis_TARGET";
		emtrjeasevel	  	= 2000;
		sfxid				= "MFX_TELEKINESIS_STARTINVEST";
		sfxisambient		= 1;
};


INSTANCE spellFX_telekinesis_KEY_CAST	(C_ParticleFXEmitKey)
{
		visname_s			= "MFX_Telekinesis_TargetEnd";
		
};

/*INSTANCE spellFX_telekinesis_KEY_STOP (C_ParticleFXEmitKey)
{
		visname_s			= "MFX_Telekinesis_TargetEnd";
		
};*/

INSTANCE spellFX_telekinesis_Origin		(CFx_Base_Proto)
{
		visname_S 			= "MFX_Telekinesis_BRIDGE";
		emtrjmode_s 		= "TARGET LINE";
		emtrjeasevel	  	= 0.001;
		emTrjOriginNode 	= "BIP01 R Hand";
		emtrjdynupdatedelay = 0.;
		lightPresetname 	= "AURA";
		sfxid				= "MFX_TELEKINESIS_INVEST";
		sfxisambient		= 1;
};
