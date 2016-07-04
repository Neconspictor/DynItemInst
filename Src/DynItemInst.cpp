/*////////////////////////////////////////////////////////////////////////////

This file is part of DynItemInst.

Copyright © 2015 David Goeth

All Rights reserved.

THE WORK (AS DEFINED BELOW) IS PROVIDED
UNDER THE TERMS OF THIS CREATIVE COMMONS
PUBLIC LICENSE ("CCPL" OR "LICENSE").
THE WORK IS PROTECTED BY COPYRIGHT AND/OR
OTHER APPLICABLE LAW. ANY USE OF THE WORK
OTHER THAN AS AUTHORIZED UNDER THIS LICENSE
OR COPYRIGHT LAW IS PROHIBITED.

BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED
HERE, YOU ACCEPT AND AGREE TO BE BOUND BY THE
TERMS OF THIS LICENSE. TO THE EXTENT THIS
LICENSE MAY BE CONSIDERED TO BE A CONTRACT,
THE LICENSOR GRANTS YOU THE RIGHTS CONTAINED
HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF
SUCH TERMS AND CONDITIONS.

Full license at http://creativecommons.org/licenses/by-nc/3.0/legalcode

/////////////////////////////////////////////////////////////////////////////*/



#include <DynItemInst.h>
#include <ObjectManager.h>
#include <Util.h>
#include <HookManager.h>
#include <Windows.h>
#include <api/g2/zcworld.h>
#include <api/g2/ocgame.h>
#include <api/g2/zcparser.h>
#include <Logger.h>
#include <api/g2/ocnpc.h>
#include <api/g2/ocnpcinventory.h>
#include <api/g2/oCObjectFactory.h>
#include <unordered_map>
#include <Configuration.h>
#include <thread>
#include <mutex>
#include <Levitation.h>

const std::string DynItemInst::SAVE_ITEM_FILE_EXT = ".SAV";
const std::string DynItemInst::SAVE_ITEM_INSTANCES  = "DII_INSTANCES";
const std::string DynItemInst::SAVE_ITEM_ADDIT = "DII_ADDIT_";
const std::string DynItemInst::SAVE_ITEM_HERO_DATA = "DII_HERO_DATA";
const std::string DynItemInst::FILE_PATERN = "\\DII_*";
bool DynItemInst::denyMultiSlot = false;
bool DynItemInst::levelChange = false;
bool DynItemInst::saveGameIsLoading = false;
bool DynItemInst::saveGameWriting = false;

DynItemInst::InstanceNames DynItemInst::instanceNames = { "DII_DUMMY_ITEM", "_NF_","_FF_" , "_RUNE_", "_OTHER_", 1, 1, 1, 1 };

std::vector<zCPar_Symbol*>* DynItemInst::symbols = new std::vector<zCPar_Symbol*>();
bool DynItemInst::showExtendedDebugInfo = false;


typedef void ( __thiscall* LoadSavegame )(void*, int, int); 
LoadSavegame loadSavegame;
typedef void ( __thiscall* WriteSavegame )(void*, int, int); 
WriteSavegame writeSavegame;
typedef int ( __thiscall* OCItemGetValue )(void*); 
OCItemGetValue oCItemGetValue;
typedef zCPar_Symbol* (__thiscall* ZCPar_SymbolTableGetSymbol)(void* pThis, int index); 
ZCPar_SymbolTableGetSymbol zCPar_SymbolTableGetSymbol;
typedef zCPar_Symbol* (__thiscall* ZCPar_SymbolTableGetSymbolString)(void* pThis, zSTRING const & symbolNaame); 
ZCPar_SymbolTableGetSymbolString zCPar_SymbolTableGetSymbolString;
typedef int (__thiscall* ZCPar_SymbolTableGetIndex)(void* pThis, zSTRING const & symbolNaame); 
ZCPar_SymbolTableGetIndex zCPar_SymbolTableGetIndex;
typedef int (__thiscall* ZCParserGetIndex)(void* pThis, zSTRING const & symbolNaame); 
ZCParserGetIndex zCParserGetIndex;
typedef int (__thiscall* CreateInstance)(void* pThis, int instanceId, void* source);
CreateInstance createInstance;
typedef void (__thiscall* OCGameLoadGame)(void* pThis, int, zSTRING const &);
OCGameLoadGame oCGameLoadGame;
typedef void ( __thiscall* OCGameLoadWorld )(void*, int, zSTRING const &); 
OCGameLoadWorld oCGameLoadWorld;
typedef void ( __thiscall* OCGameChangeLevel )(void*, zSTRING const &, zSTRING const &); 
OCGameChangeLevel oCGameChangeLevel;
typedef int ( __thiscall* OCItemMulitSlot )(void*); 
OCItemMulitSlot oCItemMulitSlot;
typedef void ( __thiscall* OCMobContainerOpen )(void*, oCNpc*); 
OCMobContainerOpen oCMobContainerOpen;


typedef void(__thiscall* ZCCameraSetFarClipZ)(void*, float);
ZCCameraSetFarClipZ zCCameraSetFarClipZ;
typedef int (__fastcall* ZCCameraScreenProjectionTouchesPortal)(void* pThis, zTBBox3D const & second, zTBBox2D const & third);
ZCCameraScreenProjectionTouchesPortal zCCameraScreenProjectionTouchesPortal;

typedef int(__fastcall* ZCCameraScreenProjectionTouchesPortalRough)(void* pThis, zTBBox3D const & second, zTBBox2D const & third);
ZCCameraScreenProjectionTouchesPortalRough zCCameraScreenProjectionTouchesPortalRough;


