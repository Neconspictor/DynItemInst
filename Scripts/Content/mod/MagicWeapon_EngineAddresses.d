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

var int ZAUBERART;
var C_Item FEUERZAUBERWAFFE;
var C_Item EISZAUBERWAFFE;
var C_Item POISONMAGICWEAPON;

//magic weapon stuff
const int MAGICWEAPON_BonusDamage = 30;
const int MAGICWEAPON_NONE = 0;
const int MAGICWEAPON_FIRE = 1;
const int MAGICWEAPON_ICE = 2;
const int MAGICWEAPON_POISON = 3;

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