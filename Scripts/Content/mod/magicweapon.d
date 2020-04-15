


//Hlp_GetInstanceID returns -1 if itm is not valid
const string MAGICWEAPON_NOT_INITIALIZED = "";

//effect time in millisecs
const int MAGICWEAPON_EFFECT_TIME = 1000*60*5;

const string MAGICWEAPON_MISSING_MELEE_WEAPON = "Keine Nahkampfwaffe ausgerüstet!";
const string MAGICWEAPON_MISSING_RANGED_WEAPON = "Keine Fernkampfwaffe ausgerüstet!";
const string MAGICWEAPON_DIALOG_BACK = "Abbrechen";
const string MAGICWEAPON_ERROR_ALREADY_ENCHANTED = "Waffe ist bereits verzaubert!";
const string MAGICWEAPON_USER_INFO = "Verzauberung hält 5 Minuten an!";

var int MAGICWEAPON_INIT_CALLED;
var string MAGICWEAPON_MELEE_WEAPON;
var string MAGICWEAPON_RANGED_WEAPON;
var int ZAUBERART;


class MAGICWEAPON_RemoveDesc {
    var string instanceName;
};

instance MAGICWEAPON_RemoveDesc@(MAGICWEAPON_RemoveDesc);

func int createMsgManipulate(var int vobPtr, var int msgType) {
	var int ptr; ptr = MEM_Alloc(OCMSGMANIPULATE_BYTE_SIZE);
	zCClassDefObjectCreated(ptr, oCMsgManipulateClassDefPtr);
	oCMsgManipulateoCMsgManipulate(ptr, msgType, vobPtr);
	return ptr;
};

func void sendMsgToNpc(var int npcPtr, var int msgPtr) {
	var int zCEventManagerPtr; zCEventManagerPtr = zCVobGetEM(npcPtr, 0);
	zCEventManagerOnMessage(zCEventManagerPtr, msgPtr, npcPtr);
};

func void sendEV_EquipItemMessageToNpc(var c_npc npc, var c_item itm) {
	var int npcPtr; npcPtr = _@(npc);
	var int itmPtr; itmPtr = _@(itm);
	var int msgType; msgType = EV_EQUIPITEM_MSGTYPE;
	var int msgPtr; msgPtr = createMsgManipulate(itmPtr, msgType);
	sendMsgToNpc(npcPtr, msgPtr);
};



func void magicWeaponInit(var c_npc npc) {
	var c_item itm; itm = Npc_GetEquippedMeleeWeapon(npc);
	MAGICWEAPON_MELEE_WEAPON = DII_GetSymbolName(Hlp_GetInstanceID(itm));
	itm = Npc_GetEquippedRangedWeapon(npc);
	MAGICWEAPON_RANGED_WEAPON = DII_GetSymbolName(Hlp_GetInstanceID(itm));
	
	//Deny further calls to this function
	MAGICWEAPON_INIT_CALLED = TRUE;
};

func void magicWeaponCreationDone(var c_npc npc) {
	MAGICWEAPON_MELEE_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
	MAGICWEAPON_RANGED_WEAPON = MAGICWEAPON_NOT_INITIALIZED;

	//Allow magic weapon initialization again
	MAGICWEAPON_INIT_CALLED = FALSE;
};

func void MagicWeaponSetEffect(var c_item itm, var string effectName) {
	//update item's visual effect
	oCItemRemoveEffect(itm);
	itm.effect = effectName;
	oCItemInsertEffect(itm);
    
	//update instance
	var int instanceId; instanceId = Hlp_GetInstanceID(itm);
	var string instanceName; instanceName = DII_GetSymbolName(instanceId);
	var c_item temp; temp = DII_CreateNewItem(instanceId);
	temp.effect = itm.effect;
	DII_UpdateInstance(temp, instanceName);
    
    //temp isn't needed anymore
    DII_DeleteItem(temp);
};

func void MagicWeaponAddItemEffect(var c_item itm) {
	if (!Hlp_IsValidItem(itm)) {
		return;
	};
	
	if (!DII_IsDynamic(itm)) {
		return;
	};
	
	var DII_USER_DATA data; data = DII_GetUserData(DII_GetSymbolName(Hlp_GetInstanceID(itm)));
	
	if (!data.data[MAGICWEAPON_ENCHANTEDWEAPON]) {
		return;
	};
	
	var string effect; effect = data.magicWeaponNewEffect;
	MagicWeaponSetEffect(itm, effect);	
};