typedef void(__thiscall* OCItemInitByScript)(void* pThis, int, int);
OCItemInitByScript oCItemInitByScript = (OCItemInitByScript)0x00711BD0;
typedef int(__thiscall* OCNpcEV_ForceRemoveWeapon)(oCNpc* pThis, void*);
OCNpcEV_ForceRemoveWeapon oCNpcEV_ForceRemoveWeapon = (OCNpcEV_ForceRemoveWeapon)0x0074EC40;
typedef oCItem*(__thiscall* OCNpcGetWeapon)(oCNpc* pThis);
OCNpcGetWeapon oCNpcGetWeapon = (OCNpcGetWeapon)0x007377A0;
typedef int(__thiscall* OCNpcSetRightHand)(oCNpc*, oCVob*);
OCNpcSetRightHand oCNpcSetRightHand = (OCNpcSetRightHand)0x0073B1C0;
typedef int(__thiscall* OCNpcGetWeaponMode)(oCNpc*);
OCNpcGetWeaponMode oCNpcGetWeaponMode = (OCNpcGetWeaponMode)0x00738C40;
typedef int(__thiscall* OCNpcSetWeaponMode2)(oCNpc*, int);
OCNpcSetWeaponMode2 oCNpcSetWeaponMode2 = (OCNpcSetWeaponMode2)0x00738E80;
typedef int(__thiscall* OCNpcIsMunitionAvailable)(oCNpc*, oCItem*);
OCNpcIsMunitionAvailable oCNpcIsMunitionAvailable = (OCNpcIsMunitionAvailable)0x0073C6E0;
typedef int(__thiscall* OCNpcSetLeftHand)(oCNpc*, oCVob*);
OCNpcSetLeftHand oCNpcSetLeftHand = (OCNpcSetLeftHand)0x0073B0C0;
typedef int(__thiscall* OCNpcSetActiveSpellInfo)(oCNpc*, int);
OCNpcSetActiveSpellInfo oCNpcSetActiveSpellInfo = (OCNpcSetActiveSpellInfo)0x0073CF20;
typedef int(__thiscall* OCMag_BookDeRegister)(oCMag_Book*, int);
OCMag_BookDeRegister oCMag_BookDeRegister = (OCMag_BookDeRegister)0x00475DA0;
typedef int(__thiscall* OCMag_BookRegister)(oCMag_Book*, oCItem*, int);
OCMag_BookRegister oCMag_BookRegister = (OCMag_BookRegister)0x00475BB0;
typedef oCSpell* (__thiscall* OCSpell_GetSelectedSpell)(oCMag_Book*);
OCSpell_GetSelectedSpell oCSpell_GetSelectedSpell = (OCSpell_GetSelectedSpell)0x00477780;
typedef oCMag_Book* (__thiscall* OCNpcGetSpellBook)(oCNpc*);
OCNpcGetSpellBook oCNpcGetSpellBook = (OCNpcGetSpellBook)0x0073EA00;
typedef void (__thiscall* OCMagBook_SetFrontSpell)(oCMag_Book*, int);
OCMagBook_SetFrontSpell oCMag_BookSetFrontSpell = (OCMagBook_SetFrontSpell)0x004789C0;
typedef void (__thiscall* OCMag_DoOpen)(oCMag_Book*);
OCMag_DoOpen oCMag_DoOpen = (OCMag_DoOpen)0x00478460;
typedef int (__thiscall* OCMag_BookGetKeyByItem)(oCMag_Book*, oCItem*);
OCMag_BookGetKeyByItem oCMag_BookGetKeyByItem = (OCMag_BookGetKeyByItem)0x00479D30;
typedef oCSpell* (__thiscall* OCMag_BookGetSpellByKey)(oCMag_Book*, int);
OCMag_BookGetSpellByKey oCMag_BookGetSpellByKey = (OCMag_BookGetSpellByKey)0x00479C60;
typedef int (__thiscall* OCSpellGetSpellID)(oCSpell*);
OCSpellGetSpellID oCSpellGetSpellID = (OCSpellGetSpellID)0x00486480;
typedef int(__thiscall* OCMag_BookGetNoOfSpells)(oCMag_Book*);
OCMag_BookGetNoOfSpells oCMag_BookGetNoOfSpells = (OCMag_BookGetNoOfSpells)0x00479B60;
typedef int(__thiscall* OCMag_BookDeRegisterItem)(oCMag_Book*, oCItem*);
OCMag_BookDeRegisterItem oCMag_BookDeRegisterItem = (OCMag_BookDeRegisterItem)0x00475CC0;
typedef void(__thiscall* OCMag_BookDoClose)(oCMag_Book*);
OCMag_BookDoClose oCMag_BookDoClose = (OCMag_BookDoClose)0x004785E0;
typedef int(__thiscall* OCMag_BookGetSelectedSpellNr)(oCMag_Book*);
OCMag_BookGetSelectedSpellNr oCMag_BookGetSelectedSpellNr = (OCMag_BookGetSelectedSpellNr)0x004777D0;
typedef void(__thiscall* OCMag_BookNextRegisterAt)(oCMag_Book*, int);
OCMag_BookNextRegisterAt oCMag_BookNextRegisterAt = (OCMag_BookNextRegisterAt)0x00479C40;
typedef void(__thiscall* OCNpcMakeSpellBook)(oCNpc*);
OCNpcMakeSpellBook oCNpcMakeSpellBook = (OCNpcMakeSpellBook)0x0075F040;
typedef void(__thiscall* OCNpcDestroySpellBook)(oCNpc*);
OCNpcDestroySpellBook oCNpcDestroySpellBook = (OCNpcDestroySpellBook)0x0075F0C0;
typedef void(__thiscall* OCNpcOpenSpellBook)(oCNpc*, int);
OCNpcOpenSpellBook oCNpcOpenSpellBook = (OCNpcOpenSpellBook)0x0073E990;
typedef int (__thiscall* OCMag_GetNoOfSpellByKey)(oCMag_Book*, int);
OCMag_GetNoOfSpellByKey oCMag_GetNoOfSpellByKey = (OCMag_GetNoOfSpellByKey)0x00479CE0;
typedef oCSpell*(__thiscall* OCMag_BookGetSelectedSpell)(oCMag_Book*);
OCMag_BookGetSelectedSpell oCMag_BookGetSelectedSpell = (OCMag_BookGetSelectedSpell)0x00477780;
typedef void(__thiscall* OCMag_BookStopSelectedSpell)(oCMag_Book*);
OCMag_BookStopSelectedSpell oCMag_BookStopSelectedSpell = (OCMag_BookStopSelectedSpell)0x00477910;
typedef void(__thiscall* OCMag_BookKillSelectedSpell)(oCMag_Book*);
OCMag_BookKillSelectedSpell oCMag_BookKillSelectedSpell = (OCMag_BookKillSelectedSpell)0x00477A90;
typedef void(__thiscall* OCItemInsertEffect)(oCItem*);
OCItemInsertEffect oCItemInsertEffect = (OCItemInsertEffect)0x00712C40;
typedef void(__thiscall* OCItemRemoveEffect)(oCItem*);
OCItemRemoveEffect oCItemRemoveEffect = (OCItemRemoveEffect)0x00712C00;

typedef void(__thiscall* ZCVobUpdatePhysics)(void*);
ZCVobUpdatePhysics zCVobUpdatePhysics = (ZCVobUpdatePhysics)0x0061D2D0;


////.text:0064B260 public: virtual void __thiscall zCRnd_D3D::DrawPoly(class zCPolygon *) proc near
typedef void(__thiscall* ZCRnd_D3DDrawPoly)(void* pThis, void*);
ZCRnd_D3DDrawPoly zCRnd_D3DDrawPoly = (ZCRnd_D3DDrawPoly)0x0064B260;

//.text:0064AC30 public: virtual void __thiscall zCRnd_D3D::DrawPolySimple(class zCTexture *, struct zTRndSimpleVertex *, int) proc near
typedef void(__thiscall* ZCRnd_D3DDrawPolySimple)(void* pThis, void* second, void* third, int fourth);
ZCRnd_D3DDrawPolySimple zCRnd_D3DDrawPolySimple = (ZCRnd_D3DDrawPolySimple)0x0064AC30;

//.text:00650CF0 public: virtual void __thiscall zD3D_alphaPoly::Draw(int) proc near
typedef void(__thiscall* ZCRnd_alphaPolyDraw)(void* pThis, int second);
ZCRnd_alphaPolyDraw zCRnd_alphaPolyDraw = (ZCRnd_alphaPolyDraw)0x00650CF0;


typedef void(__thiscall* OCAniCtrl_HumanCheckFallStates)(void*);
OCAniCtrl_HumanCheckFallStates oCAniCtrl_HumanCheckFallStates = (OCAniCtrl_HumanCheckFallStates)0x006B5810;

//.text:00529DD0 public: int __thiscall zCPolygon::RenderPoly(int) proc near
typedef int(__thiscall* ZCPolygonRenderPoly)(void*, int);
ZCPolygonRenderPoly zCPolygonRenderPoly = (ZCPolygonRenderPoly)0x00529DD0;

//.text:005B7B20 public: int __fastcall zCPolygon::ClipToFrustum(int) proc near
typedef int(__fastcall* ZCPolygonClipToFrustum)(void*, int);
ZCPolygonClipToFrustum zCPolygonClipToFrustum = (ZCPolygonClipToFrustum)0x005B7B20;

