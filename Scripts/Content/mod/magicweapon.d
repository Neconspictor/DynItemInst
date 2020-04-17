


//Hlp_GetInstanceID returns -1 if itm is not valid
const string MAGICWEAPON_NOT_INITIALIZED = "";

//effect time in millisecs
const int MAGICWEAPON_EFFECT_TIME = 1000*20;

const string MAGICWEAPON_MISSING_MELEE_WEAPON = "Keine Nahkampfwaffe ausgerüstet!";
const string MAGICWEAPON_MISSING_RANGED_WEAPON = "Keine Fernkampfwaffe ausgerüstet!";
const string MAGICWEAPON_DIALOG_BACK = "Abbrechen";
const string MAGICWEAPON_ERROR_ALREADY_ENCHANTED = "Waffe ist bereits verzaubert!";
const string MAGICWEAPON_USER_INFO = "Verzauberung hält 5 Minuten an!";

var int MAGICWEAPON_INIT_CALLED;
var string MAGICWEAPON_MELEE_WEAPON;
var string MAGICWEAPON_RANGED_WEAPON;
//var int ZAUBERART;

var int weaponIsDrawn;

//.text:00731F90 ; public: class oCItem * __thiscall oCNpc::GetSlotItem(class zSTRING const &) 7544720

func int oCNpcGetSlotItem (var c_npc npc, var string slotName) {
	
	const int oCNpcGetSlotItemAddress = 7544720;
	//var zSTRING zSlotName; zSlotName = _^(_@s(slotName));
	var int itmPtr;
	
	CALL_IntParam(_@s(slotName));
	CALL_PutRetValTo(_@(itmPtr));
    CALL__thiscall(_@(npc), oCNpcGetSlotItemAddress);
	return +itmPtr;
};

func void MagicWeaponUpdateEffect(var c_item itm, var string effectName) {
	//update item's visual effect
	oCItemRemoveEffect(itm);
	itm.effect = effectName;
	oCItemInsertEffect(itm);
};


func void MAGICWEAPON_CheckEffectState() {
	
	if (!Hlp_isValidNpc(hero)) {return;};
	
	var int itmPtr;
	var oCItem itm;
	var C_Item citm;
	var int instanceID;
	var string instanceName;
	var DII_USER_DATA data;
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_SWORD");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		
		
		if (itm.effectVob != 0 && data.data[MAGICWEAPON_ENCHANTEDWEAPON]) {
		
			//MagicWeaponUpdateEffect(citm, "");
			oCItemRemoveEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_SWORD", itmPtr, 1);
		};
		
	};
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_BOW");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		
		
		if (itm.effectVob != 0 && data.data[MAGICWEAPON_ENCHANTEDWEAPON]) {
		
			//MagicWeaponUpdateEffect(citm, "");
			oCItemRemoveEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_BOW", itmPtr, 1);
		};
		
	};
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_CROSSBOW");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		
		
		if (itm.effectVob != 0 && data.data[MAGICWEAPON_ENCHANTEDWEAPON]) {
		
			//MagicWeaponUpdateEffect(citm, "");
			oCItemRemoveEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_CROSSBOW", itmPtr, 1);
		};
		
	};
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_LONGSWORD");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		
		
		if (itm.effectVob != 0 && data.data[MAGICWEAPON_ENCHANTEDWEAPON]) {
		
			//MagicWeaponUpdateEffect(citm, "");
			oCItemRemoveEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_LONGSWORD", itmPtr, 1);
		};
		
	};
	
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_LEFTHAND");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		
		if (itm.effectVob == 0 && data.data[MAGICWEAPON_ENCHANTEDWEAPON]) {
			//MagicWeaponUpdateEffect(citm, data.magicWeaponNewEffect);
			oCItemInsertEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_LEFTHAND", itmPtr, 1);
		};
	
	};
	
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_RIGHTHAND");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		
		if (itm.effectVob == 0 && data.data[MAGICWEAPON_ENCHANTEDWEAPON]) {
			//MagicWeaponUpdateEffect(citm, data.magicWeaponNewEffect);
			oCItemInsertEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_RIGHTHAND", itmPtr, 1);
		};
	};
	
};



//.text:0074CC10 ; public: int __thiscall oCNpc::EV_DrawWeapon(class oCMsgWeapon *)    7654416  6
//.text:006B2610 ; public: int __thiscall oCAniCtrl_Human::DrawWeapon1(int, int, int)  7022096  6
//.text:006B2CF0 ; public: int __thiscall oCAniCtrl_Human::DrawWeapon2(void)   7023856    6
//.text:006B3040 ; public: int __thiscall oCAniCtrl_Human::RemoveWeapon1(void) 7024704  6
//.text:006B33B0 ; public: int __thiscall oCAniCtrl_Human::RemoveWeapon2(void) 7025584  6
//.text:0074DB20 ; public: int __thiscall oCNpc::EV_RemoveWeapon(class oCMsgWeapon *) 7658272    7

func void oCNpcEV_DrawWeaponHook() {
	MEM_Warn("Called oCNpcEV_DrawWeaponHook");
	
	if (weaponIsDrawn) {
		return;
	};
	
	
	
	weaponIsDrawn = TRUE;
};

func void oCAniCtrl_Human_DrawWeapon1() {
	MEM_Warn("Called oCAniCtrl_Human_DrawWeapon1");
};

func void oCAniCtrl_Human_DrawWeapon2() {
	MEM_Warn("Called oCAniCtrl_Human_DrawWeapon2");
};

func void oCAniCtrl_Human_RemoveWeapon1() {
	MEM_Warn("Called oCAniCtrl_Human_RemoveWeapon1");
};

