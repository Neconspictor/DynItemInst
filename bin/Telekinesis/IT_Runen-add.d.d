const	int	Value_Ru_Telekinesis			=	0;

/*******************************************************************************************/
INSTANCE ItRu_Telekinesis (C_Item)
{
	name 				=	NAME_Rune;

	mainflag 			=	ITEM_KAT_RUNE;
	flags 				=	0;

	value 				=	Value_Ru_Telekinesis;

	visual				=	"ItRu_PalLight.3ds";
	material			=	MAT_STONE;

	spell				= 	SPL_Telekinesis;
	

	wear				= 	WEAR_EFFECT;
	effect				=	"SPELLFX_WEAKGLIMMER";

	description			=	NAME_SPL_Telekinesis;
	
	TEXT	[0]			=	Name_PaladinSpell	;
	
	TEXT	[1]			=	NAME_Manakosten;		
	COUNT	[1]			=	SPL_Cost_Telekinesis;
	
	TEXT	[2]			=	NAME_Duration;		
	COUNT	[2]			=	SPL_Duration_PalLIGHT;
	
	TEXT	[5]			=	NAME_Value;				
	COUNT	[5]			=	value;
}; 
/*******************************************************************************************/