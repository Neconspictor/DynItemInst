/*////////////////////////////////////////////////////////////////////////////

This file is part of neclib.

Copyright © 2015-2020 David Goeth

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

/////////////////////////////////////////////////////////////////////////////**/

#include <Levitation.h>
#include <HookManager.h>
#include <api/g2/zcworld.h>
#include <api/g2/zcinput.h>
#include <api/g2/zcmodel.h>
#include <api/g2/ocgame.h>
#include <LevitationData.h>
#include <Util.h>
#include <queue>
#include <api/g2/zcparser.h>
#include <api/g2/oCItemExtended.h>

using namespace LevitationUtil;

typedef void(__thiscall* ZCVobDoFrameActivity)(void*); ZCVobDoFrameActivity zCVobDoFrameActivity;
typedef void(__thiscall* ZCVobSetPhysicsEnabled)(void*, int); ZCVobSetPhysicsEnabled zCVobSetPhysicsEnabled;
typedef void(__thiscall* OCGamePause)(void*, int); OCGamePause oCGamePause;
typedef void(__thiscall* OCGameUnpause)(void*); OCGameUnpause oCGameUnpause;
typedef void(__thiscall* ZCVobSetCollDet)(void*, int); ZCVobSetCollDet zCVobSetCollDet;
typedef int(__thiscall* ZCAIPlayerCheckFloorSliding)(void*); ZCAIPlayerCheckFloorSliding zCAIPlayerCheckFloorSliding;
typedef void(__thiscall* DoSurfaceAlignment)(void*); DoSurfaceAlignment doSurfaceAlignment;
typedef void(__thiscall* ZCVobCheckAndResolveCollisions)(void*); ZCVobCheckAndResolveCollisions zCVobCheckAndResolveCollisions;
typedef void(__thiscall* ZCTriggerOnTouch)(void*, zCVob*); ZCTriggerOnTouch zCTriggerOnTouch;
typedef int(__thiscall* ZCVobHasEnoughSpace)(void*, zVEC3&); ZCVobHasEnoughSpace zCVobHasEnoughSpace;
typedef zVEC3(__thiscall* ZCVobGetVelocity)(void*); ZCVobGetVelocity zCVobGetVelocity;

typedef void(__thiscall* ZCVobBeginMovement)(void* pThis);
ZCVobBeginMovement  zCVobBeginMovement  = (ZCVobBeginMovement)0x0061DA80;

//.text:007380E0 public: int __thiscall oCNpc::IsMovLock(void) proc near
typedef int(__thiscall* OCNpcIsMovLock)(oCNpc*);
OCNpcIsMovLock oCNpcIsMovLock = (OCNpcIsMovLock)0x007380E0;

//.text:00579170 public: void __thiscall zCModel::CalcModelBBox3DWorld(void) proc near
typedef void(__thiscall* ZCModelCalcModelBBox3DWorld)(zCModel*);
ZCModelCalcModelBBox3DWorld zCModelCalcModelBBox3DWorld = (ZCModelCalcModelBBox3DWorld)0x00579170;

//.text:00511320 public: int __thiscall zCAIPlayer::CheckEnoughSpaceMoveDir(class zVEC3 const &, int) proc near
typedef int(__thiscall* ZCAIPlayerCheckEnoughSpaceMoveDir)(void*, zVEC3 const &, int); ZCAIPlayerCheckEnoughSpaceMoveDir zCAIPlayerCheckEnoughSpaceMoveDir;

//.text:00699F60 ; protected: int __thiscall oCAIHuman::PC_ActionMove(int)
typedef int(__thiscall* OCAIHumanPC_ActionMove)(void* pThis, int); OCAIHumanPC_ActionMove oCAIHumanPC_ActionMove = (OCAIHumanPC_ActionMove)0x00699F60;




typedef  int (__fastcall* ZCCollObjectCharacterCalcSlideVector)(void* pThis, zVEC3 const & vec1, zVEC3 const & vec2, zVEC3 & vec3, zVEC3 & vec4, float& floatValue);
ZCCollObjectCharacterCalcSlideVector zCCollObjectCharacterCalcSlideVector;

//.text:00487C50; public: class zMAT4 & __thiscall zMAT4::SetTranslation(class zVEC3 const &)
typedef zMAT4& (__thiscall* ZMAT4SetTranslation)(zMAT4* pThis, zVEC3 const & translation);
ZMAT4SetTranslation zMAT4SetTranslation;

typedef  void(__thiscall* ZCVobCalcGroundPoly)(void* pThis);
ZCVobCalcGroundPoly zCVobCalcGroundPoly;


typedef  void (__thiscall* ZCVobUpdatePhysics)(void* pThis);
ZCVobUpdatePhysics zCVobUpdatePhysics;



//.text:0061E0D0 ; public: virtual void __thiscall zCVob::EndMovement(int)
typedef void (__thiscall* ZCVobEndMovement)(void* pThis, int arg0);
ZCVobEndMovement zCVobEndMovement;

//.text:005501F0; int __cdecl TestStatic_Char_Char(float, class zCCollisionObject *, class zMAT4 const &, class zCCollisionObject *, class zMAT4 const &, class zCCollisionReport * *)
typedef int(__cdecl* TestStatic_Char_Char)(float, void* collisionObject1, zMAT4 const& mat1, void* collisionObject2, zMAT4 const& mat2, void* collisionReportPtr);
TestStatic_Char_Char testStatic_Char_Char;

typedef int (__fastcall* ZCCollObjectCharacterTestSoftCollisions)(void* pThis, zVEC3 const& vec1, zVEC3& vec2, zVEC3& vec3, zVEC3& vec4);
ZCCollObjectCharacterTestSoftCollisions zCCollObjectCharacterTestSoftCollisions;


//.text:00551150 ; private: int __fastcall zCCollObjectCharacter::TestHardCollisions(class zVEC3 const &, class zVEC3 const &, class zVEC3 &, struct zCCollObjectCharacter::zTSpatialState &, class zVEC3 &)
typedef int(__fastcall* ZCCollObjectCharacterTestHardCollisions)(void* pThis, zVEC3 const& vec1, zVEC3 const& vec2, zVEC3& vec3, zTSpatialState* zTSpatialState, zVEC3& vec4);
ZCCollObjectCharacterTestHardCollisions zCCollObjectCharacterTestHardCollisions;

