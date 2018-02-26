#pragma once
namespace constants
{
	static const int OCITEM_FLAG_EQUIPPED = 0x40000000;

	typedef int(__thiscall* OCNpcSetWeaponMode2)(oCNpc*, int);
	static const OCNpcSetWeaponMode2 oCNpcSetWeaponMode2 = (OCNpcSetWeaponMode2)0x00738E80;

	typedef void(__thiscall* OCItemInitByScript)(void* pThis, int, int);
	static const OCItemInitByScript oCItemInitByScript  = (OCItemInitByScript)0x00711BD0;

	typedef int(__thiscall* OCNpcEV_ForceRemoveWeapon)(oCNpc* pThis, void*);
	static const OCNpcEV_ForceRemoveWeapon oCNpcEV_ForceRemoveWeapon = (OCNpcEV_ForceRemoveWeapon)0x0074EC40;
	
	typedef oCItem*(__thiscall* OCNpcGetWeapon)(oCNpc* pThis);
	static const OCNpcGetWeapon oCNpcGetWeapon = (OCNpcGetWeapon)0x007377A0;
	
	static int(__thiscall* oCNpcSetRightHand)(oCNpc*, oCVob*) = (int(__thiscall*)(oCNpc*, oCVob*))0x0073B1C0;
	
	static int(__thiscall* oCNpcGetWeaponMode)(oCNpc*) = (int(__thiscall*)(oCNpc*))0x00738C40;
	
	static int(__thiscall* oCNpcIsMunitionAvailable)(oCNpc*, oCItem*) = (int(__thiscall*)(oCNpc*, oCItem*))0x0073C6E0;
	
	static int(__thiscall* oCNpcSetLeftHand)(oCNpc*, oCVob*) = (int(__thiscall*)(oCNpc*, oCVob*))0x0073B0C0;
	
	static int(__thiscall* oCNpcSetActiveSpellInfo)(oCNpc*, int) = (int(__thiscall*)(oCNpc*, int))0x0073CF20;
	
	static int(__thiscall* oCMag_BookDeRegister)(oCMag_Book*, int) = (int(__thiscall*)(oCMag_Book*, int))0x00475DA0;
	
	static int(__thiscall* oCMag_BookRegister)(oCMag_Book*, oCItem*, int) = (int(__thiscall*)(oCMag_Book*, oCItem*, int))0x00475BB0;
	
	static oCSpell* (__thiscall* oCSpell_GetSelectedSpell)(oCMag_Book*) = (oCSpell* (__thiscall*)(oCMag_Book*))0x00477780;
	
	static oCMag_Book* (__thiscall* oCNpcGetSpellBook)(oCNpc*) = (oCMag_Book* (__thiscall*)(oCNpc*))0x0073EA00;
	
	static void(__thiscall* oCMag_BookSetFrontSpell)(oCMag_Book*, int) = (void(__thiscall*)(oCMag_Book*, int))0x004789C0;
	
	static void(__thiscall* oCMag_DoOpen)(oCMag_Book*) = (void(__thiscall*)(oCMag_Book*))0x00478460;
	
	static int(__thiscall* oCMag_BookGetKeyByItem)(oCMag_Book*, oCItem*) = (int(__thiscall*)(oCMag_Book*, oCItem*))0x00479D30;
	
	static oCSpell* (__thiscall* oCMag_BookGetSpellByKey)(oCMag_Book*, int) = (oCSpell* (__thiscall*)(oCMag_Book*, int))0x00479C60;
	
	static int(__thiscall* oCSpellGetSpellID)(oCSpell*) = (int(__thiscall*)(oCSpell*))0x00486480;
	
	static int(__thiscall* oCMag_BookGetNoOfSpells)(oCMag_Book*) = (int(__thiscall*)(oCMag_Book*))0x00479B60;
	 
	static int(__thiscall* oCMag_BookDeRegisterItem)(oCMag_Book*, oCItem*) = (int(__thiscall*)(oCMag_Book*, oCItem*))0x00475CC0;
	
	static void(__thiscall* oCMag_BookDoClose)(oCMag_Book*) = (void(__thiscall*)(oCMag_Book*))0x004785E0;
	
	static int(__thiscall* oCMag_BookGetSelectedSpellNr)(oCMag_Book*) = (int(__thiscall*)(oCMag_Book*))0x004777D0;
	
	static void(__thiscall* oCMag_BookNextRegisterAt)(oCMag_Book*, int) = (void(__thiscall*)(oCMag_Book*, int))0x00479C40;
	
	static void(__thiscall* oCNpcMakeSpellBook)(oCNpc*) = (void(__thiscall*)(oCNpc*))0x0075F040;
	
	static void(__thiscall* oCNpcDestroySpellBook)(oCNpc*) = (void(__thiscall*)(oCNpc*))0x0075F0C0;
	
	static void(__thiscall* oCNpcOpenSpellBook)(oCNpc*, int) = (void(__thiscall*)(oCNpc*, int))0x0073E990;
	
	static int(__thiscall* oCMag_GetNoOfSpellByKey)(oCMag_Book*, int) = (int(__thiscall*)(oCMag_Book*, int))0x00479CE0;
	
	static oCSpell*(__thiscall* oCMag_BookGetSelectedSpell)(oCMag_Book*) = (oCSpell*(__thiscall*)(oCMag_Book*))0x00477780;
	
	static void(__thiscall* oCMag_BookStopSelectedSpell)(oCMag_Book*) = (void(__thiscall*)(oCMag_Book*))0x00477910;

	static void(__thiscall* oCMag_BookKillSelectedSpell)(oCMag_Book*) = (void(__thiscall*)(oCMag_Book*))0x00477A90;

	static void(__thiscall* oCItemRemoveEffect)(oCItem*) = (void(__thiscall*)(oCItem*))0x00712C00;
}