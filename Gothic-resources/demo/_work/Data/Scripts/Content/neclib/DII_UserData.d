/* 
 * This constants are used to determine by the neclib library how DII_USER_DATA is defined.
 * Define as many integers and strings as you like, but keep in mind that integers have to be defined before
 * the string members. neclib will load the members assuming that order!
 * Note: If you change the amount, savegames with a different amount do not work anymore and you will get a fatal error message.
 */
const int DII_USER_DATA_INTEGER_AMOUNT = 10;
const int DII_USER_DATA_STRING_AMOUNT = 5;

//indices used for magic weapons example
const int MAGICWEAPON_ENCHANTEDWEAPON = 0;

CLASS DII_USER_DATA
{
	// At first the integer members
	var int ints [DII_USER_DATA_INTEGER_AMOUNT];
	
	
		
	//next the string members
	// string arrays don't work properly; 
	// you cannot pass string array members (like myStrings[2])to functions or 
	// use it in an assignment expression on the right side.
	var string magicWeaponNewDesc;
	var string magicWeaponOldDesc;
	var string magicWeaponNewEffect;
	var string magicWeaponOldEffect;
	var string magicWeaponOldInstanceName;
};

// slot number for DII 
const int DII_SLOT_COUNT = 9;

// Defines the animation slots that will be tested for dii item changes by NEC library.
// Default are the slots of HUM_BODY_NAKED0.ASC, but change it, if you need more. You can also add
// slots from other models.
const string DII_SLOTS[DII_SLOT_COUNT] = {
	"ZS_HELMET",
	"ZS_LEFTHAND",
	"ZS_LEFTARM",
	"ZS_RIGHTHAND",
	"ZS_CROSSBOW",
	"ZS_LONGSWORD",
	"ZS_SHIELD",
	"ZS_BOW",
	"ZS_SWORD"
};