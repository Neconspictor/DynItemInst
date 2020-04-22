///   													XXXXXXXXXXXXXXXXXX
///   													XX  LEVITATION  XX
///   													XXXXXXXXXXXXXXXXXX

INSTANCE spellFX_Levitation	(CFx_Base_Proto)
{
		visname_S 			= "MFX_Transform_INIT";
		emtrjoriginnode 	= "ZS_RIGHTHAND";
		emtrjmode_s 		= "fixed";
		emtrjloopmode_s 	= "NONE";
		emtrjeasefunc_s 	= "LINEAR";
		emtrjdynupdatedelay = 5000;
		};

		instance spellFX_Levitation_KEY_INVEST_0		(C_ParticleFXEmitKey)
		{
				//visname_S 			= "MFX_Transform_ORIGIN";
				emCreateFXID		= "spellFX_Transform_ORIGIN";
		};

		instance spellFX_Levitation_KEY_INVEST_1		(C_ParticleFXEmitKey)
		{
				//visname_S 			= "MFX_Transform_ORIGIN";
				emCreateFXID		= "spellFX_Transform_ORIGIN";
		};

		instance spellFX_Levitation_KEY_CAST			(C_ParticleFXEmitKey)
		{
				pfx_ppsisloopingChg = 1;
				emCreateFXID		= "spellFX_Transform_BLEND";
};


instance spellFX_Levitation_ORIGIN	(CFx_Base_Proto)
{
		visname_S 				= "MFX_Transform_ORIGIN";
		emtrjoriginnode 		= "BIP01";
		emtrjmode_s 			= "FIXED";
		emtrjdynupdatedelay 	= 0;
		emselfrotvel_s 			= "0 0 50";
};

instance spellFX_Levitation_BLEND	(CFx_Base_Proto)
{
		visname_S 				= "MFX_Transform_BLEND";
		emtrjmode_s 			= "FIXED";
		emtrjoriginnode 		= "ZS_RIGHTHAND";
		emtrjdynupdatedelay 	= 0;
		sfxid					= "MFX_Transform_Cast";
		sfxisambient			= 1;

};