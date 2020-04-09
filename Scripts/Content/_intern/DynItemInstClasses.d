// ***********************************************************************					
// Represents additional memory for dynamic item instances (dii).
// This additional memory is created for each dii and can be freely used.						
// ***********************************************************************

/* 
 * This constants are used to determine by the DII-Library how DII_USER_DATA is defined.
 * Define as many integers and strings as you like, but keep in mind that integers have to be defined before
 * the string members. DII will load the members assuming that order!
 */
 
const int DII_USER_DATA_INTEGER_AMOUNT = 2;
const int DII_USER_DATA_STRING_AMOUNT = 4;

//indices used for magic weapons
const int MAGICWEAPON_OLDID = 0;
const int MAGICWEAPON_ENCHANTEDWEAPON = 1;
const int MAGICWEAPON_TYPE = 2;

CLASS DII_USER_DATA
{
	// At first the integer members
	var int data [DII_USER_DATA_INTEGER_AMOUNT];
	
	//next the string members
	//string arrays don't work properly;
	var string magicWeaponNewDesc;
	var string magicWeaponOldDesc;
	var string magicWeaponNewEffect;
	var string magicWeaponOldEffect;
};