//.text:00534B70 private: void __thiscall zCBspSector::ActivateSectorRecIndoor(struct zTBBox2D const &, class zCBspSector *, int) proc near
typedef void(__thiscall* ZCBspSectorActivateSectorRecIndoor)(void*, zTBBox2D const &, void*, int);
ZCBspSectorActivateSectorRecIndoor zCBspSectorActivateSectorRecIndoor = (ZCBspSectorActivateSectorRecIndoor)0x00534B70;

void DynItemInst::hookModule()
{
	loadSavegame = (LoadSavegame) (LOAD_SAVEGAME_ADDRESS);
	writeSavegame = (WriteSavegame) (WRITE_SAVEGAME_ADDRESS);
	oCItemGetValue = (OCItemGetValue) (OCITEM_GET_VALUE_ADDRESS);
	createInstance = (CreateInstance) (ZCPARSER_CREATE_INSTANCE);
	oCGameLoadGame = (OCGameLoadGame) OCGAME_LOAD_GAME_ADDRESS;

	oCGameChangeLevel = reinterpret_cast<OCGameChangeLevel>(OCGAME_CHANGE_LEVEL_ADDRESS);
	oCItemMulitSlot = (OCItemMulitSlot) OCITEM_MULTI_SLOT;
	oCMobContainerOpen = (OCMobContainerOpen) OCMOB_CONTAINER_OPEN;

	zCCameraSetFarClipZ = (ZCCameraSetFarClipZ) 0x0054B200;
	zCCameraScreenProjectionTouchesPortal = (ZCCameraScreenProjectionTouchesPortal)0x0054C100;//0x0054BE80;
	zCCameraScreenProjectionTouchesPortalRough = (ZCCameraScreenProjectionTouchesPortalRough)0x0054BE80;//0x0054BE80;

		//0x006521E0

	HookManager* hookManager = HookManager::getHookManager();
	hookManager->addFunctionHook((LPVOID*)&loadSavegame, loadSavegameHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&writeSavegame, writeSavegameHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHookNaked, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&createInstance, createInstanceHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameChangeLevel, oCGameChangeLevelHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHookNaked, moduleDesc);

	//hookManager->addFunctionHook((LPVOID*)&oCMag_BookSetFrontSpell, oCMag_BookSetFrontSpellHook, moduleDesc);

	/*hookManager->addFunctionHook((LPVOID*)&zCCameraSetFarClipZ, zCCameraSetFarClipZHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRnd_D3DDrawPoly, zCRnd_D3DDrawPolyHook, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&zCVobUpdatePhysics, zCVobUpdatePhysicsHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCAniCtrl_HumanCheckFallStates, oCAniCtrl_HumanCheckFallStatesHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCCameraScreenProjectionTouchesPortal, zCCameraScreenProjectionTouchesPortalHookNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCCameraScreenProjectionTouchesPortalRough, zCCameraScreenProjectionTouchesPortalHookNaked, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&zCRnd_D3DDrawPolySimple, zCRnd_D3DDrawPolySimpleHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRnd_alphaPolyDraw, zCRnd_alphaPolyDrawHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPolygonRenderPoly, zCPolygonRenderPolyHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCPolygonClipToFrustum, zCPolygonClipToFrustumHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCBspSectorActivateSectorRecIndoor, zCBspSectorActivateSectorRecIndoorHook, moduleDesc);*/

	denyMultiSlot = true;
	loadDynamicInstances();
	initAdditMemory();
	denyMultiSlot = false;
}

void DynItemInst::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&loadSavegame, loadSavegameHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&writeSavegame, writeSavegameHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemGetValue, oCItemGetValueHookNaked, moduleDesc);

	hookManager->removeFunctionHook((LPVOID*)&createInstance, createInstanceHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameLoadGame, oCGameLoadGameHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameChangeLevel, oCGameChangeLevelHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCItemMulitSlot, oCItemMulitSlotHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCMobContainerOpen, oCMobContainerOpenHookNaked, moduleDesc);

	/*hookManager->removeFunctionHook((LPVOID*)&zCCameraSetFarClipZ, zCCameraSetFarClipZHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCRnd_D3DDrawPoly, zCRnd_D3DDrawPolyHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCCameraScreenProjectionTouchesPortal, zCCameraScreenProjectionTouchesPortalHookNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCCameraScreenProjectionTouchesPortalRough, zCCameraScreenProjectionTouchesPortalHookNaked, moduleDesc);
	
	hookManager->removeFunctionHook((LPVOID*)&zCRnd_D3DDrawPolySimple, zCRnd_D3DDrawPolySimpleHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCRnd_alphaPolyDraw, zCRnd_alphaPolyDrawHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCPolygonRenderPoly, zCPolygonRenderPolyHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCPolygonClipToFrustum, zCPolygonClipToFrustumHook, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCBspSectorActivateSectorRecIndoor, zCBspSectorActivateSectorRecIndoorHook, moduleDesc);
	*/

};


_declspec(naked) void DynItemInst::loadSavegameHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*7 - 5 = 2 Bytes for remaining opcode */
			nop
			nop
			/*finally hook function call*/
			jmp DynItemInst::loadSavegameHook
	}
}

_declspec(naked) void DynItemInst::writeSavegameHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Byte for remaining opcode */
			nop
			/*finally hook function call*/
			jmp DynItemInst::writeSavegameHook
	}
}


_declspec(naked) void DynItemInst::oCItemGetValueHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*9 - 5 = 4 Bytes for remaining opcode */
		nop
		nop
		nop
		nop
		/*finally hook function call*/
		jmp DynItemInst::oCItemGetValueHook	
	}
}

_declspec(naked) void DynItemInst::createInstanceHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*5 - 5 = 0 Bytes for remaining opcode */
		/*finally hook function call*/
		jmp DynItemInst::createInstanceHook
	}
}

_declspec(naked) void DynItemInst::oCGameLoadGameHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*5 - 5 = 0 Bytes for remaining opcode */
		/*finally hook function call*/
			jmp DynItemInst::oCGameLoadGameHook
	}
}

_declspec(naked) void DynItemInst::oCGameChangeLevelHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*7 - 5 = 2 Bytes for remaining opcode */
		nop
		nop
		/*finally hook function call*/
			jmp DynItemInst::oCGameChangeLevelHook
	}
}

_declspec(naked) void DynItemInst::oCItemMulitSlotHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Byte for remaining opcode */
			nop
			/*finally hook function call*/
			jmp DynItemInst::oCItemMulitSlotHook
	}
}

_declspec(naked) void DynItemInst::oCMobContainerOpenHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Byte for remaining opcode */
			nop
			/*finally hook function call*/
			jmp DynItemInst::oCMobContainerOpenHook
	}
}

int DynItemInst::zCCameraScreenProjectionTouchesPortalHookNaked(void* pThis, zTBBox3D const & second, zTBBox2D const & third)
{
	return zCCameraScreenProjectionTouchesPortalRough(pThis, second, third);
	//logStream << "DynItemInst::zCCameraScreenProjectionTouchesPortalHookNaked: result = " << result << std::endl;
	//util::debug(&logStream);
	//return 1;
}


int DynItemInst::oCItemGetValueHook(void* pThis) {
	oCItem* item = static_cast<oCItem*>(pThis);
	ObjectManager* manager = ObjectManager::getObjectManager();
	//if (manager->getDynInstanceId(item) > ObjectManager::INSTANCE_BEGIN) {
	if (manager->IsModified(item)) {
		return item->value;
	}
	return oCItemGetValue(pThis);
};



zCPar_Symbol* DynItemInst::zCPar_SymbolTableGetSymbolHook(void* pThis, int index)
{
	zCPar_Symbol* result = zCPar_SymbolTableGetSymbol(pThis, index);
	if (result == nullptr)
	{
		result = ObjectManager::getObjectManager()->getSymbolByIndex(index);
	}

	return result;
}