func void oCAniCtrl_Human_RemoveWeapon2() {
	MEM_Warn("Called oCAniCtrl_Human_RemoveWeapon2");
};

func void oCNpcEV_RemoveWeapon() {
	MEM_Warn("Called oCNpcEV_RemoveWeapon");
};

func void MAGICWEAPON_setupHooks() {
	HookEngineF(7654416, 6, oCNpcEV_DrawWeaponHook);
	HookEngineF(7022096, 6, oCAniCtrl_Human_DrawWeapon1);
	HookEngineF(7023856, 6, oCAniCtrl_Human_DrawWeapon2);
	HookEngineF(7024704, 6, oCAniCtrl_Human_RemoveWeapon1);
	HookEngineF(7025584, 6, oCAniCtrl_Human_RemoveWeapon2);
	HookEngineF(7658272, 7, oCNpcEV_RemoveWeapon);
	
	
	FF_ApplyOnce(MAGICWEAPON_CheckEffectState);
	
};


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
	
	MEM_Warn("magicWeaponInit called!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

	var c_item itm; itm = Npc_GetEquippedMeleeWeapon(npc);
	
	if (Hlp_IsValidItem(itm)) {
		MAGICWEAPON_MELEE_WEAPON = DII_GetSymbolName(Hlp_GetInstanceID(itm));
	} else {
		MAGICWEAPON_MELEE_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
	};
	
	
	itm = Npc_GetEquippedRangedWeapon(npc);
	
	if (Hlp_IsValidItem(itm)) {
		MAGICWEAPON_RANGED_WEAPON = DII_GetSymbolName(Hlp_GetInstanceID(itm));
	} else {
		MAGICWEAPON_RANGED_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
	};
	
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
	var c_item temp; temp = DII_CreateNewItem(instanceName);
	temp.effect = itm.effect;
	DII_UpdateInstance(instanceName, temp);
    
    //temp isn't needed anymore
    DII_DeleteItem(temp);
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

func int RemoveMagicWeaponEffects(var MAGICWEAPON_RemoveDesc desc) {
	//TODO

	MEM_Warn(desc.instanceName);
	
	var DII_USER_DATA data; data = DII_GetUserData(desc.instanceName); 
	
	var c_item temp; temp = DII_CreateNewItem(data.magicWeaponOldInstanceName);
	
	//temp.description = data.magicWeaponOldDesc;
	//temp.effect = data.magicWeaponOldEffect;
	
	
	DII_UpdateInstance(desc.instanceName, temp);
	//DII_ApplyInstanceChangesToAll(desc.instanceName);
	//DII_ChangeInstanceForAll(desc.instanceName, data.magicWeaponOldInstanceName);
	DII_ChangeItemsInstance(desc.instanceName, data.magicWeaponOldInstanceName);
	DII_AddProxy(desc.instanceName, data.magicWeaponOldInstanceName);
	//DII_UpdateInstance(desc.instanceName, temp);
	DII_DeleteItem(temp);
	
	/*
	data.magicWeaponNewDesc = newDesc;
	data.magicWeaponOldDesc = oldDesc;
	data.magicWeaponNewEffect = newEffect;
	data.magicWeaponOldEffect = oldEffect;
	data.magicWeaponOldInstanceName = weaponInstanceName;
	*/
	
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

func void MagicWeapon_RemoveIceSpell(var int hndl)
{
	var MAGICWEAPON_RemoveDesc desc; desc = get(hndl);
	RemoveMagicWeaponEffects(desc);
	PrintScreen ("Eisverzauberung ist erloschen!", -1, -1, "font_old_20_white.tga", 2);
	FF_RemoveData(MagicWeapon_RemoveIceSpell, hndl);
	delete(hndl);
};

func void MagicWeapon_RemoveFireSpell(var int hndl)
{

	var MAGICWEAPON_RemoveDesc desc; desc = get(hndl);
	RemoveMagicWeaponEffects(desc);
	PrintScreen ("Feuerverzauberung ist erloschen!", -1, -1, "font_old_20_white.tga", 2);
	FF_RemoveData(MagicWeapon_RemoveFireSpell, hndl);
	delete(hndl);
};

func void MagicWeapon_RemovePoisonMagic(var int hndl)
{
	var MAGICWEAPON_RemoveDesc desc; desc = get(hndl);
	RemoveMagicWeaponEffects(desc);
	PrintScreen ("Giftverzauberung ist erloschen!", -1, -1, "font_old_20_white.tga", 2);
	FF_RemoveData(MagicWeapon_RemovePoisonMagic, hndl);
	delete(hndl);
};


func void MagicWeapon_MakeEnchantedWeapon(var string weaponInstanceName, 
	var int type, 
	var string newEffect, 
	var string descriptionAddition, 
	var int scrollToRemove, 
	var func removeFunc) {
	
	
	
	var int weaponId; weaponId = DII_GetInstanceID(weaponInstanceName);
	var c_item temp; temp = DII_CreateNewItem(weaponInstanceName);
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
    
	
	/*var c_item itm; itm = Npc_GetEquippedMeleeWeapon(hero);
	
	MagicWeaponUpdateEffect(itm, newEffect);
	var oCItem itmOC; itmOC = _^(_@(itm));
	itmOC.instanz = newId;
	//oCNpc_RemoveFromSlot(hero, "ZS_SWORD", _@(itm), 1);
	oCNpc_PutInSlot(hero, "ZS_SWORD", _@(itm), 1);
	//sendEV_EquipItemMessageToNpc(hero, itm);
	
	
	return;*/
	
	
	CreateInvItems(self, newId, 1);
    
    // you have to delete two items, strangely???
	Npc_RemoveInvItems(self, weaponId, 2);
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