func void MagicWeaponRemoveItemEffect(var c_item itm) {
	if (!Hlp_IsValidItem(itm)) {
		return;
	};
	
	var DII_USER_DATA data; data = DII_GetUserData(DII_GetSymbolName(Hlp_GetInstanceID(itm)));
	
	if (!data.data[MAGICWEAPON_ENCHANTEDWEAPON]) {
		return;
	};
	
	MagicWeaponSetEffect(itm, data.magicWeaponOldEffect);	
};

func void SetInstanceId(var c_item itm, var int instanceId) {
	if (!Hlp_isValidItem(itm)) {
		return;
	};
	
	var int InstancePtr; InstancePtr = _@(itm);
	InstancePtr = InstancePtr + 816;//0x330
	MEM_WriteInt(InstancePtr, instanceId);
	
	//test assignment
	var int testId; testId = Hlp_GetInstanceID(itm);
};

func int RemoveMagicWeaponEffects(var int instanceId) {
	//TODO
	
	/*var c_item itm;
	DII_GetItemByInstanceId(itm, instanceId);
	
	if (!Hlp_isValidItem(itm)) {
		return FALSE;
	};
	
	if (!DII_IsDynamic(itm)) {
		return FALSE;
	};
	
	var DII_USER_DATA data2;
	data2 = DII_GetUserData(instanceId); 
	
	//get the old instance id
	var int previousId; previousId = data2.data[MAGICWEAPON_OLDID];
	
	//Make sure that the dii is restored right
	MagicWeaponSetEffect(itm, data2.magicWeaponOldEffect);
	
	//Restore old effect
	itm.effect = data2.magicWeaponOldEffect;
	oCItemRemoveEffect(itm);
	
	//assign right instance id and umark item from dii
	itm.description = data2.magicWeaponOldDesc;
	var int itmPtr; itmPtr = _@(itm);
	var oCItem itmOC; itmOC = _^(itmPtr);
	DII_UpdateInstance(itm);
	itmOC.instanz = previousId;
	//DII_MarkAsReusable(instanceId, previousId);
	oCItemInsertEffect(itm);*/
	return TRUE;
};

func void MagicWeapon_ReleaseInstance(var int instanceId) {
    var DII_USER_DATA data;
	data = DII_GetUserData(instanceId); 
	
	//get the old instance id
	var int previousId; previousId = data.data[MAGICWEAPON_OLDID];
    DII_MarkAsReusable(instanceId, previousId);
    
    //unsign the instance as enchanted weapon
	data.data[MAGICWEAPON_ENCHANTEDWEAPON] = FALSE;
	data.data[MAGICWEAPON_OLDID] = MAGICWEAPON_NOT_INITIALIZED;
	data.data[MAGICWEAPON_TYPE] = MAGICWEAPON_NONE;
    data.magicWeaponOldEffect = "";
    data.magicWeaponOldDesc = "";
};

func void MagicWeapon_RemoveIceSpell(var int instanceId)
{
	RemoveMagicWeaponEffects(instanceId);
	PrintScreen ("Eisverzauberung ist erloschen!", -1, -1, "font_old_20_white.tga", 2);
    MagicWeapon_ReleaseInstance(instanceId);
	FF_RemoveData(MagicWeapon_RemoveIceSpell, instanceId);
};

func void MagicWeapon_RemoveFireSpell(var int instanceId)
{
	RemoveMagicWeaponEffects(instanceId);
	PrintScreen ("Feuerverzauberung ist erloschen!", -1, -1, "font_old_20_white.tga", 2);
    MagicWeapon_ReleaseInstance(instanceId);
	FF_RemoveData(MagicWeapon_RemoveFireSpell, instanceId);
};

func void MagicWeapon_RemovePoisonMagic(var int instanceId)
{
	RemoveMagicWeaponEffects(instanceId);
	PrintScreen ("Giftverzauberung ist erloschen!", -1, -1, "font_old_20_white.tga", 2);
    MagicWeapon_ReleaseInstance(instanceId);
	FF_RemoveData(MagicWeapon_RemovePoisonMagic, instanceId);
};