zCPar_Symbol* DynItemInst::zCPar_SymbolTableGetSymbolStringHook(void* pThis, zSTRING const & symbolName)
{
	zCPar_Symbol* result = ObjectManager::getObjectManager()->getSymbolByName(symbolName);
	if (result == nullptr)
	{
		result = zCPar_SymbolTableGetSymbolString(pThis, symbolName);
	}
	return result;
}

int DynItemInst::zCPar_SymbolTableGetIndexHook(void* pThis, zSTRING const& symbolName)
{
	int result = zCPar_SymbolTableGetIndex(pThis, symbolName);
	if (result == NULL)
	{
		result = ObjectManager::getObjectManager()->getIndexByName(symbolName);
	} 
	return result;
}

int DynItemInst::zCParserGetIndexHook(void* pThis, zSTRING const& symbolName)
{
	int result = ObjectManager::getObjectManager()->getIndexByName(symbolName);
	if (result == NULL)
	{
		result = zCParserGetIndex(pThis, symbolName);	
	}
	return result;
}

DynItemInst::DynItemInst()
	:Module()
{
	moduleDesc = "DynItemInst";
}

DynItemInst::~DynItemInst()
{
}


 void DynItemInst::loadSavegameHook(void* pThis,int saveGameSlotNumber, int b)
{   
	logStream << "DynItemInst::loadSavegameHook: load savegame..." << std::endl;
	util::logInfo(&logStream);
	saveGameIsLoading = true;
	denyMultiSlot = true;
	ObjectManager::getObjectManager()->releaseInstances();
	loadSavegame(pThis, saveGameSlotNumber, b);
	loadDynamicInstances();
	initAdditMemory();
	denyMultiSlot = false;
	saveGameIsLoading = false;

	logStream << "DynItemInst::loadSavegameHook: done." << std::endl;
	util::logInfo(&logStream);
};


 void DynItemInst::writeSavegameHook(void* pThis,int saveGameSlotNumber, int b)
{   
	logStream << "DynItemInst::writeSavegameHook: save game..." << std::endl;
	util::logInfo(&logStream);

	saveGameWriting = true;
	// reset instance name counters
	instanceNames.nearFightCounter = 1;
	instanceNames.distanceFightCounter = 1;
	instanceNames.runeCounter = 1;
	instanceNames.otherCounter = 1;

	oCGame* game = oCGame::GetGame();
	zCWorld* world = game->GetWorld();
	oCWorld* ocworld = game->GetGameWorld();
	ObjectManager* manager = ObjectManager::getObjectManager();
	
	zCListSort<oCNpc>* npcList = world->GetNpcList();

	denyMultiSlot = true;

	oCNpc* hero = oCNpc::GetHero();
	while(npcList != nullptr) {
		oCNpc* npc = npcList->GetData();
		if (npc == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}
		bool isHero = npc == hero;
		if (isHero && DynItemInst::levelChange) {
			npcList = npcList->GetNext();
			continue;
		}
		oCNpcInventory* inventory = npc->GetInventory();
		if (inventory == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}
		inventory->UnpackAllItems();
		zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
		oCMag_Book* magBook = oCNpcGetSpellBook(npc);
		int selectedSpellKey = -1;
		if (magBook) {
			oCSpell* selectedSpell = oCMag_BookGetSelectedSpell(magBook);
			if (selectedSpell)
			{
				selectedSpellKey = oCSpellGetSpellID(selectedSpell);
				//oCMag_BookStopSelectedSpell(magBook);
				oCMag_BookKillSelectedSpell(magBook);

			}
			logStream << "DynItemInst::writeSavegameHook: selectedSpellKey=" << selectedSpellKey<< std::endl;
			util::debug(&logStream);
		} else
		{
			logStream << "DynItemInst::writeSavegameHook: magBook is null!" << std::endl;
			util::debug(&logStream, Logger::Warning);
		}

		resetInstanceNameStruct();

		while(list != nullptr) {
			oCItem* item = list->GetData();
			int id = modifyItemForSaving(item, isHero);
			int equiped = id && item->HasFlag(OCITEM_FLAG_EQUIPPED);
			
			magBook = oCNpcGetSpellBook(npc);
			int spellKey; 
			if (magBook) {
				spellKey = oCMag_BookGetKeyByItem(magBook, item);
			} else
			{
				spellKey = -1;
			}
			if (!equiped)
			{
				spellKey = -1;
			}
			// selected spell key begins at 0, but spell key strangely at 1 
			bool activeSpellItem = (selectedSpellKey  == (item->spell));
			logStream << "DynItemInst::writeSavegameHook: create addit memory= " << (id > 0) << std::endl;
			util::debug(&logStream);
			if (id) manager->createAdditionalMemory(item, id, isHero, activeSpellItem, spellKey);
			if (equiped)
			{
				logStream << "DynItemInst:: Item is equipped! " << std::endl;
				util::debug(&logStream);
				//Deny invocation of equip function
				int unEquipFunction = item->on_unequip;
				item->on_unequip = 0;

				//unequip item
				npc->Equip(item);

				//restore functions
				item->on_unequip = unEquipFunction;
				
				//Mark item as equipped
				item->SetFlag(OCITEM_FLAG_EQUIPPED);
			}

			list = list->GetNext();
		}
		npcList = npcList->GetNext();
	}
	
	zCListSort<oCItem>* itemList = world->GetItemList();
	while(itemList != nullptr) {
		oCItem* item = itemList->GetData();
		int id = modifyItemForSaving(item, false);
		if (id) manager->createAdditionalMemory(item, id, false);
		itemList = itemList->GetNext();
	}

	denyMultiSlot = false;

	//finally write the savegame and restore items that are reseted by the savegame writing method
	std::string saveGameDir;
	if (saveGameSlotNumber != -1)
	{
		saveGameDir = manager->getSaveGameDirectoryPath(saveGameSlotNumber);	
	} else
	{
		saveGameDir = manager->getCurrentDirectoryPath();
	}

	std::string currentDir = manager->getCurrentDirectoryPath();

	util::copyContentTo(currentDir,saveGameDir, FILE_PATERN);
	//util::copyContentTo(saveGameDir,currentDir, FILE_PATERN);

	zSTRING worldName = ocworld->GetWorldName();
	std::string saveAddit = SAVE_ITEM_ADDIT + std::string(const_cast<char*>(worldName.ToChar())) + 
		SAVE_ITEM_FILE_EXT; 
	std::string saveInstances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;
	std::string heroData = SAVE_ITEM_HERO_DATA + SAVE_ITEM_FILE_EXT;
	
	// Write actual savegame
	writeSavegame(pThis, saveGameSlotNumber, b);
	
	std::list<AdditMemory*> heroItemList;
	manager->getHeroAddits(heroItemList);
	int heroItemSize = heroItemList.size();
	manager->saveHeroData(heroItemList, const_cast<char*>(saveGameDir.c_str()), const_cast<char*>(heroData.c_str()));
	heroItemList.clear();

	manager->saveNewInstances(const_cast<char*>(saveGameDir.c_str()), const_cast<char*>(saveInstances.c_str()));
	manager->saveWorldObjects(heroItemSize, const_cast<char*>(saveGameDir.c_str()), const_cast<char*>(saveAddit.c_str()));

	util::copyFileTo(saveGameDir + std::string("\\") +  saveInstances, currentDir  + 
		std::string("\\") + saveInstances);
	util::copyFileTo(saveGameDir + std::string("\\") + saveAddit, currentDir  + 
		std::string("\\") + saveAddit);
	util::copyFileTo(saveGameDir + std::string("\\") + heroData, currentDir  + 
		std::string("\\") + heroData);

	restoreDynamicInstances(game);
	manager->removeAllAdditionalMemory();	

	saveGameWriting = false;

	logStream << "DynItemInst::writeSavegameHook: done." << std::endl;
	util::logInfo(&logStream);
};

 void DynItemInst::restoreItem(oCItem* item,  oCNpcInventory* inventory, std::unordered_map<int, oCItem*>* equippedSpells, oCItem** activeSpellItem) {
	if (item == nullptr) return;
	if (item->instanz < 0) {
		ObjectManager* manager = ObjectManager::getObjectManager();
		int additId = -item->instanz;
		AdditMemory* addit = manager->getAddit(additId);
		//mark additKey as visited
		if (addit == nullptr || addit->visited) {
			logStream << "DynItemInst::restoreItem: Addit is null or was already visited!!!" << std::endl;
			logStream << item->name.ToChar() << " : " << additId << std::endl;
			util::logFault(&logStream);
			return;
		}

		//Each item should only once be restored
		addit->visited = true;

		int instanceId; 
		if (addit->instanceId == manager->getIdForSpecialPurposes())
		{
			instanceId = item->GetInstance();
		} else
		{
			instanceId = addit->instanceId;
		}

		int instanz = addit->instanz;
		item->instanz = instanz;

		zCWorld* world = oCGame::GetGame()->GetWorld();

		// is item located in the world?
		if (inventory == nullptr)
		{
			oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceId);
			world->AddVob(copy);
			copy->SetPositionWorld(item->GetVobPosition());
			world->RemoveVob(item);
			return;
		}

		bool originalMultiSlotSetting = denyMultiSlot;
		denyMultiSlot = true;

		// is the item equipped?
		int equipped = item->HasFlag(OCITEM_FLAG_EQUIPPED);

		if (equipped)
		{

			logStream << "DynItemInst::restoreItem: Restore equipped item..." << std::endl;
			util::debug(&logStream);

			oCNpc* owner = inventory->GetOwner();
			int weaponMode = oCNpcGetWeaponMode(owner);

			if (isReadiedWeapon(weaponMode, item) && !item->HasFlag(512))
			{
				logStream << "DynItemInst::restoreItem: Force to remove weapon..." << std::endl;
				util::debug(&logStream);
				oCNpcEV_ForceRemoveWeapon(owner, item);
			}

			int amount = item->instanz;
			if (amount != 1)
			{
				logStream << "DynItemInst::restoreItem: amount > 1!";
				util::logFault(&logStream);
			}
		
			zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, instanceId);
			oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceId);
			int munitionAvailable = 0;

			if (!item->HasFlag(512)) //item isn't a rune
			{
				int slotNumber = getSlotNumber(inventory, item);
				util::assertDIIRequirements(slotNumber >= 0, "slotNumber >= 0");
				logStream << "DynItemInst::restoreItem: slotnumber= " << slotNumber << std::endl;
				logStream << "DynItemInst::restoreItem: item->description= " << item->description.ToChar() << std::endl;
				logStream << "DynItemInst::restoreItem: item->GetInstance()= " << item->GetInstance() << std::endl;
				logStream << "DynItemInst::restoreItem: copy->description= " << copy->description.ToChar() << std::endl;
				logStream << "DynItemInst::restoreItem: copy->GetInstance()= " << copy->GetInstance() << std::endl;
				util::debug(&logStream);

				inventory->Remove(item);
				
				//store some attribute to search for the copy after it was inserted into the inventory
				int copyStoreValue = copy->instanz;
				//assign any value that will be unique
				int searchValue = -6666666;
				copy->instanz = searchValue;

				//DynItemInst::denyMultiSlot = false;
				inventory->Insert(copy);

				// Since multi-slotting was denied, copy is now on a own slot (not merged) and can be accessed
				copy = searchItemInInvbyInstanzValue(inventory, searchValue);
				util::assertDIIRequirements(copy != nullptr, "item to insert shouldn't be null!");
				copy->instanz = copyStoreValue;
				//Deny invocation of equip function
				int equipFunction = copy->on_equip; 
				copy->on_equip = 0;
				copy->ClearFlag(OCITEM_FLAG_EQUIPPED);
				owner->Equip(copy);

				//restore function

				logStream << "DynItemInst::restoreItem: item is now equipped!" << std::endl;
				logStream << "DynItemInst::restoreItem: Weapon mode: " << weaponMode << std::endl;
				util::debug(&logStream);
				copy = getInvItemByInstanceId(inventory, instanceId)->GetData();
				copy->on_equip = equipFunction;
				oCNpcSetWeaponMode2(owner, weaponMode);  //3 for one hand weapons
				munitionAvailable = oCNpcIsMunitionAvailable(owner, copy);
			} else
			{
				oCItemInitByScript(item, instanceId, item->instanz);
				item->ClearFlag(OCITEM_FLAG_EQUIPPED);
			}

			if (munitionAvailable)
			{
				logStream << "DynItemInst::restoreItem: Munition is available." << std::endl;
				util::debug(&logStream);
			}

			// Is readied weapon a bow?
			if (copy && copy->HasFlag(1 << 19) && weaponMode == 5)
			{
				logStream << "DynItemInst::restoreItem: Bow is readied!" << std::endl;
				logStream << "DynItemInst::restoreItem: Weapon mode: " << weaponMode << std::endl;
				util::debug(&logStream);

				updateRangedWeapon(copy, inventory, true);
			}
			else if (copy && copy->HasFlag(1 << 20) && munitionAvailable && weaponMode == 6)
			{
				logStream << "DynItemInst::restoreItem: Crossbow is readied!" << std::endl;
				logStream << "DynItemInst::restoreItem: Weapon mode: " << weaponMode << std::endl;
				util::debug(&logStream);

				updateRangedWeapon(copy, inventory, false);
			}
			else if (item && item->HasFlag(512)) // Magic 
			{
				logStream << "DynItemInst::restoreItem: Readied weapon is a magic thing!" << std::endl;
				util::debug(&logStream);
				oCMag_Book* magBook = oCNpcGetSpellBook(owner);
				magBook = oCNpcGetSpellBook(owner);
				int itemSpellKey = oCMag_BookGetKeyByItem(magBook, item);
				if (itemSpellKey <= 7)
				{
					oCMag_BookDeRegisterItem(magBook, item);
					oCMag_BookNextRegisterAt(magBook, itemSpellKey-1);
				}
				if (addit->spellKey >= 0)
				{
					if (!equippedSpells)
					{
						logStream << "DynItemInst::restoreItem: equippedSpells is null!" << std::endl;
						util::debug(&logStream, Logger::Warning);
					} else
					{
						equippedSpells->insert(std::pair<int, oCItem*>(addit->spellKey, item));
					}
				}

				magBook = oCNpcGetSpellBook(owner);
				if (magBook)
				{	
					if (addit->activeSpellItem && activeSpellItem)
					{
						*activeSpellItem = item;
					}
					
					logStream << "DynItemInst::restoreItem: selectedSpellKey = " << oCMag_BookGetSelectedSpellNr(magBook) << std::endl;
					util::debug(&logStream);
					logStream << "DynItemInst::restoreItem: An Spell is active" << std::endl;
					util::debug(&logStream);
				}
			}

			logStream << "DynItemInst::restoreItem: Restored equipped item!" << std::endl;
			util::debug(&logStream);
			denyMultiSlot = originalMultiSlotSetting;
			return;
		}
		
		oCItemInitByScript(item, instanceId, item->instanz);
		item->ClearFlag(OCITEM_FLAG_EQUIPPED);
		inventory->Remove(item, item->instanz);
		inventory->Insert(item);
		denyMultiSlot = originalMultiSlotSetting;
	}
}


 int DynItemInst::modifyItemForSaving(oCItem* item, bool isHeroItem) {
	if (item == nullptr) return NULL;
	ObjectManager* manager = ObjectManager::getObjectManager();

	int id = manager->getDynInstanceId(item);

	// make exception for runes
	if ((id == NULL) && item->HasFlag(OCITEM_FLAG_ITEM_KAT_RUNE) && item->HasFlag(OCITEM_FLAG_EQUIPPED)) {
		return manager->getIdForSpecialPurposes();
	}

	if (id == NULL) return NULL;

	manager->oCItemSaveRemoveEffect(item);
	
	zCParser* parser = zCParser::GetParser();
	int saveId; 
	
	if (item->HasFlag(OCITEM_FLAG_EQUIPPED))
	{
		logStream << "DynItemInst::modifyItemForSaving: item->description: " << item->description.ToChar() << std::endl;
		util::debug(&logStream);
		std::stringstream ss;
		if (item->HasFlag(2)) //near fight
		{
			ss << instanceNames.base << instanceNames.nearFight << instanceNames.nearFightCounter;
			++instanceNames.nearFightCounter;
		} else if (item->HasFlag(4)) // distance fight
		{
			ss << instanceNames.base << instanceNames.distanceFight << instanceNames.distanceFightCounter;
			++instanceNames.distanceFightCounter;
		} else if (item->HasFlag(512)) // runes 
		{
			ss << instanceNames.base << instanceNames.rune << instanceNames.runeCounter;
			++instanceNames.runeCounter;
		}
		else // other
		{
			ss << instanceNames.base << instanceNames.other << instanceNames.otherCounter;
			++instanceNames.otherCounter;
		}

		std::string name = ss.str();
		saveId = parser->GetIndex(name.c_str());

		if (saveId <= 0)
		{
			logStream << "instance name not found: " << name << std::endl;
			util::logFatal(&logStream);
			//throw new DynItemInst::DII_InstanceNameNotFoundException(ss.str().c_str());
		}
		
	} else
	{
		saveId = parser->GetIndex("DII_DUMMY_ITEM");
	}
	manager->setInstanceId(item, saveId);
	return id;
}


 oCItem* DynItemInst::makeEquippedCopy(oCItem* item, oCNpcInventory* inventory)
 {
	 if (item->instanz >= 0) {
		 return item;
	 }
	 ObjectManager* manager = ObjectManager::getObjectManager();
	 int additId = -item->instanz;
	 AdditMemory* addit = manager->getAddit(additId);
	 if (addit == nullptr) {
		 logStream << "DynItemInst::makeEquippedCopy: Warning: Addit is null!!!" << std::endl;
		 logStream << item->name.ToChar() << " : " << additId << std::endl;
		 util::debug(&logStream, Logger::Fault);
		 return nullptr;
	 }

	 int instanceId = addit->instanceId;
	 int instanz = addit->instanz;
	 item->instanz = instanz;

	 zCWorld* world = oCGame::GetGame()->GetWorld();

	 oCNpc* owner = inventory->GetOwner();
	 int amount = item->instanz;
	 
	 //TODO: deny invocation od equip function maybe?
	  owner->EquipItem(item);
	 inventory->Remove(item, item->instanz);
	 zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, instanceId);
	 oCItem* copy = oCObjectFactory::GetFactory()->CreateItem(instanceId);
	 if (list != nullptr)
	 {
		 item = list->GetData();
		 copy->instanz = item->instanz + amount;
		 inventory->Remove(item, item->instanz);
	 } else
	 {
		 copy->instanz = amount;
	 }

	 return copy;
 }

