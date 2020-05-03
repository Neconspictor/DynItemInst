


//Hlp_GetInstanceID returns -1 if itm is not valid
const string MAGICWEAPON_NOT_INITIALIZED = "";

//effect time in millisecs
const int MAGICWEAPON_EFFECT_TIME = 1000*60;

const string MAGICWEAPON_MISSING_MELEE_WEAPON = "Keine Nahkampfwaffe ausgerüstet!";
const string MAGICWEAPON_MISSING_RANGED_WEAPON = "Keine Fernkampfwaffe ausgerüstet!";
const string MAGICWEAPON_DIALOG_BACK = "Abbrechen";
const string MAGICWEAPON_ERROR_ALREADY_ENCHANTED = "Waffe ist bereits verzaubert!";
const string MAGICWEAPON_USER_INFO = "Verzauberung hält für 1 Minute an!";

var int MAGICWEAPON_INIT_CALLED;
var string MAGICWEAPON_MELEE_WEAPON;
var string MAGICWEAPON_RANGED_WEAPON;
//var int MAGICWEAPON_TYPE;


// object addresses
const int oCObjectFactory = 9276912;//0x008D8DF0
const int  oCMsgManipulateClassDefPtr = 11216992;//00AB286

//function addresses
const int oCItemRemoveEffectAddress = 7416832; //0x00712C00
const int oCItemInsertEffectAddress = 7416896; //0x712C40
const int zCClassDefObjectCreatedAddress = 5942960; //005AAEB0
const int oCMsgManipulateoCMsgManipulateAddress = 7768384; //00768940
const int zCEventManagerOnMessageAddress = 7889792;//00786380
const int zCVobGetEMAddress = 6290960;//005FFE10

// constants
const int EV_EQUIPITEM_MSGTYPE = 14; //0x0E
const int OCMSGMANIPULATE_BYTE_SIZE = 124;//0x7C

var int MAGICWEAPON_TYPE;

//magic weapon stuff
const int MAGICWEAPON_BonusDamage = 30;
const int MAGICWEAPON_NONE = 0;
const int MAGICWEAPON_FIRE = 1;

//functions

//.text:00712C00 public: void __thiscall oCItem::RemoveEffect(void) proc near
func void oCItemRemoveEffect(var c_item itm) {
	//before calling the actual function check that itm has an effect to remove
	var int ptr; ptr = _@(itm);
	ptr = ptr + 832;//0x340
	if (ptr == 0) {
		Print("oCItemRemoveEffect: Item has no valid effect!");
		return;
	};

	var int this; this = _@(itm);
	CALL__thiscall(this, oCItemRemoveEffectAddress);
};

//.text:00712C40 public: void __thiscall oCItem::InsertEffect(void) proc near
func void oCItemInsertEffect(var c_item itm) {
	//before calling the actual function check that itm has no effect already
	var int ptr; ptr = _@(itm);
	ptr = ptr + 832;//0x340
	if (MEM_ReadInt(ptr) != 0) {
		Print("oCItemInsertEffect: Item has already an effect!");
		return;
	};
	var int this; this = _@(itm);
	CALL__thiscall(this, oCItemInsertEffectAddress);
};

//.text:005AAEB0 public: static void __cdecl zCClassDef::ObjectCreated(class zCObject *, class zCClassDef *) proc near
func void zCClassDefObjectCreated(var int zCObjectPtr, var int zCClassDefPtr) {
	CALL_IntParam(zCClassDefPtr);
	CALL_IntParam(zCObjectPtr);
	CALL__cdecl(zCClassDefObjectCreatedAddress);
};

//.text:00768940 public: __thiscall oCMsgManipulate::oCMsgManipulate(enum  oCMsgManipulate::TManipulateSubType, class zCVob *) proc near
func void oCMsgManipulateoCMsgManipulate(var int msgPtr, var int subType, var int vobPtr) {
	CALL_IntParam(vobPtr);
	CALL_IntParam(subType);
	CALL__thiscall(msgPtr, oCMsgManipulateoCMsgManipulateAddress);
};