func void MagicWeapon_RemoveAnyActiveSpell(var int instanceId)
{
	MagicWeapon_RemoveIceSpell(instanceId);
	MagicWeapon_RemoveFireSpell(instanceId);
	MagicWeapon_RemovePoisonMagic(instanceId);
};

func void MagicWeapon_MakeEnchantedWeapon(var int weaponInstanceName, 
	var int type, 
	var string newEffect, 
	var string descriptionAddition, 
	var int scrollToRemove, 
	var func removeFunc) {
	
	
	var int weaponId; weaponId = DII_GetInstanceID(weaponInstanceName);
	var c_item temp; temp = DII_CreateNewItem(weaponId);
	var string oldDesc; oldDesc = temp.description;
	var string oldEffect; oldEffect = temp.effect;

	var string newDesc; newDesc = ConcatStrings(temp.description, descriptionAddition);
	temp.description = newDesc;
	temp.effect = newEffect;
	var int newId;
	var string newInstanceName;
	
	//Create new instance id and new item
	newInstanceName = DII_CreateNewInstanceStr(temp);
	newId = DII_GetInstanceID(newInstanceName);
	
    //temp isn't needed anymore
    DII_DeleteItem(temp);
    
	CreateInvItems(self, newId, 1);
    
    // you have to delete two items, strangely???
	Npc_RemoveInvItems(self, weaponId, 1);
	var DII_USER_DATA data;
	data = DII_GetUserData(newInstanceName);
	data.magicWeaponNewDesc = newDesc;
	data.magicWeaponOldDesc = oldDesc;
	data.magicWeaponNewEffect = newEffect;
	data.magicWeaponOldEffect = oldEffect;
	data.magicWeaponOldInstanceName = weaponInstanceName;
	
	//sign newId as an enchanted weapon
	data.data[MAGICWEAPON_ENCHANTEDWEAPON] = TRUE;
	data.data[MAGICWEAPON_TYPE] = type;
	Npc_RemoveInvItems (self, scrollToRemove, 1);
	
	//Equip weapon
	Npc_GetInvItem(self, newId);
	sendEV_EquipItemMessageToNpc(self, item);
	
	var int hndl; hndl = new(MAGICWEAPON_RemoveDesc@);
	var MAGICWEAPON_RemoveDesc removeDesc; removeDesc = get(hndl);
	removeDesc.instanceName = newInstanceName;
    
	//setup remove trigger;
	FF_ApplyExtData(removeFunc, MAGICWEAPON_EFFECT_TIME, -1, hndl);
};


instance PC_MAGICWEAPON_END(C_Info)
{
	npc = PC_Hero;
	nr = 999;
	condition = pc_magicweapon_end_condition;
	information = pc_magicweapon_end_info;
	permanent = TRUE;
	description = MAGICWEAPON_DIALOG_BACK;
};


func int pc_magicweapon_end_condition()
{
	if (ZAUBERART > 0)
	{
		return TRUE;
	};
};

func void pc_magicweapon_end_info()
{
	ZAUBERART = 0;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};


instance PC_FEUERZAUBER_NAH(C_Info)
{
	npc = PC_Hero;
	nr = 100;
	condition = pc_feuerzauber_nah_condition;
	information = pc_feuerzauber_nah_info;
	permanent = TRUE;
	description = "Feuerzauber auf Nahkampfwaffe anwenden!";
};


func int pc_feuerzauber_nah_condition()
{		
	if (ZAUBERART != 1) {
		return FALSE;
	}; 
	
	if (!MAGICWEAPON_INIT_CALLED) {
		magicWeaponInit(self);
	};
	
	if (DII_GetInstanceID(MAGICWEAPON_MELEE_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)) {
		return TRUE;
	};
	
	return FALSE;
};

func void pc_feuerzauber_nah_info()
{	
	if (DII_IsInstanceDynamic(MAGICWEAPON_MELEE_WEAPON)) {
		Print(MAGICWEAPON_ERROR_ALREADY_ENCHANTED);
	} else if (DII_GetInstanceID(MAGICWEAPON_MELEE_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)){
		MagicWeapon_MakeEnchantedWeapon(MAGICWEAPON_MELEE_WEAPON, MAGICWEAPON_FIRE, "SPELLFX_FLAMESWORD", " - entflammt", itwr_feuerzauber, MagicWeapon_RemoveFireSpell);
		MAGICWEAPON_MELEE_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
		AI_ReadyMeleeWeapon(self);
		PrintScreen (MAGICWEAPON_USER_INFO, -1, -1, "font_old_20_white.tga", 2);
	}else{
		Print(MAGICWEAPON_MISSING_MELEE_WEAPON);
	};

	//allow calling this dialog again
	ZAUBERART = 0;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};