typedef void(__fastcall* ZCCollObjectCharacterDetectCollisionsSelf)(void* pThis, void* collisionObjectArray, void* collisionReports);
ZCCollObjectCharacterDetectCollisionsSelf zCCollObjectCharacterDetectCollisionsSelf;

//.text:0050DC40 ; private: void __thiscall zCAIPlayer::CheckPhysics(void)
typedef void(__thiscall* ZCAIPlayerCheckPhysics)(void* pThis);
ZCAIPlayerCheckPhysics zCAIPlayerCheckPhysics;

//.text:00550E20 ; private: void __fastcall zCCollObjectCharacter::FindFloorWaterCeiling(class zVEC3 const &, struct zCCollObjectCharacter::zTSpatialState &)
typedef void(__fastcall* ZCCollObjectCharacterFindFloorWaterCeiling)(void* pThis, zVEC3 const & vec1, void* zTSpatialState);
ZCCollObjectCharacterFindFloorWaterCeiling zCCollObjectCharacterFindFloorWaterCeiling;

//.text:00706B60 ; protected: virtual void __thiscall oCItemContainer::DrawCategory(void)
using OCItemContainerDrawCategory = void(__thiscall*)(void* pThis);
OCItemContainerDrawCategory oCItemContainerDrawCategory = (OCItemContainerDrawCategory)0x00706B60;

//.text:007092C0 ; public: virtual class oCItem * __thiscall oCItemContainer::GetSelectedItem(void)
using OCItemContainerGetSelectedItem = oCItem * (__thiscall*)(oCItemContainer* pThis);
OCItemContainerGetSelectedItem oCItemContainerGetSelectedItem = (OCItemContainerGetSelectedItem)0x007092C0;

//.text:00707BA4 ; naked function
using OCItemContainerDrawGetItem = void (*)();
OCItemContainerDrawGetItem oCItemContainerDrawGetItem = (OCItemContainerDrawGetItem)0x00707BA4;

//.text:00709740 ; void __thiscall oCItemContainer::NextItem(oCItemContainer *__hidden this)
using OCItemContainerNextItem = void(__thiscall*)(oCItemContainer* pThis);
OCItemContainerNextItem oCItemContainerNextItem = (OCItemContainerNextItem)0x00709740;

//.text:007076B0 ; void __thiscall oCItemContainer::Draw(oCItemContainer *this)
using OCItemContainerDraw = void (__thiscall*)(oCItemContainer* pThis);
OCItemContainerDraw oCItemContainerDraw = (OCItemContainerDraw)0x007076B0;

//.text:00705B80 ; int __cdecl oCItemCompareFunction(int,int)
using OCItemCompareFunction = int(__cdecl*)(oCItem* a, oCItem* b);
OCItemCompareFunction oCItemCompareFunction = (OCItemCompareFunction)0x00705B80;


void oCItemContainerDrawGetItemNaked();

namespace LevitationUtil
{
	ZVEC3Normalize zVEC3Normalize = (ZVEC3Normalize)0x00490EA0;
	OCNpcResetPos oCNpcResetPos = (OCNpcResetPos)0x006824D0;
	//.text:0061E0D0 public: virtual void __thiscall zCVob::EndMovement(int) proc near
	//ZCVobEndMovement zCVobEndMovement = (ZCVobEndMovement)0x0061E0D0;
	RotateLocalY rotateLocalY = (RotateLocalY)0x0061B720;
}

int COLLISION_HEAD_SIZE_Y = 25;
bool Levitation::gameIsPaused = false;
bool Levitation::noCollision = false;
int Levitation::frameTime = 0;
int Levitation::diffFrameTime = 0;
bool Levitation::mCustomCollisionDetected = false;

LevitationData heroLevitationBean;
bool heroLevitationBeanCalled;
bool fakeUpKey;
int frameTimePast;



Levitation::Levitation() :Module("Levitation")
{
}

Levitation::~Levitation()
{
}

void Levitation::hookModule()
{
	zCVobDoFrameActivity = reinterpret_cast<ZCVobDoFrameActivity>(ZCVOB_DO_FRAME_ACTIVITY_ADDRESS);
	zCVobSetPhysicsEnabled = reinterpret_cast<ZCVobSetPhysicsEnabled>(ZCVOB_SET_PHYSICS_ENABLED_ADDRESS);
	oCGamePause = reinterpret_cast<OCGamePause>(OCGAME_PAUSE_ADDRESS);
	oCGameUnpause = reinterpret_cast<OCGameUnpause>(OCGAME_UNPAUSE_ADDRESS);
	zCVobSetCollDet = reinterpret_cast<void(__thiscall*)(void*, int)>(ZCVOB_SET_COLL_DET_ADDRESS);
	zCAIPlayerCheckFloorSliding = reinterpret_cast<int(__thiscall*)(void*)>((ZCAIPLAYER_CHECK_FLOOR_SLIDING_ADDRESS));

	doSurfaceAlignment = reinterpret_cast<DoSurfaceAlignment>((DO_SURFACE_ALIGNMENT_ADDRESS));
	zCTriggerOnTouch = reinterpret_cast<ZCTriggerOnTouch>(ZCTRIGGER_ON_TOUCH_ADDRESS);
	zCVobHasEnoughSpace = reinterpret_cast<ZCVobHasEnoughSpace>(ZCVOB_HAS_ENOUGH_SPACE_ADDRESS);
	zCVobCheckAndResolveCollisions = reinterpret_cast<ZCVobCheckAndResolveCollisions>((ZCVOB_CHECK_AND_RESOLVE_COLLISION_ADDRESS));
	zCVobGetVelocity = reinterpret_cast<zVEC3(__thiscall*)(void*)>(ZCVOB_GET_VELOCITY_ADDRESS);
	zCAIPlayerCheckEnoughSpaceMoveDir = (ZCAIPlayerCheckEnoughSpaceMoveDir)0x00511320;
	testStatic_Char_Char = (TestStatic_Char_Char)TESTSTATIC_CHAR_CHAR_ADDRESS;
	zCCollObjectCharacterTestHardCollisions = (ZCCollObjectCharacterTestHardCollisions) ZCCOLL_OBJECT_CHARACTER_TEST_HARD_COLLIONS_ADDRESS;
	zCAIPlayerCheckPhysics = (ZCAIPlayerCheckPhysics)ZCAIPLAYER_CHECK_PHYSICS;




	zCVobUpdatePhysics = (ZCVobUpdatePhysics)ZCVOB_UPDATE_PHYSICS_ADDRESS;
	zCVobEndMovement = (ZCVobEndMovement)ZCVOB_END_MOVEMENT;
	zCCollObjectCharacterFindFloorWaterCeiling = (ZCCollObjectCharacterFindFloorWaterCeiling)ZCCOLL_OBJECT_CHARACTER_FIND_FLOOR_WATER_CEILING;
	zCCollObjectCharacterTestSoftCollisions = (ZCCollObjectCharacterTestSoftCollisions)ZCCOLL_OBJECT_CHARACTER_TEST_SOFT_COLLIONS_ADDRESS;
	zCCollObjectCharacterDetectCollisionsSelf = (ZCCollObjectCharacterDetectCollisionsSelf)ZCCOLL_OBJECT_CHARACTER_DETECT_SELF_COLLISION_ADDRESS;
	zCVobCalcGroundPoly = (ZCVobCalcGroundPoly)ZCVOB_CALC_GROUND_POLY_ADDRESS;
	zMAT4SetTranslation = (ZMAT4SetTranslation) ZMAT4_SET_TRANSLATION_ADDRESS;
	zCCollObjectCharacterCalcSlideVector = (ZCCollObjectCharacterCalcSlideVector)ZCCOLL_OBJECT_CHARACTER_CALC_SLIDE_VECTOR_ADDRESS;

	HookManager* hookManager = HookManager::getHookManager();
	hookManager->addFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, mModuleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGamePause, oCGamePauseHook, mModuleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameUnpause, oCGameUnpauseHook, mModuleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobCheckAndResolveCollisions, zCVobCheckAndResolveCollisionsHook, mModuleDesc);
	
	hookManager->addFunctionHook((LPVOID*)&zCVobUpdatePhysics, zCVobUpdatePhysicsHook, mModuleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCAIHumanPC_ActionMove, oCAIHumanPC_ActionMoveHook, mModuleDesc);
}