bool DynItemInst::itemsAreModified()
{
	return isSaveGameLoading() || levelChange || saveGameWriting;
}

void DynItemInst::zCCameraSetFarClipZHook(void* pThis, float value)
{
	int targetValue = 60000.0f;
	int newValue = 100000.0f;
	int secondValue = 20000.0f;
	if (value != targetValue)
	{
		value *= Configuration::getFarClipZMultiplicator();
	}

	zCCameraSetFarClipZ(pThis, value);
}

void DynItemInst::zCRnd_D3DDrawPolyHook(void* pThis, void* poly)
{

	zCRnd_D3DDrawPoly(pThis, poly);
}

void DynItemInst::zCRnd_D3DDrawPolySimpleHook(void* pThis, void* second, void* third, int fourth)
{
	zCRnd_D3DDrawPolySimple(pThis, second, third, fourth);
}

void DynItemInst::zCRnd_alphaPolyDrawHook(void* pThis, int second)
{
	//zCRnd_alphaPolyDraw(pThis, second);
}

void DynItemInst::zCVobUpdatePhysicsHook(void * pThis)
{
	if (oCNpc::GetHero() != pThis)
	{
		//zCVobUpdatePhysics(pThis);
	}
	zCVobUpdatePhysics(pThis);
}

void DynItemInst::oCAniCtrl_HumanCheckFallStatesHook(void * oCAniCtrl_Human)
{
	if (oCNpc::GetHero() != (zCVob*)((BYTE*)oCAniCtrl_Human + 0x64))
	{
		
	}
	oCAniCtrl_HumanCheckFallStates(oCAniCtrl_Human);
}

