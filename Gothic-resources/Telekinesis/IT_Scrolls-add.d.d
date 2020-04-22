const int 	Value_Sc_Telekinesis	= 200;

/*******************************************************************************************/
INSTANCE ItSc_Telekinesis(C_Item)
{
	name 				=	NAME_Spruchrolle;

	mainflag 			=	ITEM_KAT_RUNE;
	flags 				=	ITEM_MULTI|ITEM_MISSION;

	value 				=	Value_Sc_Telekinesis;

	visual				=	"ItSc_PalLight.3DS";
	material			=	MAT_LEATHER;

	spell				= 	SPL_TELEKINESIS;
	cond_atr[2]   		=	ATR_MANA_MAX;
	cond_value[2]  		=	SPL_Cost_Telekinesis;

	wear				= 	WEAR_EFFECT;
	effect				=	"SPELLFX_WEAKGLIMMER";

	description			=	NAME_SPL_Telekinesis;
	
	TEXT	[0]			=	Name_MageScroll	;
	
	TEXT	[1]			=	NAME_Mana_needed;			
	COUNT	[1]			=	SPL_Cost_Telekinesis;
	
	TEXT	[5]			=	NAME_Value;					
	COUNT	[5]			=	value;
};

/*******************************************************************************************/