void Levitation::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, mModuleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGamePause, oCGamePauseHook, mModuleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameUnpause, oCGameUnpauseHook, mModuleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCVobUpdatePhysics, zCVobUpdatePhysicsHook, mModuleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCAIHumanPC_ActionMove, oCAIHumanPC_ActionMoveHook, mModuleDesc);
	//hookManager->removeFunctionHook((LPVOID*)&zCVobCheckAndResolveCollisions, zCVobCheckAndResolveCollisionsHook, mModuleDesc);
}

zVEC3 levitatePosition;
bool doHardTests = false;

int Levitation::LEVITATION_IsGamePaused()
{
	return gameIsPaused;
}

void Levitation::zCVobDoFrameActivityHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = false;
	float oldYPosition = 0;
	zVEC3 oldLook;
	zVEC3 pos;


	if (pThis == hero)
	{
		frameTimePast = calcPastFrameTime(); 
		adjust = isLevitationActive();
	}

	if (adjust) {
		//oldYPosition = hero->GetPosition().y;
		char* modelPtr = (char*)hero->GetModel();

		modelPtr[0x1F8] = 0xD;

		zMAT4* mat = &hero->trafoObjToWorld;
		oldLook = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		heroLevitationBean.oldLook = oldLook;

		//check if hard collision tests should be applied (important for vobs and mobs)
		//doHardTests = check_prePass(hero, hero->trafoObjToWorld);

		//.text:0065F790 ; public: static class oCInformationManager & __cdecl oCInformationManager::GetInformationManager(void)
		using OCInformationManagerGetInformationManager = void* (__cdecl*)();
		static OCInformationManagerGetInformationManager oCInformationManagerGetInformationManager = (OCInformationManagerGetInformationManager)0x0065F790;
		//.text:006609D0 ; public: int __fastcall oCInformationManager::HasFinished(void)
		using OCInformationManagerHasFinished = int(__fastcall*)(void* pThis);
		static OCInformationManagerHasFinished oCInformationManagerHasFinished = (OCInformationManagerHasFinished)0x006609D0;
		

		void* infoManager = oCInformationManagerGetInformationManager();
		int hasFinished = oCInformationManagerHasFinished(infoManager);

		if (hasFinished)
			levitatePosition = levitate();

		
		hero->GetPositionWorld(pos.x, pos.y, pos.z);
		float hoverDistance = heroLevitationBean.getDistanceToGround(pos);
		if (true) {
			float add = getMinHoverDistance() - hoverDistance;
			pos.y += max(0, add);
			hero->SetPositionWorld(pos);
		}
		
	}

	//if (adjust) {
	//	adjust = !doCustomCollisionCheck(hero);
	//}
	
	zCVobDoFrameActivity(pThis);

	if (adjust) {
		doCustomCollisionCheck(hero);
		//hero->SetPositionWorld(pos);
	}
	

	
}

void Levitation::zCVobSetPhysicsEnabledHook(void* pThis, int second)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero) && isLevitationActive()) {
		zCInput* input = zCInput::GetInput();
		//if (input->KeyPressed(0x1A)) return; //zCVobSetPhysicsEnabled(pThis, true);
		return;
	}
	zCVobSetPhysicsEnabled(pThis, second);
}

void Levitation::oCGamePauseHook(void* pThis, int second)
{
	mLogStream << "Paused Game!" << std::endl;
	util::logAlways(&mLogStream);
	Levitation::gameIsPaused = true;
	oCGamePause(pThis, second);
}

void Levitation::oCGameUnpauseHook(void* pThis)
{
	mLogStream << "Unpaused Game!" << std::endl;
	util::logAlways(&mLogStream);
	Levitation::gameIsPaused = false;
	oCGameUnpause(pThis);
}

void Levitation::zCVobSetCollDetHook(void* pThis, int second)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero) && isLevitationActive()) {
		return zCVobSetCollDet(pThis, 0);
	}
	zCVobSetCollDet(pThis, second);
}

int Levitation::zCAIPlayerCheckFloorSlidingHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero) && isLevitationActive()) {
		return 1;
	}
	return zCAIPlayerCheckFloorSliding(pThis);
}

void Levitation::DoSurfaceAlignmentHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (hero == pThis) && isLevitationActive();
	if (adjust) {
		return;
	}
	return doSurfaceAlignment(pThis);
}