int DynItemInst::zCPolygonRenderPolyHook(void* pThis, int second)
{
	//return zCPolygonRenderPoly(pThis, second);
	zCPolygon* poly = (zCPolygon*)pThis;
	if (poly && zCMaterial::IsPortalMaterial(poly->GetMaterial())) {
		return 0;
	}
	return zCPolygonRenderPoly(pThis, second);
}

bool change = false;

int DynItemInst::zCPolygonClipToFrustumHook(void* pThis, int second)
{
	//.text:005B7B20 public: int __fastcall zCPolygon::ClipToFrustum(int) proc near
	if (change) {
		return 1;
	}
	return zCPolygonClipToFrustum(pThis, second);;
}

void DynItemInst::zCBspSectorActivateSectorRecIndoorHook(void* pThis, zTBBox2D const& second, void* third, int fourth)
{
	change = true;
	zCBspSectorActivateSectorRecIndoor(pThis, second, third, fourth);
	change = false;
}

void DynItemInst::restoreDynamicInstances(oCGame* game) {
	logStream << "DynItemInst::restoreDynamicInstances: restore... "  << std::endl;
	util::logInfo(&logStream);
	denyMultiSlot = true;
	zCWorld* world = game->GetWorld();
	zCListSort<oCNpc>* npcList = world->GetNpcList();
	ObjectManager* manager = ObjectManager::getObjectManager();
	std::list<oCItem*> tempList;
	std::list<oCItem*> equippedItems;
	std::list<oCItem*>::iterator it;

	while(npcList != nullptr) {
		oCNpc* npc = npcList->GetData();
		if (npc == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}
		
		oCNpcInventory* inventory = npc->GetInventory();
		if (inventory == nullptr) {
			npcList = npcList->GetNext();
			continue;
		}

		inventory->UnpackAllItems();
		zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
		while(list != nullptr) {
			oCItem* item = list->GetData();
			if (item != nullptr) tempList.push_back(item);

			list = list->GetNext();
		}
		npcList = npcList->GetNext();

		for (it = tempList.begin(); it != tempList.end(); ++it)
		{
			oCItem* item = *it;
			if (item == nullptr) continue;
			int equiped = 0;
			if (item->instanz < 0) {
				int additId = -item->instanz;
				AdditMemory* addit = manager->getAddit(additId);
				util::assertDIIRequirements(addit, "DynItemInst::restoreDynamicInstances: addit != nullptr");
				equiped = addit->flags & OCITEM_FLAG_EQUIPPED;
			}
			if (equiped)
			{
				item->SetFlag(OCITEM_FLAG_EQUIPPED);
				equippedItems.push_back(*it);
				int instanceId = (*it)->GetInstance();
				logStream << "DynItemInst::restoreDynamicInstances: item with following id is marked as equipped: " << instanceId << std::endl;
				util::debug(&logStream);
				//control that instanceId is that of a marked equipped item
				int checkInstanceId = zCParser::GetParser()->GetIndex(zSTRING(instanceNames.base.c_str()));
				util::assertDIIRequirements(instanceId != checkInstanceId, "instanceId != checkInstanceId");
				
			} else
			{
				restoreItem(*it, inventory);
			}
		}

		oCItem* temp = nullptr;
		oCItem** selectedSpellItem = &temp;
		std::unordered_map<int, oCItem*> equippedSpells;

		for (it = equippedItems.begin(); it != equippedItems.end(); ++it)
		{
			restoreItem(*it, inventory, &equippedSpells, selectedSpellItem);
		}

		tempList.clear();
		equippedItems.clear();
		
		for (auto it2 = equippedSpells.begin(); it2 != equippedSpells.end(); ++it2)
		{
			int key = it2->first;
			oCItem* item = it2->second;
			oCMag_Book* magBook = npc->GetSpellBook();
			oCMag_BookNextRegisterAt(magBook, key);
			npc->Equip(item);
		}
		equippedSpells.clear();

		if (*selectedSpellItem)
		{
			logStream << "DynItemInst::restoreDynamicInstances: SET SELECTED SPELL KEY!!!!" << std::endl;
			util::debug(&logStream);
			oCMag_Book* magBook = npc->GetSpellBook();
			int itemSpellKey = oCMag_BookGetKeyByItem(magBook, *selectedSpellItem);
			int noOfSpellKey = oCMag_GetNoOfSpellByKey(magBook, itemSpellKey);
			oCSpell* spell = oCMag_BookGetSpellByKey(magBook, itemSpellKey);
			

			logStream << "DynItemInst::restoreDynamicInstances: itemSpellKey = " << itemSpellKey << std::endl;
			util::debug(&logStream);
			logStream << "DynItemInst::restoreDynamicInstances: itemSpellKey = " << spell << std::endl;
			util::debug(&logStream);
			int weaponMode = oCNpcGetWeaponMode(npc);
			if (weaponMode == 7)
			{
				oCNpcEV_ForceRemoveWeapon(npc, *selectedSpellItem);
				oCMag_BookKillSelectedSpell(magBook);
				oCMag_BookSetFrontSpell(magBook, noOfSpellKey - 1);
				oCNpcSetWeaponMode2(npc, weaponMode);
			}
		}
		selectedSpellItem = nullptr;
	}

	tempList.clear();
	zCListSort<oCItem>* itemList = world->GetItemList();
	while(itemList != nullptr) {
		oCItem* item = itemList->GetData();
		if (item != nullptr)
		{
			tempList.push_back(item);	
		}
		itemList = itemList->GetNext();
	}

	it = tempList.begin();
	for (; it != tempList.end(); ++it)
	{
		restoreItem(*it);
	}
	tempList.clear();

	denyMultiSlot = false;

	logStream << "DynItemInst::restoreDynamicInstances: done." << std::endl;
	util::logInfo(&logStream);
}