//.text:00786380 public: virtual void __thiscall zCEventManager::OnMessage(class zCEventMessage *, class zCVob *)
func void zCEventManagerOnMessage(var int zCEventManagerPtr, var int msgPtr, var int vobPtr) {
	CALL_IntParam(vobPtr);
	CALL_IntParam(msgPtr);
	CALL__thiscall(zCEventManagerPtr, zCEventManagerOnMessageAddress);
};

//.text:005FFE10 public: class zCEventManager * __fastcall zCVob::GetEM(int) proc near
func int zCVobGetEM(var int zCVobPtr, var int intParam) {
	var int ret;
	CALL_PutRetValTo(_@(ret));
	CALL__fastcall(zCVobPtr, intParam, zCVobGetEMAddress);
	return +ret;
};




class MAGICWEAPON_RemoveDesc {
    var string instanceName;
};

instance MAGICWEAPON_RemoveDesc@(MAGICWEAPON_RemoveDesc);

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

//.text:006742F0 ; public: class zSTRING const & __thiscall oCVisualFX::GetName(void)const
func string oCVisualFX_GetName (var int ptr) {

	if (!ptr) { return "";};
	
	const int oCVisualFX_GetNameAddress = 6767344;
	//var zSTRING zSlotName; zSlotName = _^(_@s(slotName));
	var int zstringPtr;
	
	CALL_PutRetValTo(_@(zstringPtr));
    CALL__thiscall(ptr, oCVisualFX_GetNameAddress);
	
	return MEM_ReadString(zstringPtr);
	//_^@s(zstringPtr);
};


func void MAGICWEAPON_CheckEffectState() {
	
	if (!Hlp_isValidNpc(hero)) {return;};
	
	var int itmPtr;
	var oCItem itm;
	var C_Item citm;
	var int instanceID;
	var string instanceName;
	var DII_USER_DATA data;
	var string visualEffectName;
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_SWORD");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		visualEffectName = oCVisualFX_GetName(itm.effectVob);
		
		if (data.ints[MAGICWEAPON_ENCHANTEDWEAPON] && !Hlp_StrCmp(visualEffectName, data.magicWeaponOldEffect)) {
			itm.effect = data.magicWeaponOldEffect;
			oCItemRemoveEffect(citm);
			oCItemInsertEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_SWORD", itmPtr, 1);
			itm.effect = data.magicWeaponNewEffect;
		};
		
	};
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_BOW");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		visualEffectName = oCVisualFX_GetName(itm.effectVob);
		
		if (data.ints[MAGICWEAPON_ENCHANTEDWEAPON] && !Hlp_StrCmp(visualEffectName, data.magicWeaponOldEffect)) {
			itm.effect = data.magicWeaponOldEffect;
			oCItemRemoveEffect(citm);
			oCItemInsertEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_BOW", itmPtr, 1);
			itm.effect = data.magicWeaponNewEffect;
		};
		
	};
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_CROSSBOW");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		visualEffectName = oCVisualFX_GetName(itm.effectVob);
		
		
		if (data.ints[MAGICWEAPON_ENCHANTEDWEAPON] && !Hlp_StrCmp(visualEffectName, data.magicWeaponOldEffect)) {
			itm.effect = data.magicWeaponOldEffect;
			oCItemRemoveEffect(citm);
			oCItemInsertEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_CROSSBOW", itmPtr, 1);
			itm.effect = data.magicWeaponNewEffect;
		};
		
	};
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_LONGSWORD");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		visualEffectName = oCVisualFX_GetName(itm.effectVob);
		
		
		if (data.ints[MAGICWEAPON_ENCHANTEDWEAPON] && !Hlp_StrCmp(visualEffectName, data.magicWeaponOldEffect)) {
			itm.effect = data.magicWeaponOldEffect;
			oCItemRemoveEffect(citm);
			oCItemInsertEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_LONGSWORD", itmPtr, 1);
			itm.effect = data.magicWeaponNewEffect;
		};
		
	};
	
	
	
	itmPtr = oCNpcGetSlotItem(hero, "ZS_LEFTHAND");
	
	if (itmPtr != 0) {
		
		itm = _^(itmPtr);
		citm = _^(itmPtr);
		
		instanceID = Hlp_GetInstanceID(itm);
		instanceName = DII_GetSymbolName(instanceID);
		data = DII_GetUserData(instanceName);
		visualEffectName = oCVisualFX_GetName(itm.effectVob);
		
		if (data.ints[MAGICWEAPON_ENCHANTEDWEAPON] && !Hlp_StrCmp(visualEffectName, data.magicWeaponNewEffect)) {
			itm.effect = data.magicWeaponNewEffect;
			oCItemRemoveEffect(citm);
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
		visualEffectName = oCVisualFX_GetName(itm.effectVob);
		
		if (data.ints[MAGICWEAPON_ENCHANTEDWEAPON] && !Hlp_StrCmp(visualEffectName, data.magicWeaponNewEffect)) {
			itm.effect = data.magicWeaponNewEffect;
			oCItemRemoveEffect(citm);
			oCItemInsertEffect(citm);
			oCNpc_PutInSlot(hero, "ZS_RIGHTHAND", itmPtr, 1);
			
		};
	};
	
};