instance PC_FEUERZAUBER_FERN(C_Info)
{
	npc = PC_Hero;
	nr = 200;
	condition = pc_feuerzauber_fern_condition;
	information = pc_feuerzauber_fern_info;
	permanent = TRUE;
	description = "Feuerzauber auf Fernkampfwaffe anwenden!";
};


func int pc_feuerzauber_fern_condition()
{
	if (ZAUBERART != 1) {
		return FALSE;
	}; 
	
	if (!MAGICWEAPON_INIT_CALLED) {
		magicWeaponInit(self);
	};
	
	if (DII_GetInstanceID(MAGICWEAPON_RANGED_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)) {
		return TRUE;
	};
	
	return FALSE;
};

func void pc_feuerzauber_fern_info()
{	
	if (DII_IsInstanceDynamic(MAGICWEAPON_RANGED_WEAPON)) {
		Print(MAGICWEAPON_ERROR_ALREADY_ENCHANTED);
	} else if (DII_GetInstanceID(MAGICWEAPON_RANGED_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)){
		MagicWeapon_MakeEnchantedWeapon(MAGICWEAPON_RANGED_WEAPON, MAGICWEAPON_FIRE, "SPELLFX_FLAMESWORD", " - entflammt", itwr_feuerzauber, MagicWeapon_RemoveFireSpell);
		MAGICWEAPON_RANGED_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
		AI_ReadyRangedWeapon(self);
		PrintScreen (MAGICWEAPON_USER_INFO, -1, -1, "font_old_20_white.tga", 2);
	}else{
		Print(MAGICWEAPON_MISSING_RANGED_WEAPON);
	};
	ZAUBERART = 0;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};


instance PC_EISZAUBER_NAH(C_Info)
{
	npc = PC_Hero;
	nr = 100;
	condition = pc_eiszauber_nah_condition;
	information = pc_eiszauber_nah_info;
	permanent = TRUE;
	description = "Eiszauber auf Nahkampfwaffe anwenden!";
};


func int pc_eiszauber_nah_condition()
{
	if (ZAUBERART != 2) {
		return FALSE;
	}; 
	
	if (!MAGICWEAPON_INIT_CALLED) {
		magicWeaponInit(self);
	};
	
	if (DII_GetInstanceID(MAGICWEAPON_MELEE_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)) {
		return TRUE;
	};
	
	return FALSE;
};


func void pc_eiszauber_nah_info()
{
	if (DII_IsInstanceDynamic(MAGICWEAPON_MELEE_WEAPON)) {
		Print(MAGICWEAPON_ERROR_ALREADY_ENCHANTED);
	} else if (DII_GetInstanceID(MAGICWEAPON_MELEE_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)){
		MagicWeapon_MakeEnchantedWeapon(MAGICWEAPON_MELEE_WEAPON, MAGICWEAPON_ICE, "SPELLFX_ICESWORD", " - vereist", itwr_eiszauber, MagicWeapon_RemoveIceSpell);
		MAGICWEAPON_MELEE_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
		AI_ReadyMeleeWeapon(self);
		PrintScreen (MAGICWEAPON_USER_INFO, -1, -1, "font_old_20_white.tga", 2);
	}else{
		Print(MAGICWEAPON_MISSING_MELEE_WEAPON);
	};
	
	//allow calling this dialog again
	ZAUBERART = 0;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};


instance PC_EISZAUBER_FERN(C_Info)
{
	npc = PC_Hero;
	nr = 200;
	condition = pc_eiszauber_fern_condition;
	information = pc_eiszauber_fern_info;
	permanent = TRUE;
	description = "Eiszauber auf Fernkampfwaffe anwenden!";
};


func int pc_eiszauber_fern_condition()
{
	if (ZAUBERART != 2) {
		return FALSE;
	}; 
	
	if (!MAGICWEAPON_INIT_CALLED) {
		magicWeaponInit(self);
	};
	
	if (DII_GetInstanceID(MAGICWEAPON_RANGED_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)) {
		return TRUE;
	};
	
	return FALSE;
};