bool DynItemInst::isSaveGameLoading()
{
	return saveGameIsLoading;
};


 int DynItemInst::createInstanceHook(void* pThis, int instanceId, void* source)
{
	zCPar_Symbol* symbol = zCParser::GetParser()->GetSymbol(instanceId);
	if (symbol == nullptr)
	{
		logStream << "DynItemInst::createInstanceHook: symbol is null! InstanceId: " << instanceId << std::endl;
		util::debug(&logStream, Logger::Warning);
	}

	int result = createInstance(pThis, instanceId, source);;

	int instanceBegin = ObjectManager::getObjectManager()->getInstanceBegin();
	bool isTarget = symbol && (instanceId >= instanceBegin) && (instanceBegin > 0);
	if (isTarget)
	{
		oCItem* item = (oCItem*)source;
		ObjectManager* manager = ObjectManager::getObjectManager();
		manager->InitItemWithDynInstance(item, instanceId);	
		result = manager->getDynInstanceId(item);
	}

	return result;
}

void DynItemInst::oCGameLoadGameHook(void* pThis, int second, zSTRING const& worldName)
{
	logStream << "DynItemInst::oCGameLoadGameHook: load..."<< std::endl;
	util::logInfo(&logStream);
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	oCGameLoadGame(pThis, second, worldName);

	logStream << "DynItemInst::oCGameLoadGameHook: done." << std::endl;
	util::logInfo(&logStream);
}

void __thiscall DynItemInst::oCGameChangeLevelHook(void* pThis, zSTRING const & first, zSTRING const & second) {
	logStream << "DynItemInst::oCGameChangeLevelHook: change level..." << std::endl;
	util::logInfo(&logStream);

	levelChange = true;
	ObjectManager* manager = ObjectManager::getObjectManager();
	oCNpcInventory* inv = oCNpc::GetHero()->GetInventory();
	inv->UnpackAllItems();
	std::list<oCItem*> tempList;
	std::list<int> idList;
	std::list<oCItem*>::iterator it;
	std::list<int>::iterator idIt;
	zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inv->inventory_data);
	while(list != nullptr)
	{
		oCItem* item = list->GetData();
		if (item != nullptr)
		{
			if (item->HasFlag(OCITEM_FLAG_EQUIPPED))
			{
				tempList.push_back(item);
				idList.push_back(manager->getInstanceId(*item));
			}
		}
		list = list->GetNext();
	}

	for (it = tempList.begin(); it != tempList.end(); ++it)
	{
		oCNpc::GetHero()->Equip(*it);
	}
	tempList.clear();

	oCGameChangeLevel(pThis, first, second);
	inv = oCNpc::GetHero()->GetInventory();
	inv->UnpackAllItems();
	
	for (idIt = idList.begin(); idIt != idList.end(); ++idIt)
	{
		oCItem* item = getInvItemByInstanceId(inv, *idIt)->GetData();
		oCNpc::GetHero()->Equip(item);
	}
	idList.clear();
	levelChange = false;
	initAdditMemory();

	logStream << "DynItemInst::oCGameChangeLevelHook: done." << std::endl;
	util::logInfo(&logStream);
}

int DynItemInst::oCItemMulitSlotHook(void* pThis)
{
	if (denyMultiSlot)
	{
		return 0;
	}
	return oCItemMulitSlot(pThis);
}

void DynItemInst::oCMobContainerOpenHook(void* pThis, oCNpc* npc)
{
	oCMobContainer* container = (oCMobContainer*) pThis;
	ObjectManager* manager = ObjectManager::getObjectManager();
	int address = (int)container->containList_next;
	zCListSort<oCItem>* listAddress = reinterpret_cast<zCListSort<oCItem>*>(address);
	zCListSort<oCItem>* list = listAddress;

	while(list != nullptr) {
		oCItem* item = list->GetData();
		if (item == nullptr) {
			list = list->GetNext();
			continue;
		}

		int instanceId = manager->getInstanceId(*item);
		bool change = false;
		if (instanceId == -1)
		{
			instanceId = manager->getIndexByName(item->name);
			change = true;
		}
		if (change && manager->IsModified(instanceId)) {
			manager->assignInstanceId(item, instanceId);
		}

		list = list->GetNext();
	}

	oCMobContainerOpen(pThis, npc);

}