func void MAGICWEAPON_init() {	
	FF_ApplyOnce(MAGICWEAPON_CheckEffectState);
	
};


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


func void SetInstanceId(var c_item itm, var int instanceId) {
	if (!Hlp_isValidItem(itm)) {
		return;
	};
	
	var oCItem ocitm; ocitm = _^(_@(itm));
	ocitm.instanz = instanceId;
};

func void RemoveMagicWeaponEffects(var MAGICWEAPON_RemoveDesc desc) {

	
	var DII_USER_DATA data; data = DII_GetUserData(desc.instanceName); 	
	data.ints[MAGICWEAPON_ENCHANTEDWEAPON] = FALSE;
	
	var c_item temp; temp = DII_CreateNewItem(data.magicWeaponOldInstanceName);
	DII_UpdateInstance(desc.instanceName, temp);
	DII_ApplyInstanceChangesToAll(desc.instanceName);
	DII_ChangeInstanceForAll(desc.instanceName, data.magicWeaponOldInstanceName);
	DII_AddProxy(desc.instanceName, data.magicWeaponOldInstanceName);
	DII_DeleteItem(temp);
};

func void MagicWeapon_RemoveFireSpell(var int hndl)
{

	var MAGICWEAPON_RemoveDesc desc; desc = get(hndl);
	RemoveMagicWeaponEffects(desc);
	PrintScreen ("Feuerverzauberung ist erloschen!", -1, -1, "font_old_20_white.tga", 2);
	FF_RemoveData(MagicWeapon_RemoveFireSpell, hndl);
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
	

	var string newDesc; newDesc = ConcatStrings(temp.description, descriptionAddition);
	var string oldEffect; oldEffect = temp.effect;
	
	temp.description = newDesc;
	temp.effect = newEffect;
	temp.damageTotal = temp.damageTotal + MAGICWEAPON_BonusDamage;
	temp.damage[DAM_INDEX_FIRE] = temp.damage[DAM_INDEX_FIRE] + MAGICWEAPON_BonusDamage;
	temp.damageType = temp.damageType | DAM_FIRE;
	temp.text[1] = "Feuerschaden-Bonus";
	temp.count[1] = MAGICWEAPON_BonusDamage;
	temp.count[2] = temp.damageTotal ;
	var int newId;
	var string newInstanceName;
	
	//Create new instance id and new item
	newInstanceName = DII_CreateNewInstance(temp);
	newId = DII_GetInstanceID(newInstanceName);
	
    //temp isn't needed anymore
    DII_DeleteItem(temp);
	
	
	CreateInvItems(self, newId, 1);
    
    // you have to delete two items, strangely???
	Npc_RemoveInvItems(self, weaponId, 2);
	var DII_USER_DATA data;
	data = DII_GetUserData(newInstanceName);
	data.magicWeaponNewEffect = newEffect;
	data.magicWeaponOldEffect = oldEffect;
	data.magicWeaponOldInstanceName = weaponInstanceName;
	
	//sign newId as an enchanted weapon
	data.ints[MAGICWEAPON_ENCHANTEDWEAPON] = TRUE;
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
	condition = PC_MAGICWEAPON_END_CONDITION;
	information = PC_MAGICWEAPON_END_INFO;
	permanent = TRUE;
	description = MAGICWEAPON_DIALOG_BACK;
};


