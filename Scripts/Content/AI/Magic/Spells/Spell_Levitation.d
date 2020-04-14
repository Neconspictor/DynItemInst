// *********************
// Levitation Spell
// *********************

const int SPL_Cost_Levitation			= 5;


INSTANCE Spell_Levitation (C_Spell_Proto)
{
	time_per_mana			= 0;
	spelltype 				= SPELL_NEUTRAL;
	targetCollectAlgo		= TARGET_COLLECT_NONE;
	//canTurnDuringInvest		= 0;
};

func int Spell_Logic_Levitation (var int manaInvested)
{
	if ((Npc_GetActiveSpellIsScroll(self) && (self.attribute[ATR_MANA] >= SPL_Cost_Scroll)))
	|| (self.attribute[ATR_MANA] >= SPL_Cost_Levitation)
	{
		return SPL_SENDCAST;
	}
	else
	{
		return SPL_SENDSTOP;
	};
};

func void Spell_Cast_Levitation(var int spellLevel)
{
	if (Npc_GetActiveSpellIsScroll(self))
	{
		self.attribute[ATR_MANA] = self.attribute[ATR_MANA] - SPL_Cost_Scroll;
	}
	else
	{
		self.attribute[ATR_MANA] = self.attribute[ATR_MANA] - SPL_Cost_Levitation;
	};
	
	self.aivar[AIV_SelectSpell] += 1;
	
	if (Npc_IsPlayer(self)) {
		AI_RemoveWeapon(self);
		LEVITATION_Begin();
	};
};