void Levitation::zCVobCheckAndResolveCollisionsHook(void* pThis)
{

	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;
	zVEC3 translation;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	bool adjust = (pThis == hero) && isLevitationActive() && (collisionObject != nullptr) && !customCollisionDetected();
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		translation = zVEC3(mat->_14, mat->_24, mat->_34);
		/*mLogStream << "zCVobCheckAndResolveCollisionsHook (before):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&mLogStream);*/
		//return;
		
	}
	zCVobCheckAndResolveCollisions(pThis);

	if (adjust)
	{
		collisionObject = hero->m_poCollisionObject;
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);

		zVEC3 diff; 
		diff.x = mat->_14 - translation.x;
		diff.y = mat->_24 - translation.y;
		diff.z = mat->_34 - translation.z;
		float length = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

		

		if (length < 2.2 || length > 20) return;

		mat->_14 = translation.x;
		mat->_24 = translation.y;
		mat->_34 = translation.z;
		
		/*zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCVobCheckAndResolveCollisionsHook (after):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&mLogStream);*/
	}
}

void Levitation::zCTriggerOnTouchHook(void* pThis, zCVob* vob)
{
	if (vob == oCNpc::GetHero()) {
		mLogStream << "zCTriggerOnTouch() called on hero!!" << std::endl;
		util::debug(&mLogStream);
		zCTriggerOnTouch(pThis, vob);
	}
}

int Levitation::zCVobHasEnoughSpaceHook(void* pThis, zVEC3& second)
{
	int result = zCVobHasEnoughSpace(pThis, second);
	if (pThis == oCNpc::GetHero()) {
		mLogStream << "called for hero!: " << result << std::endl;
		util::debug(&mLogStream);
	}
	return result;
}

zVEC3 Levitation::zCVobGetVelocityHook(void* pThis)
{
	return zCVobGetVelocity(pThis);
}

int Levitation::TestStatic_Char_CharHook(float value, void* collisionObject1, zMAT4 const& mat1, void* collisionObject2,
	zMAT4 const& mat2, void* collisionReportPtr)
{
	return 0;
}

int Levitation::zCCollObjectCharacterCalcSlideVectorHook(void* pThis, zVEC3 const& vec1, zVEC3 const& vec2, zVEC3& vec3,
	zVEC3& vec4, float& floatValue)
{

	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	bool adjust = (pThis == collisionObject) && isLevitationActive() && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCCollObjectCharacterCalcSlideVectorHook (before):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		mLogStream << "\tvec1 = " << vec1 << std::endl;
		mLogStream << "\tvec2 = " << vec2 << std::endl;
		mLogStream << "\tvec3 = " << vec3 << std::endl;
		mLogStream << "\tvec4 = " << vec4 << std::endl;
		mLogStream << "\tfloatValue = " << floatValue << std::endl;
		util::logInfo(&mLogStream);
	}

	int result = zCCollObjectCharacterCalcSlideVector(pThis, vec1, vec2, vec3, vec4, floatValue);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCCollObjectCharacterCalcSlideVectorHook (after):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		mLogStream << "\tvec1 = " << vec1 << std::endl;
		mLogStream << "\tvec2 = " << vec2 << std::endl;
		mLogStream << "\tvec3 = " << vec3 << std::endl;
		mLogStream << "\tvec4 = " << vec4 << std::endl;
		mLogStream << "\tfloatValue = " << floatValue << std::endl;
		util::logInfo(&mLogStream);
	}

	return result;
}

bool activatezMAT4Logging = false;

zMAT4& Levitation::zMAT4SetTranslationHook(zMAT4* pThis, zVEC3 const& translation)
{
	if (activatezMAT4Logging)
	{
		zVEC3 trans(pThis->_14, pThis->_24, pThis->_34);
		mLogStream << "zMAT4SetTranslationHook (before):" << std::endl;
		mLogStream << "\ttrans = " << trans << std::endl;
		util::logInfo(&mLogStream);
	}

	zMAT4& result = zMAT4SetTranslation(pThis, translation);

	if (activatezMAT4Logging)
	{
		zVEC3 trans(pThis->_14, pThis->_24, pThis->_34);
		mLogStream << "zMAT4SetTranslationHook (after):" << std::endl;
		mLogStream << "\ttrans = " << trans << std::endl;
		util::logInfo(&mLogStream);
	}

	return result;
}

void Levitation::zCVobCalcGroundPolyHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	bool adjust = (pThis == hero) && isLevitationActive() && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCVobCalcGroundPolyHook (before):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logInfo(&mLogStream);
	}
	zCVobCalcGroundPoly(pThis);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCVobCalcGroundPolyHook (after):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logInfo(&mLogStream);
	}
}

void Levitation::zCVobUpdatePhysicsHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	zVEC3 translationBefore;

	bool adjust = (pThis == hero) && isLevitationActive() && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);

		auto pos = hero->GetPosition();

		mat->_14 = pos.x;
		mat->_24 = pos.y;
		mat->_34 = pos.z;

		translationBefore = zVEC3(mat->_14, mat->_24, mat->_34);
		//logStream << "zCVobUpdatePhysicsHook (before):" << std::endl;
		//logStream << "\ttranslation = " << translation << std::endl;
		//util::logInfo(&logStream);
	}

	if (!adjust)
		zCVobUpdatePhysics(pThis);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translationAfter(mat->_14, mat->_24, mat->_34);

		if (translationAfter.y < translationBefore.y)
		{
			//mat->_24 = translationBefore.y;
		}

		//logStream << "zCVobUpdatePhysicsHook (after):" << std::endl;
		//logStream << "\ttranslation = " << translation << std::endl;
		//util::logInfo(&logStream);
	}
}

void Levitation::zCVobEndMovementHook(void* pThis, int arg0)
{
	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	bool adjust = (pThis == hero) && isLevitationActive() && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCVobEndMovementHook (before):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logInfo(&mLogStream);
	}
	zCVobEndMovement(pThis, arg0);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCVobEndMovementHook (after):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logInfo(&mLogStream);
	}
}

int Levitation::zCCollObjectCharacterTestSoftCollisionsHook(void* pThis, zVEC3 const& vec1, zVEC3& vec2, zVEC3& vec3,
	zVEC3& vec4)
{
	oCNpc* hero = oCNpc::GetHero();

	bool adjust = (pThis != nullptr) && isLevitationActive() && (pThis == hero->m_poCollisionObject);
	if (adjust) {

		//vec2.y = vec1.y;

		//vec2 += levitatePosition;

		int result = zCCollObjectCharacterTestSoftCollisions(pThis, vec1, vec2, vec3, vec4);

		

		return result;
	}
	return zCCollObjectCharacterTestSoftCollisions(pThis, vec1, vec2, vec3, vec4);
}