func int PC_MAGICWEAPON_END_CONDITION()
{
	if (MAGICWEAPON_TYPE > MAGICWEAPON_NONE)
	{
		return TRUE;
	};
};

func void PC_MAGICWEAPON_END_INFO()
{
	MAGICWEAPON_TYPE = MAGICWEAPON_NONE;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};


instance PC_FIRE_ENCHANTMENT_MELEE(C_Info)
{
	npc = PC_Hero;
	nr = 100;
	condition = PC_FIRE_ENCHANTMENT_MELEE_CONDITION;
	information = PC_FIRE_ENCHANTMENT_MELEE_INFO;
	permanent = TRUE;
	description = "Feuerverzauberung auf Nahkampfwaffe anwenden.";
};


func int PC_FIRE_ENCHANTMENT_MELEE_CONDITION()
{		
	if (MAGICWEAPON_TYPE != MAGICWEAPON_FIRE) {
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

func void PC_FIRE_ENCHANTMENT_MELEE_INFO()
{	
	var DII_USER_DATA data;
	data = DII_GetUserData(MAGICWEAPON_MELEE_WEAPON);

	if (data.ints[MAGICWEAPON_ENCHANTEDWEAPON]) {
		Print(MAGICWEAPON_ERROR_ALREADY_ENCHANTED);
	} else if (DII_GetInstanceID(MAGICWEAPON_MELEE_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)){
		MagicWeapon_MakeEnchantedWeapon(MAGICWEAPON_MELEE_WEAPON, MAGICWEAPON_FIRE, "SPELLFX_FIRESWORD", " - entflammt", ItWr_FireEnchantment, MagicWeapon_RemoveFireSpell);
		MAGICWEAPON_MELEE_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
		AI_ReadyMeleeWeapon(self);
		PrintScreen (MAGICWEAPON_USER_INFO, -1, -1, "font_old_20_white.tga", 2);
	}else{
		Print(MAGICWEAPON_MISSING_MELEE_WEAPON);
	};

	//allow calling this dialog again
	MAGICWEAPON_TYPE = MAGICWEAPON_NONE;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};


instance PC_FIRE_ENCHANTMENT_RANGED(C_Info)
{
	npc = PC_Hero;
	nr = 200;
	condition = PC_FIRE_ENCHANTMENT_RANGED_CONDITION;
	information = PC_FIRE_ENCHANTMENT_RANGED_INFO;
	permanent = TRUE;
	description = "Feuerverzauberung auf Fernkampfwaffe anwenden.";
};


func int PC_FIRE_ENCHANTMENT_RANGED_CONDITION()
{
	if (MAGICWEAPON_TYPE != MAGICWEAPON_FIRE) {
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

func void PC_FIRE_ENCHANTMENT_RANGED_INFO()
{	
	var DII_USER_DATA data;
	data = DII_GetUserData(MAGICWEAPON_RANGED_WEAPON);

	if (data.ints[MAGICWEAPON_ENCHANTEDWEAPON]) {
		Print(MAGICWEAPON_ERROR_ALREADY_ENCHANTED);
	} else if (DII_GetInstanceID(MAGICWEAPON_RANGED_WEAPON) != DII_GetInstanceID(MAGICWEAPON_NOT_INITIALIZED)){
		MagicWeapon_MakeEnchantedWeapon(MAGICWEAPON_RANGED_WEAPON, MAGICWEAPON_FIRE, "SPELLFX_FIRESWORD", " - entflammt", ItWr_FireEnchantment, MagicWeapon_RemoveFireSpell);
		MAGICWEAPON_RANGED_WEAPON = MAGICWEAPON_NOT_INITIALIZED;
		AI_ReadyRangedWeapon(self);
		PrintScreen (MAGICWEAPON_USER_INFO, -1, -1, "font_old_20_white.tga", 2);
	}else{
		Print(MAGICWEAPON_MISSING_RANGED_WEAPON);
	};
	MAGICWEAPON_TYPE = MAGICWEAPON_NONE;
	AI_StopProcessInfos (self);
	magicWeaponCreationDone(self);
};