void DynItemInst::oCMag_BookSetFrontSpellHook(void* pThis, int number)
{
	logStream << "DynItemInst::oCMag_BookSetFrontSpellHook: number: " << number << std::endl;
	util::debug(&logStream);
	oCMag_BookSetFrontSpell((oCMag_Book*)pThis, number);
}

zCVisual* DynItemInst::zCVisualLoadVisual(zSTRING const& name)
{
	XCALL(ZCVISUAL_LOAD_VISUAL);
}

zCListSort<oCItem>* DynItemInst::getInvItemByInstanceId(oCNpcInventory* inventory, int instanceId)
{
	inventory->UnpackCategory();
	ObjectManager* manager = ObjectManager::getObjectManager();
	zCListSort<oCItem>* list = reinterpret_cast<zCListSort<oCItem>*>(inventory->inventory_data);
	while(list != nullptr) {
		oCItem* item = list->GetData();
		if (item != nullptr && manager->getInstanceId(*item) == instanceId)
		{
			return list;
		}
		list = list->GetNext();
	}

	return nullptr;
};

oCItem* DynItemInst::getInvItemByInstanceId2(oCNpcInventory* inventory, int instanceId)
{
	inventory->UnpackCategory();
	int itemNumber = inventory->GetNumItemsInCategory();
	for (int i = 0; i < itemNumber; ++i)
	{
		oCItem* itm = inventory->GetItem(i);
		if (itm != nullptr && itm->GetInstance() == instanceId)
		{
			return itm;
		}
	}

	return nullptr;
}



std::string DynItemInst::getClearedWorldName(zSTRING const & worldName) {
	std::string text (const_cast<char*>(const_cast<zSTRING &>(worldName).ToChar()));
	std::vector<std::string> splits;
	util::split(splits, text, '/');
	text = splits.back();
	splits.clear();
	util::split(splits, text, '\\');
	text = splits.back();
	splits.clear();
	util::split(splits, text, '.');
	//now only two elements are in splits (the file name and the file type (*.ZEN))
	text = splits.front();
	return text;
}

void DynItemInst::loadDynamicInstances()
{
	logStream << "DynItemInst::loadDynamicInstances: load dii instances..." << std::endl;
	util::logInfo(&logStream);
	ObjectManager* manager = ObjectManager::getObjectManager();
	manager->releaseInstances();
	std::string instances = SAVE_ITEM_INSTANCES + SAVE_ITEM_FILE_EXT;
	std::string saveGameDir = manager->getCurrentDirectoryPath();//manager->getSaveGameDirectoryPath(saveGameSlotNumber);
	std::string fileName = saveGameDir + std::string("\\") + instances;
	manager->loadNewInstances((char*)fileName.c_str());
	logStream << "DynItemInst::loadDynamicInstances: done." << std::endl;
	util::logInfo(&logStream);
}

void DynItemInst::initAdditMemory()
{
	logStream << "DynItemInst::initAdditMemory: init..." << std::endl;
	util::logInfo(&logStream);

	ObjectManager* manager = ObjectManager::getObjectManager();
	std::string saveGameDir = manager->getCurrentDirectoryPath();

	std::string worldName = oCGame::GetGame()->GetGameWorld()->GetWorldName().ToChar();
	std::string saveFile = SAVE_ITEM_ADDIT + worldName + SAVE_ITEM_FILE_EXT; 
	std::string additSaveGameFilePath = saveGameDir + std::string("\\") + saveFile;

	std::string heroData; 
	heroData= saveGameDir + std::string("\\") 
			+ SAVE_ITEM_HERO_DATA + SAVE_ITEM_FILE_EXT;
	manager->loadHeroData(const_cast<char*>(heroData.c_str()));
	manager->loadWorldObjects(const_cast<char*>(additSaveGameFilePath.c_str()));

	restoreDynamicInstances(oCGame::GetGame());
	manager->removeAllAdditionalMemory();

	//zCParser* parser = zCParser::GetParser();
	//parser->CallFunc(parser->GetIndex("DII_AFTER_LOADING_FINISHED_CALLBACK"));
	logStream << "DynItemInst::initAdditMemory: done." << std::endl;
	util::logInfo(&logStream);
}

bool DynItemInst::isReadiedWeapon(int weaponMode, oCItem* item)
{
	switch(weaponMode)
	{

	case 3:
		if (item->HasFlag(2))
		{
			return true;
		}
		break;
	case 4:
		if (item->HasFlag(2))
		{
			return true;
		}
		break;
	case 5:
		if (item->HasFlag(1<<19))
		{
			return true;
		}
		break;
	case 6:
		if (item->HasFlag(1<<20))
		{
			return true;
		}
		break;
	case 7:
		return true;
	default: break;
	}

	return false;
}

void DynItemInst::updateRangedWeapon(oCItem* item, oCNpcInventory* inventory, bool munitionUsesRightHand)
{
	zCListSort<oCItem>* list = getInvItemByInstanceId(inventory, item->munition);
	oCItem* munition = list->GetData();
	int arrowId = ObjectManager::getObjectManager()->getInstanceId(*munition);
	zCListSort<oCItem>* list2 = list->GetNext();
	oCItem* munition2 = nullptr;
	oCNpc* owner = inventory->GetOwner();

	if (list2)
	{
		munition2 = list2->GetData();
	}

	if (munition2 && munition2->instanz > 1)
	{
		logStream << "DynItemInst::updateRangedWeapon: munition2->instanz > 1!";
		util::logFault(&logStream);
	}

	int amount = munition->instanz;
	inventory->Remove(munition, munition->instanz);
	if (munition2)
	{
		inventory->Remove(munition2, munition2->instanz);
		amount += munition2->instanz;
	}
	munition = oCObjectFactory::GetFactory()->CreateItem(munition->GetInstance());
	munition->instanz = amount;
	inventory->Insert(munition);
	munition = getInvItemByInstanceId(inventory, arrowId)->GetData();

	if (munitionUsesRightHand)
	{
		oCNpcSetRightHand(owner, munition->SplitItem(1));
	} else
	{
		oCNpcSetLeftHand(owner, munition->SplitItem(1));
	}
}

void DynItemInst::resetInstanceNameStruct()
{
	instanceNames.nearFightCounter = 1;
	instanceNames.distanceFightCounter = 1;
	instanceNames.runeCounter = 1;
	instanceNames.otherCounter = 1;
}

int DynItemInst::getSlotNumber(oCNpcInventory* inventory, oCItem* item)
{
	int itemNumber = inventory->GetNumItemsInCategory();
	for (int i = 0; i < itemNumber; ++i)
	{
		oCItem* slotItem = inventory->GetItem(i);
		if (slotItem == item)
		{
			return i;
		}
	}

	//item isn't in the inventory
	return -1;
}

oCItem* DynItemInst::searchItemInInvbyInstanzValue(oCNpcInventory* inventory, int searchValue)
{
	inventory->UnpackCategory();
	int itemNumber = inventory->GetNumItemsInCategory();
	for (int i = 0; i < itemNumber; ++i)
	{
		oCItem* slotItem = inventory->GetItem(i);
		if (slotItem->instanz == searchValue)
		{
			return slotItem;
		}
	}

	//no item was found
	return nullptr;
};