int Levitation::zCCollObjectCharacterTestHardCollisionsHook(void* pThis, zVEC3& vec1, zVEC3& vec2, zVEC3& vec3,
	zTSpatialState* zTSpatialState, zVEC3& vec4)
{

	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;
	if (hero != nullptr)
	{
		collisionObject = hero->m_poCollisionObject;
	}

	bool adjust = (pThis == collisionObject) && isLevitationActive() && (collisionObject != nullptr);
	if (adjust) {
		//if (Levitation::noCollision && !DynItemInst::loadGame && !HookManager::changeLevel) return;
		//vec3.x = 0;
		//vec3.y = 0;
		//vec3.z = 0;

		//zVEC3 translation = levitatePosition;
		//vec2 += levitatePosition;

		int result = 0;

		if (doHardTests)
		{
			result = zCCollObjectCharacterTestHardCollisions(pThis, vec1, vec2, vec3, zTSpatialState, vec4);
		} else
		{
			vec3 = vec2;
		}

		//vec3.y = vec2.y;

		bool log = (vec3.y != vec2.y) && (vec3.y != vec1.y);
		//log = true;

		//logStream << "zCCollObjectCharacterTestHardCollisionsHook: after:" << std::endl;
		//logStream << "vec3 (after) = " << vec3 << std::endl;
		//logStream << "vec2 (after) = " << vec2 << std::endl;
		//logStream << "vec1 (after) = " << vec1 << std::endl;
		//logStream << "log (after) = " << log << std::endl;
		//logStream << "levitatePosition (after) = " << levitatePosition << std::endl;

		//util::logInfo(&logStream);

		if (log)
		{

			//vec3.y = vec2.y;
			float temp = vec3.y + 10;
			vec3.y = max(vec2.y, temp);
		}
		return result;
	}
	return zCCollObjectCharacterTestHardCollisions(pThis, vec1, vec2, vec3, zTSpatialState, vec4);
}

void Levitation::zCCollObjectCharacterDetectCollisionsSelfHook(void* pThis, void* collisionObjectArray,
	void* collisionReports)
{

	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (pThis != nullptr) && isLevitationActive() && (pThis == hero->m_poCollisionObject);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)pThis + 0x44);
		zVEC3 translation (mat->_14, mat->_24, mat->_34);
		mLogStream << "zCCollObjectCharacterDetectCollisionsSelfHook (before):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logInfo(&mLogStream);


	}

	zCCollObjectCharacterDetectCollisionsSelf(pThis, collisionObjectArray, collisionReports);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)pThis + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCCollObjectCharacterDetectCollisionsSelfHook (after):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logInfo(&mLogStream);
	}
}

void Levitation::zCAIPlayerCheckPhysicsHook(void* pThis)
{
	bool adjust = (pThis == oCNpc::GetHero()) && (pThis != NULL) && isLevitationActive();
	if (adjust) {
		return;
	}
	zCAIPlayerCheckPhysics(pThis);
}

void Levitation::zCCollObjectCharacterFindFloorWaterCeilingHook(void* pThis, zVEC3 const& vec1, zTSpatialState* zTSpatialState)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (pThis != nullptr) && isLevitationActive() && (pThis == hero->m_poCollisionObject);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)pThis + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCCollObjectCharacterFindFloorWaterCeilingHook (before):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		mLogStream << "\tzTSpatialState = " << *zTSpatialState << std::endl;
		util::logInfo(&mLogStream);
	}
	zCCollObjectCharacterFindFloorWaterCeiling(pThis, vec1, zTSpatialState);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)pThis + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCCollObjectCharacterFindFloorWaterCeilingHook (after):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		mLogStream << "\tzTSpatialState = " << *zTSpatialState << std::endl;
		util::logInfo(&mLogStream);
	}
}

int Levitation::zCVobIsColliding(void* pThis)
{
	XCALL(ZCVOB_IS_COLLIDING);
}

int Levitation::zCBspBaseCollectPolysInBBox3D(void*, zTBBox3D const&, zCPolygon**&, int&)
{
	XCALL(ZCBSPBASE_COLLECT_POLYS_IN_BBOX3D);
}

void Levitation::zCBspBaseCollectVobsInBBox3D(void* pThis, zCArray<zCVob*>& collectedVobs, zTBBox3D const& boundingBox)
{
	XCALL(0x00531110);
}

int Levitation::zCAIPlayerCheckEnoughSpaceMoveDirHook(void* pThis, zVEC3 const& vec, int second)
{
	int result = zCAIPlayerCheckEnoughSpaceMoveDir(pThis, vec, second);
	oCNpc* hero = oCNpc::GetHero();
	if (result && hero != NULL && (pThis == hero) && isLevitationActive()) {
		mLogStream << "zCAIPlayerCheckEnoughSpaceMoveDirHook: called!"  << std::endl;
		util::logInfo(&mLogStream);
		return 0;
	}

	return result;
}

int Levitation::oCAIHumanPC_ActionMoveHook(void* pThis, int param1)
{
	void* hero = (void*)oCNpc::GetHero();

	bool adjust = isLevitationActive() && pThis != nullptr;

	if (adjust)
	{
		void* npc = *(void**)((char*)pThis + 0x12C);
		adjust &= (npc == hero);
	}

	if (adjust)
	{
		int modelAddress = *(int*)((char*)pThis + 0x68);

		if (modelAddress != 0)
		{
			char* model = (char*)modelAddress;
			model[0x1F8] = 0xD;
		}
	}

	return oCAIHumanPC_ActionMove(pThis, param1);
}

// TODO: Floor aligning doesn't work properly on certain cases; keyword: consider poly normals!
void Levitation::doFloorAligning(zVEC3* finalPosition, zMAT4* mat)
{
	zVEC3 test = *finalPosition;
	zVEC3 look(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
	test += look * 20;
	float collisionYDownDist1 = getCollideYDir(test, 100, false).distance;
	float collisionYDownDist2 = getCollideYDir(*finalPosition, 100, false).distance;
	float collisionYDownDist = min(collisionYDownDist1, collisionYDownDist2);
	if (collisionYDownDist < 150)
	{
		finalPosition->y += 50 - (collisionYDownDist - 100);
	}
}

bool Levitation::isLevitationActive()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("_LEVITATION_IsActive");
	return static_cast<bool>(symbol->content.data_int);
}

