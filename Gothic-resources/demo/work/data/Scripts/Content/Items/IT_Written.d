instance ITWR_FireEnchantment(C_Item)
{
	name = "Feuerverzauberung";
	mainflag = ITEM_KAT_DOCS;
	flags = ITEM_MULTI;
	value = 200;
	visual = "ItWr_Scroll_01.3DS";
	material = MAT_LEATHER;
	on_state[0] = useFireEnchantment;
	scemeName = "MAP";
	description = name;
	text[1] = "Die Feuerverzauberung vereint die Kraft des Stahls mit der Kraft des Feuers.";
	text[2] = "Anwendbar auf die getragene Nah- oder Fernkampfwaffe.";
	text[3] = "Feuerschaden-Bonus:";		count[3] = MAGICWEAPON_BonusDamage;
	text[5] = NAME_VALUE; 					count[5] = value;
	nutrition = 1;
};


func void useFireEnchantment()
{
	ZAUBERART = 1;
	AI_ProcessInfos (hero);
};