func void pc_eiszauber_fern_info()
{
	if (DII_IsInstanceDynamic(MAGICWEAPON_RANGED_WEAPON)) {
		Print(MAGICWEAPON_ERROR_ALREADY_ENCHANTED);
	} else if (DII_GetInstanceID(MAGICWEAPON_RANGED_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)){
		MagicWeapon_MakeEnchantedWeapon(MAGICWEAPON_RANGED_WEAPON, MAGICWEAPON_ICE, "SPELLFX_ICESWORD", " - vereist", itwr_eiszauber, MagicWeapon_RemoveIceSpell);
		MAGICWEAPON_RANGED_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
		AI_ReadyRangedWeapon(self);
		PrintScreen (MAGICWEAPON_USER_INFO, -1, -1, "font_old_20_white.tga", 2);
	}else{
		Print(MAGICWEAPON_MISSING_RANGED_WEAPON);
	};
	ZAUBERART = 0;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};


instance PC_POISONMAGIC_NAH(C_Info)
{
	npc = PC_Hero;
	nr = 100;
	condition = pc_POISONMAGIC_nah_condition;
	information = pc_POISONMAGIC_nah_info;
	permanent = TRUE;
	description = "Giftverzauberung auf Nahkampfwaffe anwenden!";
};


func int pc_POISONMAGIC_nah_condition()
{
	if (ZAUBERART != 3) {
		return FALSE;
	}; 
	
	if (!MAGICWEAPON_INIT_CALLED) {
		magicWeaponInit(self);
	};
	
	if (DII_GetInstanceID(MAGICWEAPON_MELEE_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)) {
		return TRUE;
	};
	
	return FALSE;
};

func void pc_POISONMAGIC_nah_info()
{	
	if (DII_IsInstanceDynamic(MAGICWEAPON_MELEE_WEAPON)) {
		Print(MAGICWEAPON_ERROR_ALREADY_ENCHANTED);
	} else if (DII_GetInstanceID(MAGICWEAPON_MELEE_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)){
		MagicWeapon_MakeEnchantedWeapon(MAGICWEAPON_MELEE_WEAPON, MAGICWEAPON_POISON, "SPELLFX_POISONSWORD", " - vergiftet", itwr_PoisonMagic, MagicWeapon_RemovePoisonMagic);
		MAGICWEAPON_MELEE_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
		AI_ReadyMeleeWeapon(self);
		PrintScreen (MAGICWEAPON_USER_INFO, -1, -1, "font_old_20_white.tga", 2);
	}else{
		Print(MAGICWEAPON_MISSING_MELEE_WEAPON);
	};
	
	//allow calling this dialog again
	ZAUBERART = 0;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};


instance PC_POISONMAGIC_FERN(C_Info)
{
	npc = PC_Hero;
	nr = 200;
	condition = pc_POISONMAGIC_fern_condition;
	information = pc_POISONMAGIC_fern_info;
	permanent = TRUE;
	description = "Giftverzauberung auf Fernkampfwaffe anwenden!";
};


func int pc_POISONMAGIC_fern_condition()
{
	if (ZAUBERART != 3) {
		return FALSE;
	}; 
	
	if (!MAGICWEAPON_INIT_CALLED) {
		magicWeaponInit(self);
	};
	
	if (DII_GetInstanceID(MAGICWEAPON_RANGED_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)) {
		return TRUE;
	};
	
	return FALSE;
};

func void pc_POISONMAGIC_fern_info()
{
	if (DII_IsInstanceDynamic(MAGICWEAPON_RANGED_WEAPON)) {
		Print(MAGICWEAPON_ERROR_ALREADY_ENCHANTED);
	} else if (DII_GetInstanceID(MAGICWEAPON_RANGED_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)){
		MagicWeapon_MakeEnchantedWeapon(MAGICWEAPON_RANGED_WEAPON, MAGICWEAPON_POISON, "SPELLFX_POISONSWORD", " - vergiftet", itwr_PoisonMagic, MagicWeapon_RemovePoisonMagic);
		MAGICWEAPON_RANGED_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
		AI_ReadyRangedWeapon(self);
		PrintScreen (MAGICWEAPON_USER_INFO, -1, -1, "font_old_20_white.tga", 2);
	}else{
		Print(MAGICWEAPON_MISSING_RANGED_WEAPON);
	};
	
	//allow calling this dialog again
	ZAUBERART = 0;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};