int Levitation::getMinHoverDistance()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_HOVER_DISTANCE");
	return symbol->content.data_int;
}

float Levitation::getSpeedVertical()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_SPEED_VERTICAL");
	return static_cast<float>(symbol->content.data_int);
}

float Levitation::getSpeedForward()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_SPEED_FORWARD");
	return static_cast<float>(symbol->content.data_int);
}

float Levitation::getSpeedBackward()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_SPEED_BACKWARD");
	return static_cast<float>(symbol->content.data_int);
}

float Levitation::getGravity()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_GRAVITY");
	float gravity = static_cast<float>(symbol->content.data_int);
	if (gravity < 0.0) gravity = 0.0f;
	return gravity;
}

float Levitation::getTurnSpeed()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_SPEED_TURN");
	float speedTurn = static_cast<float>(symbol->content.data_int);
	if (speedTurn < 0.0) speedTurn = 0.0f;
	return speedTurn;
}

int Levitation::getMoveForwardKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_FORWARD");
	return symbol->content.data_int;
}

int Levitation::getMoveBackwardKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_BACKWARD");
	return symbol->content.data_int;
}

int Levitation::getTurnLeftKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_LEFT");
	return symbol->content.data_int;
}

int Levitation::getTurnRightKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_RIGHT");
	return symbol->content.data_int;
}

int Levitation::getMoveUpKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_UP");
	return symbol->content.data_int;
}

int Levitation::getMoveDownKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_DOWN");
	return symbol->content.data_int;
}

bool Levitation::customCollisionDetected()
{
	return mCustomCollisionDetected;
}

zVEC3 Levitation::levitate() {

	zVEC3 positionAdd(0, 0, 0);

	if (!isLevitationActive()) return positionAdd;
	if (Levitation::gameIsPaused) return positionAdd;

	std::stringstream mLogStream;

	oCNpc* hero = oCNpc::GetHero();
	//if (oCNpcIsMovLock(hero)) return positionAdd;

	float angle = 0;

	//Levitation::noCollision = true;
	const float speedVertical = getSpeedVertical();
	const float speedForward = getSpeedForward();
	const float speedBackward = getSpeedBackward();
	const float distanceVertical = speedVertical * float(frameTimePast) / 1000.0f;
	const float distanceForward = speedForward * float(frameTimePast) / 1000.0f;
	const float distanceBackward = speedBackward * float(frameTimePast) / 1000.0f;
	const float distanceGravity = getGravity() * float(frameTimePast) / 1000.0f;
	const float speedTurn = getTurnSpeed();

	const int moveForwardKey = getMoveForwardKey();
	const int moveBackwardKey = getMoveBackwardKey();
	const int turnLeftKey = getTurnLeftKey();
	const int turnRightKey = getTurnRightKey();
	const int moveUpKey = getMoveUpKey();
	const int moveDownKey = getMoveDownKey();

	zVEC3 oldLook = heroLevitationBean.oldLook;
	zCInput* input = zCInput::GetInput();
	hero->ResetXZRotationsLocal();

	const auto turnLeft = input->KeyPressed(turnLeftKey);
	const auto turnRight = input->KeyPressed(turnRightKey);
	const auto moveForward = input->KeyPressed(moveForwardKey);
	const auto moveBackward = input->KeyPressed(moveBackwardKey);
	const auto moveUp = input->KeyPressed(moveUpKey);
	const auto moveDown = input->KeyPressed(moveDownKey);

	if (!turnLeft && !turnRight)
	{
		zMAT4* mat = &hero->trafoObjToWorld;
		mat->m[0][2] = oldLook.x;
		mat->m[1][2] = oldLook.y;
		mat->m[2][2] = oldLook.z;
	}

	if (turnLeft)
	{
		angle += -speedTurn * float(frameTimePast) / 1000.0f;
	}
	else if (turnRight)
	{
		angle += speedTurn * float(frameTimePast) / 1000.0f;
	}

	// Pressed � (levitation up)
	if (moveUp) {
		positionAdd.y += distanceVertical;
	}
	// Pressed � (levitation down)
	else if (moveDown) {
		positionAdd.y -= distanceVertical;
	}

	positionAdd.y -= distanceGravity;

	// Pressed Arrow_Up (levitation forward)
	if (moveForward) {
		zMAT4* mat = &hero->trafoObjToWorld;
		zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		positionAdd.x += look.x * distanceForward;
		positionAdd.y += look.y * distanceForward;
		positionAdd.z += look.z * distanceForward;
	}
	// Pressed Arrow_Down (levitation backward)
	else if (moveBackward) {
		zMAT4* mat = &hero->trafoObjToWorld;
		zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		positionAdd.x -= look.x * distanceBackward;
		positionAdd.y -= look.y * distanceBackward;
		positionAdd.z -= look.z * distanceBackward;
	}

	//finally apply changes
	rotateLocalY(hero, angle);
	zVEC3 finalPosition = hero->GetPositionWorld();
	heroLevitationBean.oldXPos = finalPosition.x;
	heroLevitationBean.oldYPos = finalPosition.y;
	heroLevitationBean.oldZPos = finalPosition.z;
	finalPosition += positionAdd;

	hero->SetPositionWorld(finalPosition);
	//heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(finalPosition));

	return positionAdd;
};

Motion Levitation::getCollideYDir(zVEC3 pos, float middlePointDist, bool up) {
	zVEC3 dir;
	if (up) {
		pos.y += middlePointDist;
		dir = pos;
		dir.y += 1000000000;
	}
	else {
		pos.y += middlePointDist;
		dir = pos;
		dir.y -= 1000000000;
	}
	zCWorld* world = oCGame::GetGame()->GetWorld();
	int flags = (1 << 0) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14);
	LevitationData::zCWorldTraceRayNearestHit(world, pos, dir, NULL, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs(intersection->y - pos.y);
	motion.objectPos = pos;
	motion.intersect = *intersection;
	return motion;
};

Motion Levitation::getCollideForwardDir(oCNpc* npc, float middlepointDistance, bool backward) {
	zMAT4* mat = &(npc->trafoObjToWorld);
	zVEC3 dir = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
	// position inside the BBox
	float headSize = 30.0f;
	zVEC3 pos = zVEC3(mat->m[0][3], mat->m[1][3] + middlepointDistance + headSize, mat->m[2][3]);
	pos.x += dir.x * 10;
	pos.y += dir.y * 10;
	pos.z += dir.z * 10;
	if (backward) {
		dir = zVEC3(-dir.x, -dir.y, -dir.z);
	}

	float distanceFactor = 1000000000;
	// dir has length 1 
	// we want a vector that is 10000 units away from pos in direction dir
	zVEC3 posOutside = zVEC3(pos.x + dir.x*distanceFactor, pos.y + dir.y*distanceFactor, pos.z + dir.z*distanceFactor);

	zCWorld* world = oCGame::GetGame()->GetWorld();
	int flags = (1 << 0) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14);
	LevitationData::zCWorldTraceRayNearestHit(world, pos, posOutside, NULL, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs((intersection->x - pos.x) + (intersection->y - pos.y) + (intersection->z - pos.z));
	motion.objectPos = pos;
	motion.intersect = *intersection;
	return motion;
};


int Levitation::calcPastFrameTime() {
	int time = sysGetTime();
	int diff = time - Levitation::frameTime;
	Levitation::frameTime = time;
	Levitation::diffFrameTime = diff;
	return diff;
};

zVEC3 getCollidingPolyNormal(oCNpc* hero, const zMAT4& mat)
{
	bool intersected = false;
	zVEC3 pos(mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)hero + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int third = 0;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = LevitationData::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	zTBBox3D rec;
	rec.bbox3D_maxs = zVEC3(100, 100, 100);//box.bbox3D_maxs;//zVEC3(100,100,100);
										   //logStream << "Test:  box.bbox3D_maxs: " << box.bbox3D_maxs << std::endl;
										   //util::logInfo(&logStream);
	rec.bbox3D_mins = zVEC3(-100, -100, -100);// box.bbox3D_mins;//zVEC3(-100, -100, -100);
											  //logStream << "Test:  box.bbox3D_mins: " << box.bbox3D_mins << std::endl;
											  //util::logInfo(&logStream);
	rec.bbox3D_maxs += pos;
	rec.bbox3D_mins += pos;
	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	Levitation::zCBspBaseCollectPolysInBBox3D(pointer, rec, polys, third);
	if (polys != NULL) {
		//writeToConsole("Collision detected: ", third);
		//logStream << "Test: Collision detected: " << third << std::endl;
		//util::logInfo(&logStream);
		if (third)
		{
			//create plane from intersecting poly
			Plane plane;
			zVEC3 look = zVEC3(mat.m[0][2], mat.m[1][2], mat.m[2][2]);
			zVEC3Normalize(&look);
			zVEC3 oldPos = zVEC3(heroLevitationBean.oldXPos, heroLevitationBean.oldYPos, heroLevitationBean.oldZPos);
			zVEC3 dir = pos - oldPos;
			zVEC3Normalize(&dir);

			Plane::initPlaneByVectors(plane, pos, look);
			plane.convertToHNF();
			Line motionLine(oldPos, look);

			for (int i = 0; i < third; ++i)
			{
				zCPolygon* poly = polys[i];

				if (poly->CheckBBoxPolyIntersection(rec))
				{
					if (poly->CheckBBoxPolyIntersection(box))
					{
						zVEC3* polyNormal = poly->getPolyNormal();

						//we have found what we wanted -> a collision!
						return zVEC3(polyNormal->x, polyNormal->y, polyNormal->z);
						
					}
				}
			}
		}
	}

	//nothing found - return null vector
	return zVEC3(0,0,0);
}

bool Levitation::check_prePass(oCNpc* hero, const zMAT4& mat)
{
	bool intersected = false;
	zVEC3 pos(mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)hero + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int third = 0;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = LevitationData::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	zCArray<zCVob*> collectedVobs;
	Levitation::zCBspBaseCollectVobsInBBox3D(pointer, collectedVobs, box);

	return (collectedVobs.GetSize() > 1); // +1 for hero
}

bool checkVobCollision__checkVob(zCVob* vob, const zTBBox3D& boundingBox)
{
	if (vob == nullptr) return false;

	std::stringstream mLogStream;


	zCArray<void*>* leafObjects = &vob->vobLeafList;
	zCPolygon** polys;
	//int third;
	mLogStream << "checkVobCollision__checkVob(): Check object with leaf number: " << leafObjects->GetSize() << std::endl;

	auto& name = vob->objectName;

	mLogStream << "checkVobCollision__checkVob(): visual name: " << name.ToChar() << std::endl;
	util::logInfo(&mLogStream);

	for (unsigned int i = 0; i < leafObjects->GetSize(); ++i)
	{
		zCBspBase_Small* leaf = (zCBspBase_Small*)leafObjects->GetItem(i);
		//Levitation::zCBspBaseCollectPolysInBBox3D(leaf, boundingBox, polys, third);
		mLogStream << "checkVobCollision__checkVob(): leave has poly count: " << leaf->numPolys << std::endl;
		//logStream << "checkVobCollision__checkBspLeaf(): Found polys: " << third << std::endl;
		util::logInfo(&mLogStream);

		zCPolygon** polyList = (zCPolygon**)leaf->polyList;

		for (int j = 0; j < leaf->numPolys; ++j)
		{
			zCPolygon* poly = polyList[j];

			//logStream << "checkVobCollision__checkVob(): pos = " << pos << std::endl;
			//logStream << "checkVobCollision__checkVob(): vob pos = " << vob->GetPositionWorld() << std::endl;
			//util::logInfo(&logStream);
			if (poly->CheckBBoxPolyIntersection(boundingBox))
			{
				mLogStream << "checkVobCollision__checkVob(): Intersection found! " << std::endl;
				util::logInfo(&mLogStream);
				return true;
			}
		}
		
	}
	return false;
}


bool Levitation::checkVobCollision(void* zCBspBaseObject, zCVob* testedVob, const zTBBox3D& boundingBox)
{
	zCArray<zCVob*> collectedVobs;
	Levitation::zCBspBaseCollectVobsInBBox3D(zCBspBaseObject, collectedVobs, boundingBox);

	if (collectedVobs.GetSize() > 0)
	{
		std::stringstream mLogStream;
		mLogStream << "checkVobCollision(): Found vobs!: " << collectedVobs.GetSize() << std::endl;
		util::logInfo(&mLogStream);

		for (unsigned int i = 0; i < collectedVobs.GetSize(); ++i)
		{
			mLogStream << "checkVobCollision(): test vob with number: " << i << std::endl;
			util::logInfo(&mLogStream);

			if (i > 0) {
				bool test = false;
			}

			zCVob* vob = collectedVobs.GetItem(i);
			if (vob != testedVob && vob != nullptr)
			{
				int zCVob_bitfield0_collDetectionDynamic = ((1 << 1) - 1) << 7;
				int zCVob_bitfield0_staticVob = ((1 << 1) - 1) << 4;
				bool collidesWithDynamicVobs = vob->bitfield[0] & zCVob_bitfield0_collDetectionDynamic;
				bool isStaticVob = vob->bitfield[0] & zCVob_bitfield0_staticVob;
				//bool hasCollisionObject = vob->m_poCollisionObject != nullptr;
				if (collidesWithDynamicVobs) //isStaticVob
				{
					if (checkVobCollision__checkVob(vob, boundingBox))
						return true;
				}
			}
		}
	}

	return false;
}

bool checkCollision(oCNpc* hero, const zMAT4& mat)
{
	bool intersected = false;
	zVEC3 pos (mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)hero + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int third = 0;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = LevitationData::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	zTBBox3D rec;
	rec.bbox3D_maxs = box.bbox3D_maxs;//zVEC3(100,100,100);
	box.bbox3D_maxs.x = 20;
	box.bbox3D_maxs.z *= 0.5;
	rec.bbox3D_mins = box.bbox3D_mins;//zVEC3(-100, -100, -100);
	box.bbox3D_mins.x = -20;
	box.bbox3D_mins.z *= 0.5;
	rec.bbox3D_maxs += pos;
	rec.bbox3D_mins += pos;

	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	

	Levitation::zCBspBaseCollectPolysInBBox3D(pointer, rec, polys, third);
	if (polys != NULL) {
		if (third)
		{
			//create plane from intersecting poly
			Plane plane;
			zVEC3 look = zVEC3(mat.m[0][2], mat.m[1][2], mat.m[2][2]);
			zVEC3Normalize(&look);
			zVEC3 oldPos = zVEC3(heroLevitationBean.oldXPos, heroLevitationBean.oldYPos, heroLevitationBean.oldZPos);
			zVEC3 dir = pos - oldPos;
			zVEC3Normalize(&dir);

			Plane::initPlaneByVectors(plane, pos, look);
			plane.convertToHNF();
			Line motionLine(oldPos, look);

			float globalLambda = 0;

			for (int i = 0; i < third; ++i)
			{
				zCPolygon* poly = polys[i];

				if (poly->CheckBBoxPolyIntersection(box))
				{
					zCMaterial* material = poly->GetMaterial();
					char* bitflag = ((char*)(char*)material + 0x70);
					bool hasNoCollision = *bitflag & (1 << 4);

					const char* materialName = material->GetName().ToChar();
					bool isGhostOccluder = strcmp(materialName, "GHOSTOCCLUDER") == 0;
					//bool isNW_MISC_FULLALPHA_01 = strcmp(materialName, "NW_MISC_FULLALPHA_01") == 0;

					if (isGhostOccluder) {
						//*bitflag = *bitflag | (1 << 4);
					}

					if (hasNoCollision)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with no collision material" << std::endl;
						//util::debug(&logStream);
						continue;
					}

					if (isGhostOccluder)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with GHOSTOCCLUDER material" << std::endl;
						//util::debug(&logStream);
						continue;
					}

					/*if (isNW_MISC_FULLALPHA_01)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with NW_MISC_FULLALPHA_01 material" << std::endl;
						//util::debug(&logStream);
						continue;
					}*/

				

					if (!intersected && !hasNoCollision && !isGhostOccluder) {
						intersected = true;

						//std::stringstream logStream;
						//logStream << "checkCollision(): intersection found!: " << materialName << std::endl;
						//util::debug(&logStream);
						//we have found what we wanted -> a collision!
						break;
					}
				}
			}
		}
	}

	//if (!intersected)
		//intersected = Levitation::checkVobCollision(pointer, hero, rec);

	return intersected;
}


bool Levitation::doCustomCollisionCheck(oCNpc* npc) {

	std::stringstream mLogStream;
	zTBBox3D bBox = LevitationData::zCModelGetBBox3D(npc->GetModel());// ->GetBBox3D();
	zVEC3 pos = npc->GetPosition();
	zMAT4* mat = &(npc->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;

	zCModel* model = npc->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zVEC3 oldPos = zVEC3(heroLevitationBean.oldXPos, heroLevitationBean.oldYPos, heroLevitationBean.oldZPos);

	if (mCustomCollisionDetected = checkCollision(npc, *mat))
	{
		npc->SetPositionWorld(oldPos);

		//if (checkCollision(hero, *mat))
		//{
		//	logStream << "Test: stuck detected!" << std::endl;
		//	util::logInfo(&logStream);
		//	zVEC3 normal = getCollidingPolyNormal(hero, *mat);
			//hero->SetPositionWorld(oldPos + normal*20);
		//} 
	}

	return mCustomCollisionDetected;
}

std::ostream & operator<<(std::ostream & os, const Plane & p)
{
	os << "(" << p.a << ", " << p.b << ", " << p.c << ", "<< 
		p.d << ")";
	return os;
}

__int32 __cdecl Levitation::sysGetTime() {
	XCALL(0x00505280);
};

std::ostream& operator<<(std::ostream& os, const zTSpatialState& spatialState)
{
	os << "m_fFloorY = " << spatialState.m_fFloorY << std::endl;
	os << "m_fWaterY = " << spatialState.m_fWaterY << std::endl;
	os << "m_fCeilingY = " << spatialState.m_fCeilingY << std::endl;
	os << "m_fLastFloorY = " << spatialState.m_fLastFloorY << std::endl;

	os << "m_bFloorIsStair = " << (bool)spatialState.m_bFloorIsStair << std::endl;
	os << "m_bFloorIsVob = " << (bool)spatialState.m_bFloorIsVob << std::endl;
	os << "m_bIsUninited = " << (bool)spatialState.m_bIsUninited << std::endl;

	return os;
}

int __thiscall zCMaterial::IsPortalMaterial(void* pThis)
{
	XCALL(0x005648D0);
};

const float Float::EPSILON = 0.001f;