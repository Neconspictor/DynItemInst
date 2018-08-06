#include <Levitation.h>
#include <HookManager.h>
#include <api/g2/zcworld.h>
#include <api/g2/zcinput.h>
#include <api/g2/zcmodel.h>
#include <api/g2/ocgame.h>
#include <LevitationBean.h>
#include <Util.h>
#include <queue>
#include <api/g2/zcparser.h>
#include <api/g2/oCItemExtended.h>

using namespace LevitationUtil;

typedef void(__thiscall* ZCVobDoFrameActivity)(void*); ZCVobDoFrameActivity zCVobDoFrameActivity;
typedef void(__thiscall* ZCVobSetPhysicsEnabled)(void*, int); ZCVobSetPhysicsEnabled zCVobSetPhysicsEnabled;
typedef void(__cdecl* ZCVobCheckAutoUnlink)(zCVob*); ZCVobCheckAutoUnlink zCVobCheckAutoUnlink;
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
bool Levitation::adjustHeroPosition = false;
bool Levitation::pausedGame = false;
bool Levitation::noCollision = false;
int Levitation::frameTime = 0;
int Levitation::diffFrameTime = 0;
float Levitation::yPos = 0;

LevitationBean* heroLevitationBean;
bool heroLevitationBeanCalled;
bool fakeUpKey;
int frameTimePast;



Levitation::Levitation()
	:Module()
{
	moduleDesc = "Levitation";
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

	zCVobCheckAutoUnlink = reinterpret_cast<void(__cdecl*)(zCVob*)>(ZCVOB_CHECK_AUTO_UNLINK_ADDRESS);
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
	hookManager->addFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobSetPhysicsEnabled, zCVobSetPhysicsEnabledHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGamePause, oCGamePauseHook, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&oCGameUnpause, oCGameUnpauseHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&testStatic_Char_Char, TestStatic_Char_CharHook, moduleDesc);



	
	//hookManager->addFunctionHook((LPVOID*)&zCCollObjectCharacterCalcSlideVector, zCCollObjectCharacterCalcSlideVectorHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zMAT4SetTranslation, zMAT4SetTranslationHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobCalcGroundPoly, zCVobCalcGroundPolyHook, moduleDesc);
	
	
	hookManager->addFunctionHook((LPVOID*)&zCVobUpdatePhysics, zCVobUpdatePhysicsHook, moduleDesc);


	hookManager->addFunctionHook((LPVOID*)&oCAIHumanPC_ActionMove, oCAIHumanPC_ActionMoveHook, moduleDesc);




	
	hookManager->addFunctionHook((LPVOID*)&oCItemContainerDrawCategory, oCItemContainerDrawCategoryHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&oCItemContainerGetSelectedItem, oCItemContainerGetSelectedItemHook, moduleDesc);
	
	//hookManager->addFunctionHook((LPVOID*)&oCItemContainerDrawGetItem, oCItemContainerDrawGetItemNaked, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&oCItemContainerNextItem, oCItemContainerNextItemHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCItemContainerNextItem, oCItemContainerNextItemReversed, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&oCItemContainerDraw, oCItemContainerDrawHook, moduleDesc);

	
	
	
	
	//hookManager->addFunctionHook((LPVOID*)&zCVobEndMovement, zCVobEndMovementHook, moduleDesc);
	
	
	//hookManager->addFunctionHook((LPVOID*)&zCCollObjectCharacterTestHardCollisions, zCCollObjectCharacterTestHardCollisionsHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCCollObjectCharacterTestSoftCollisions, zCCollObjectCharacterTestSoftCollisionsHook, moduleDesc);
	
	
	//hookManager->addFunctionHook((LPVOID*)&zCCollObjectCharacterDetectCollisionsSelf, zCCollObjectCharacterDetectCollisionsSelfHook, moduleDesc);
	
	//hookManager->addFunctionHook((LPVOID*)&zCAIPlayerCheckPhysics, zCAIPlayerCheckPhysicsHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCCollObjectCharacterFindFloorWaterCeiling, zCCollObjectCharacterFindFloorWaterCeilingHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&doSurfaceAlignment, DoSurfaceAlignmentHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobCheckAndResolveCollisions, zCVobCheckAndResolveCollisionsHook, moduleDesc);

	//hookManager->addFunctionHook((LPVOID*)&zCTriggerOnTouch, zCTriggerOnTouchHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobHasEnoughSpace, zCVobHasEnoughSpaceHook, moduleDesc);

	//hookManager->addFunctionHook((LPVOID*)&zCAIPlayerCheckEnoughSpaceMoveDir, zCAIPlayerCheckEnoughSpaceMoveDirHook, moduleDesc);
}

void Levitation::unHookModule()
{
}

zVEC3 levitatePosition;
bool doHardTests = false;

void Levitation::zCVobDoFrameActivityHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	if (!heroLevitationBeanCalled) {
		heroLevitationBean = new LevitationBean(hero);
		heroLevitationBeanCalled = true;
	}
	bool adjust = (hero == pThis) && adjustHeroPosition;
	float oldYPosition = 0;
	zVEC3 oldLook;


	if (pThis == hero)
	{
		frameTimePast = calcPastFrameTime();
	}

	if (adjust) {
		//oldYPosition = hero->GetPosition().y;
		char* modelPtr = (char*)hero->GetModel();

		modelPtr[0x1F8] = 0xD;

		zMAT4* mat = &hero->trafoObjToWorld;
		oldLook = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		heroLevitationBean->oldLook = oldLook;
		//heroLevitationBean->oldXPos = mat->m[0][3];
		//heroLevitationBean->oldYPos = mat->m[1][3];
		//heroLevitationBean->oldZPos = mat->m[2][3];


		//frameTimePast = calcPastFrameTime();
		//Test(hero);

		/*if (collisionObject != nullptr)
		{
			levitatePosition = hero->GetPositionWorld();
			zMAT4* matCollisionObject = (zMAT4*)((char*)collisionObject + 0x44);
			matCollisionObject->_14 = levitatePosition.x;
			matCollisionObject->_24 = levitatePosition.y;
			matCollisionObject->_34 = levitatePosition.z;
			//zVEC3 translation(mat->_14, mat->_24, mat->_34);
		}*/

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

		/*zVEC3 pos;
		hero->GetPositionWorld(pos.x, pos.y, pos.z);
		float hoverDistance = heroLevitationBean->getDistanceToGround(pos);
		if (hoverDistance < heroLevitationBean->HOVER_DISTANCE)
		{
			//fakeUpKey = true;
			float speed = 10.0f; //100cm per second
			float moveUpDistance = speed * float(frameTimePast) / 1000;
			hero->GetPositionWorld(pos.x, pos.y, pos.z);
			pos.y += moveUpDistance;
			hero->SetPositionWorld(pos);
			heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(pos));
			//heroLevitationBean->increaseHoverDistance(moveUpDistance);
		}*/

		zVEC3 pos;
		hero->GetPositionWorld(pos.x, pos.y, pos.z);
		float hoverDistance = heroLevitationBean->getDistanceToGround(pos);
		float add = 40 - hoverDistance;
		pos.y += max(0, add);
		hero->SetPositionWorld(pos);
	}
	zCVobDoFrameActivity(pThis);

	if (adjust) {
		//noCollision = false;
		//zCVobSetCollDetHook(hero, 1);

		//hero->ResetXZRotationsWorld();
		//zCVobSetPhysicsEnabled(hero, true);
		//noCollision = true;

		//zVEC3 pos = hero->GetPosition();
		//if (pos.y < oldYPosition) {
		//	hero->SetPositionWorld(zVEC3(pos.x, oldYPosition, pos.z));
		//}

		/*float hoverDistance = heroLevitationBean->getHoverDistance();
		if (hoverDistance < heroLevitationBean->HOVER_DISTANCE)
		{
			//fakeUpKey = true;
			float speed = 10.0f; //100cm per second
			float moveUpDistance = speed * float(frameTimePast) / 1000;
			pos = hero->GetPosition();
			pos.y += moveUpDistance;
			//hero->SetPositionWorld(pos);
			heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(pos));
			//heroLevitationBean->increaseHoverDistance(moveUpDistance);
		}*/

		//Test(hero);
		//levitatePosition = levitate();
		Test(hero);
	}
}

void Levitation::zCVobSetPhysicsEnabledHook(void* pThis, int second)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero) && Levitation::adjustHeroPosition) {
		zCInput* input = zCInput::GetInput();
		//if (input->KeyPressed(0x1A)) return; //zCVobSetPhysicsEnabled(pThis, true);
		return;
	}
	zCVobSetPhysicsEnabled(pThis, second);
}

void Levitation::oCGamePauseHook(void* pThis, int second)
{
	logStream << "Paused Game!" << std::endl;
	util::debug(&logStream);
	Levitation::pausedGame = true;
	oCGamePause(pThis, second);
}

void Levitation::oCGameUnpauseHook(void* pThis)
{
	logStream << "Unpaused Game!" << std::endl;
	util::debug(&logStream);
	Levitation::pausedGame = false;
	oCGameUnpause(pThis);
}

void Levitation::zCVobSetCollDetHook(void* pThis, int second)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero) && adjustHeroPosition) {
		return zCVobSetCollDet(pThis, 0);
	}
	zCVobSetCollDet(pThis, second);
}

int Levitation::zCAIPlayerCheckFloorSlidingHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero) && adjustHeroPosition) {
		return 1;
	}
	return zCAIPlayerCheckFloorSliding(pThis);
}

void Levitation::DoSurfaceAlignmentHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (hero == pThis) && Levitation::adjustHeroPosition;
	if (adjust) {
		return;
	}
	return doSurfaceAlignment(pThis);
}

void Levitation::zCVobCheckAndResolveCollisionsHook(void* pThis)
{

	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	bool adjust = (pThis == hero) && Levitation::adjustHeroPosition && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCVobCheckAndResolveCollisionsHook (before):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&logStream);
	}
	zCVobCheckAndResolveCollisions(pThis);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCVobCheckAndResolveCollisionsHook (after):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&logStream);
	}
}

void Levitation::zCTriggerOnTouchHook(void* pThis, zCVob* vob)
{
	if (vob == oCNpc::GetHero()) {
		logStream << "zCTriggerOnTouch() called on hero!!" << std::endl;
		util::debug(&logStream);
		zCTriggerOnTouch(pThis, vob);
	}
}

int Levitation::zCVobHasEnoughSpaceHook(void* pThis, zVEC3& second)
{
	int result = zCVobHasEnoughSpace(pThis, second);
	if (pThis == oCNpc::GetHero()) {
		logStream << "called for hero!: " << result << std::endl;
		util::debug(&logStream);
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

	bool adjust = (pThis == collisionObject) && Levitation::adjustHeroPosition && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCCollObjectCharacterCalcSlideVectorHook (before):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		logStream << "\tvec1 = " << vec1 << std::endl;
		logStream << "\tvec2 = " << vec2 << std::endl;
		logStream << "\tvec3 = " << vec3 << std::endl;
		logStream << "\tvec4 = " << vec4 << std::endl;
		logStream << "\tfloatValue = " << floatValue << std::endl;
		util::logWarning(&logStream);
	}

	int result = zCCollObjectCharacterCalcSlideVector(pThis, vec1, vec2, vec3, vec4, floatValue);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCCollObjectCharacterCalcSlideVectorHook (after):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		logStream << "\tvec1 = " << vec1 << std::endl;
		logStream << "\tvec2 = " << vec2 << std::endl;
		logStream << "\tvec3 = " << vec3 << std::endl;
		logStream << "\tvec4 = " << vec4 << std::endl;
		logStream << "\tfloatValue = " << floatValue << std::endl;
		util::logWarning(&logStream);
		util::logWarning(&logStream);
	}

	return result;
}

bool activatezMAT4Logging = false;

zMAT4& Levitation::zMAT4SetTranslationHook(zMAT4* pThis, zVEC3 const& translation)
{
	if (activatezMAT4Logging)
	{
		zVEC3 trans(pThis->_14, pThis->_24, pThis->_34);
		logStream << "zMAT4SetTranslationHook (before):" << std::endl;
		logStream << "\ttrans = " << trans << std::endl;
		util::logWarning(&logStream);
	}

	zMAT4& result = zMAT4SetTranslation(pThis, translation);

	if (activatezMAT4Logging)
	{
		zVEC3 trans(pThis->_14, pThis->_24, pThis->_34);
		logStream << "zMAT4SetTranslationHook (after):" << std::endl;
		logStream << "\ttrans = " << trans << std::endl;
		util::logWarning(&logStream);
	}

	return result;
}

void Levitation::zCVobCalcGroundPolyHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	bool adjust = (pThis == hero) && Levitation::adjustHeroPosition && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCVobCalcGroundPolyHook (before):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&logStream);
	}
	zCVobCalcGroundPoly(pThis);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCVobCalcGroundPolyHook (after):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&logStream);
	}
}

void Levitation::zCVobUpdatePhysicsHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	zVEC3 translationBefore;

	bool adjust = (pThis == hero) && Levitation::adjustHeroPosition && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		translationBefore = zVEC3(mat->_14, mat->_24, mat->_34);
		//logStream << "zCVobUpdatePhysicsHook (before):" << std::endl;
		//logStream << "\ttranslation = " << translation << std::endl;
		//util::logWarning(&logStream);
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
		//util::logWarning(&logStream);
	}
}

void Levitation::zCVobEndMovementHook(void* pThis, int arg0)
{
	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	bool adjust = (pThis == hero) && Levitation::adjustHeroPosition && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCVobEndMovementHook (before):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&logStream);
	}
	zCVobEndMovement(pThis, arg0);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCVobEndMovementHook (after):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&logStream);
	}
}

int Levitation::zCCollObjectCharacterTestSoftCollisionsHook(void* pThis, zVEC3 const& vec1, zVEC3& vec2, zVEC3& vec3,
	zVEC3& vec4)
{
	oCNpc* hero = oCNpc::GetHero();

	bool adjust = (pThis != nullptr) && Levitation::adjustHeroPosition && (pThis == hero->m_poCollisionObject);
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

	bool adjust = (pThis == collisionObject) && Levitation::adjustHeroPosition && (collisionObject != nullptr);
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

		//util::logWarning(&logStream);

		if (log)
		{

			//vec3.y = vec2.y;
			float temp = vec3.y + 10;
			vec3.y = max(vec2.y, temp);

			if (result)
			{
				//logStream << "zCCollObjectCharacterTestHardCollisionsHook: after:" << std::endl;
				//logStream << "result is = " << result << std::endl;
				//util::logWarning(&logStream);
				//vec3.y = vec2.y;
				
			}

			/*//zVEC3 velo = zCVobGetVelocity(hero);
			logStream << "zCCollObjectCharacterTestHardCollisionsHook: after:" << std::endl;
			//logStream << "velo = " << velo << std::endl;
			logStream << "vec1(after) = " << vec1 << std::endl;
			logStream << "vec2(after) = " << vec2 << std::endl;
			logStream << "vec3(after) = " << vec3 << std::endl;
			logStream << "vec4(after) = " << vec4 << std::endl;
			logStream << "zTSpatialState(after) = " << *zTSpatialState << std::endl;
			util::logWarning(&logStream);
			*/


			//zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
			//hero->GetPositionWorld();
			//zVEC3 translation(mat->_14, mat->_24, mat->_34);
			//translation = hero->GetPositionWorld();
			//vec3.y = vec2.y;
			//vec3.y = translation.y;

			//float temp = vec2.y + levitatePosition.y;
			//vec3.y = max(vec3.y, temp);
			//vec3.y += 20;

			//if (temp > vec3.y)
			//	vec3.y = temp;
			//vec1.y = vec3.y;
		}
		return result;
	}
	return zCCollObjectCharacterTestHardCollisions(pThis, vec1, vec2, vec3, zTSpatialState, vec4);
}

void Levitation::zCCollObjectCharacterDetectCollisionsSelfHook(void* pThis, void* collisionObjectArray,
	void* collisionReports)
{

	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (pThis != nullptr) && Levitation::adjustHeroPosition && (pThis == hero->m_poCollisionObject);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)pThis + 0x44);
		zVEC3 translation (mat->_14, mat->_24, mat->_34);
		logStream << "zCCollObjectCharacterDetectCollisionsSelfHook (before):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&logStream);


	}

	zCCollObjectCharacterDetectCollisionsSelf(pThis, collisionObjectArray, collisionReports);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)pThis + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCCollObjectCharacterDetectCollisionsSelfHook (after):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&logStream);
	}
}

void Levitation::zCAIPlayerCheckPhysicsHook(void* pThis)
{
	bool adjust = (pThis == oCNpc::GetHero()) && (pThis != NULL) && Levitation::adjustHeroPosition;
	if (adjust) {
		return;
	}
	zCAIPlayerCheckPhysics(pThis);
}

void Levitation::zCCollObjectCharacterFindFloorWaterCeilingHook(void* pThis, zVEC3 const& vec1, zTSpatialState* zTSpatialState)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (pThis != nullptr) && Levitation::adjustHeroPosition && (pThis == hero->m_poCollisionObject);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)pThis + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCCollObjectCharacterFindFloorWaterCeilingHook (before):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		logStream << "\tzTSpatialState = " << *zTSpatialState << std::endl;
		util::logWarning(&logStream);
	}
	zCCollObjectCharacterFindFloorWaterCeiling(pThis, vec1, zTSpatialState);

	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)pThis + 0x44);
		zVEC3 translation(mat->_14, mat->_24, mat->_34);
		logStream << "zCCollObjectCharacterFindFloorWaterCeilingHook (after):" << std::endl;
		logStream << "\ttranslation = " << translation << std::endl;
		logStream << "\tzTSpatialState = " << *zTSpatialState << std::endl;
		util::logWarning(&logStream);
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
	if (result && hero != NULL && (pThis == hero) && adjustHeroPosition) {
		logStream << "zCAIPlayerCheckEnoughSpaceMoveDirHook: called!"  << std::endl;
		util::logInfo(&logStream);
		return 0;
	}

	return result;
}

int Levitation::oCAIHumanPC_ActionMoveHook(void* pThis, int param1)
{
	void* hero = (void*)oCNpc::GetHero();

	bool adjust = adjustHeroPosition && pThis != nullptr;

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

void Levitation::getState(oCItemContainer& container, ContainerState& state)
{
	state.selectedItemNumber = container.selectedItem;

	int index = 0;
	zCListSort<oCItem>* contents = container.contents;

	while(contents->data == nullptr)
	{
		contents = contents->next;
	}

	// get the selected item
	while(index < state.selectedItemNumber)
	{
		// Important: at first updating the index!
		if (contents->data != nullptr)
			++index;

		contents = contents->next;
	}
	state.selectedItem = contents->data;
	state.selectedItemList = contents;


	// get the total item number
	while (contents->next != nullptr) {
		
		// Important: at first updating the index!
		if (contents->data != nullptr)
			++index;

		contents = contents->next;
		
	}
	state.itemNumber = index;

	// calculate row and column indices
	state.selectedItemRowNumber = -1;
	state.selectedItemColumnNumber = -1;
	if (state.selectedItemNumber > 0)
	{
		index = state.selectedItemNumber - container.offset;
		state.selectedItemRowNumber = (index / container.maxSlotsRow); // row index begins at 0
		state.selectedItemColumnNumber = index % container.maxSlotsRow; // column index begins at 0	
	}
}

void Levitation::oCItemContainerDrawCategoryHook(void* pThis)
{
	oCItemContainerDrawCategory(pThis);
}

oCItem* Levitation::oCItemContainerGetSelectedItemHook(oCItemContainer* pThis)
{
	oCItem* selectedItem = oCItemContainerGetSelectedItem(pThis);
	if (selectedItem != nullptr && meetsCondition(selectedItem))
	{
		logStream << "oCItemContainerGetSelectedItemHook: called!" << std::endl;
		util::logInfo(&logStream);
		oCItemContainerNextItemHook(pThis);
		selectedItem = oCItemContainerGetSelectedItem(pThis);
	}
	return selectedItem;
}

void Levitation::oCItemContainerDrawHook(oCItemContainer* pThis)
{
	oCItem* selectedItem = oCItemContainerGetSelectedItem(pThis);
	if (selectedItem != nullptr && meetsCondition(selectedItem))
	{
		oCItemContainerNextItemHook(pThis);
	}
	oCItemContainerDraw(pThis);
}

bool Levitation::meetsCondition(oCItem* item)
{
	return item->amount <= 1;
}

oCItem* Levitation::myTestFunction(oCItem** currentItem, zCListSort<oCItem>** pointer, oCItem* item)
{
	if (item == nullptr) return nullptr;

	// categories (mainflag)
	static const int ITEM_KAT_NONE = 1 << 0;  // Sonstiges
	static const int ITEM_KAT_NF = 1 << 1;  // Nahkampfwaffen
	static const int ITEM_KAT_FF = 1 << 2;  // Fernkampfwaffen
	static const int ITEM_KAT_MUN = 1 << 3;  // Munition (MULTI)
	static const int ITEM_KAT_ARMOR = 1 << 4;  // Ruestungen
	static const int ITEM_KAT_FOOD = 1 << 5;  // Nahrungsmittel (MULTI)
	static const int ITEM_KAT_DOCS = 1 << 6;  // Dokumente
	static const int ITEM_KAT_POTIONS = 1 << 7;  // Traenke
	static const int ITEM_KAT_LIGHT = 1 << 8;  // Lichtquellen
	static const int ITEM_KAT_RUNE = 1 << 9;  // Runen/Scrolls
	static const int ITEM_KAT_MAGIC = 1 << 31;  // Ringe/Amulette/Guertel
	static const int ITEM_KAT_KEYS = ITEM_KAT_NONE;

	while(meetsCondition(item)) //item->mainflags & ITEM_KAT_NF || item->mainflags & ITEM_KAT_FF
	{
		if (pointer == nullptr) {
			*currentItem = nullptr;
			return nullptr;
		}
		*pointer = (*pointer)->next;
		if (*pointer == nullptr) {
			*currentItem = nullptr;
			return nullptr;
		}
		zCListSort<oCItem>* list = *pointer;
		item = list->data;
		*currentItem = item;

		if (item == nullptr) return nullptr;
	}

	/*std::stringstream ss;
	ss << "size: " << pointer->GetSize() << std::endl;
	if (pointer->GetSize() > 0)
	{
		ss << "first item name: " << pointer->data[0].name.ToChar() << std::endl;
		ss << "first item name: " << pointer->data[0].amount << std::endl;
	}
	util::logAlways(&ss);*/

	return item;
}

__declspec(naked) void oCItemContainerDrawGetItemNaked()
{

	//Pointer_toPointerOfItem= dword ptr -18Ch
	// mov     eax, [esp+1B4h+Pointer_toPointerOfItem]
	// esp + 0x1b4 - 0x18c
	// esp +0x1b4 - 0x18c - 0x20
	_asm {
		push esp - 4 // esp -4
		push ecx // esp -8
		push edx // esp -12
		push ebx // esp -16
		push ebp // esp -20
		push edi // esp -24
		push esi // esp -28
		push eax // esp -32 = 0x20
		mov eax, esp //+ 0x1b4 - 0x18c + 0x20]
		add eax, 0x1b4 - 0x18c + 0x20
		push eax // esp - 36 = 0x24
		mov eax, esp
		add eax, 0x1b4 - 0x190 + 0x24
		push eax
		call Levitation::myTestFunction
		add esp, 0xC // 3x push eax
		pop esi
		pop edi
		pop ebp
		pop ebx
		pop edx
		pop ecx
		pop esp
		test eax, eax
		jz label
		push 0x00707BAA
		ret
		label :
		push 0x007083B4
			ret
	}
}

void Levitation::oCItemContainerNextItemHook(oCItemContainer* pThis)
{
	oCItemContainer* container = pThis;

	ContainerState state {0};
	getState(*container, state);

	zCListSort<oCItem>* contents = state.selectedItemList;

	int index = state.selectedItemNumber;
	if (contents != nullptr)
		contents = contents->next;

	bool found = false;

	while (contents != nullptr && !found)
	{
		++index;
		oCItem* selectedItem = contents->data;
		if (selectedItem != nullptr && !meetsCondition(selectedItem))
		{
			// we have what we want!
			found = true;
		}
		contents = contents->next;
	}

	// update state of container when there was found a valid item
	if (found)
	{
		container->selectedItem = index;
	}
}

int ITEM_ACTIVE = 0x40000000;

void sub(oCItemContainer* container, int& posX, int& posY, int selectedItemIndex, int containerOffset)
{

	loc_709780: // TODO: remove gotos

	zCListSort<oCItem>* contents = container->contents;
	zCListSort<oCItem>* contentsNext = contents->next;

	int lastItemIndex = 0;

	while (contentsNext != nullptr)
	{
		contentsNext = contentsNext->next;
		++lastItemIndex;
	}

	--lastItemIndex; // index is 'item count - 1'

	if (container->selectedItem < lastItemIndex) //.text:0070979C                 cmp     container->selectedItem, lastItemIndex
												 //.text:0070979E                 jge     loc_709899
	{

			if (oCItemContainer::IsSplitScreen(container))  // else will jump to .text:00709853 loc_709853:
			{
				//int bufferFlowCheck = (container->selectedItem + 1) / container->maxSlotsCol; // .text:007097B7                 cdq
				int modulo = (container->selectedItem + 1) % container->maxSlotsCol; // .text:007097B7                 cdq

				if (modulo == 0) // no buffer overflow has happened; else will jump to .text:00709853 loc_709853:
				{
					oCItemContainer* nextActiveContainer = nullptr;
					oCItemContainer::GetPosition(container, posX, posY);

					//.data:00AB0FD8 ; void *dword_AB0FD8
					zCList<oCItemContainer>* openContainers = *(zCList<oCItemContainer>**)0x00AB0FD8;

					while (openContainers != nullptr)
					{
						oCItemContainer* currentContainer = openContainers->data;

						if (currentContainer != container)
						{
							if (!oCItemContainer::IsPassive(currentContainer) && oCItemContainer::IsOpen(currentContainer))
							{
								int currentContainerPosX;
								int currentContainerPosY;
								oCItemContainer::GetPosition(currentContainer, currentContainerPosX, currentContainerPosY);

								if (currentContainerPosX > posX)
								{
									nextActiveContainer = currentContainer;
								}
							}
						}

						//.text:0070982E loc_70982E:
						openContainers = openContainers->next;

					}

					//.text:00709835                 mov     eax, [esp+2Ch+oCItemContainer_var]

					if (nextActiveContainer != nullptr)
					{
						oCItemContainer::Activate(nextActiveContainer);
					}

					//.text:00709844 loc_709844: 
					if (nextActiveContainer != nullptr) //.text:00709851                 jnz     short loc_7098A7
					{
						goto loc_7098A7;
					}
				}
			}

			//.text:00709853 loc_709853:
			container->selectedItem = container->selectedItem + 1;
			int lastVisibleItemIndex = container->maxSlots + container->offset - 1;


			if (container->selectedItem > lastVisibleItemIndex)
			{
				container->offset = container->offset + container->maxSlots;
			}

			//.text:00709872 loc_709872: 

			if (container->m_bManipulateItemsDisabled == 0)
			{
				goto loc_7098A7;
			}

			// We know that there are enough items as 'container->selectedItem < lastItemIndex'; 
			// Thus there won't be a nullptr exception!
			if (oCItemContainer::GetSelectedItem(container)->HasFlag(ITEM_ACTIVE)) {
				//.text:00709891                 jnz     loc_709780
				goto loc_709780;
			}
	}
	else
	{
		//.text:00709899 loc_709899:
		container->selectedItem = selectedItemIndex;
		container->offset = containerOffset;
	}

loc_7098A7:;
}

void Levitation::oCItemContainerNextItemReversed(oCItemContainer* container)
{
	int posX;
	int posY;

	if (container->contents == nullptr) return;

	int selectedItemIndex = container->selectedItem;
	int containerOffset = container->offset;


	if (oCItemContainerGetSelectedItem(container) != nullptr)
	{
		oCItem* selectedItem = oCItemContainerGetSelectedItem(container);
		selectedItem->RotateForInventory(1.0f);
	}

	//.text:00709780 loc_709780: 

	sub(container, posX, posY, selectedItemIndex, containerOffset);

	//.text:007098A7 loc_7098A7:

	if (oCItemContainer::IsSplitScreen(container))
	{
		if (selectedItemIndex == container->selectedItem)
		{
			if (containerOffset == container->offset)
			{
				selectedItemIndex = 0;
				int posX;
				int posY;
				oCItemContainer::GetPosition(container, posX, posY);

				//.data:00AB0FD8 ; void *dword_AB0FD8
				zCList<oCItemContainer>* openContainers = *(zCList<oCItemContainer>**)0x00AB0FD8;

				if (openContainers != nullptr)
				{
					oCItemContainer* activeContainer = nullptr;
					do
					{
						oCItemContainer* currentContainer = openContainers->data;
						if (currentContainer != container)
						{
							if (!oCItemContainer::IsPassive(currentContainer))
							{
								if (oCItemContainer::IsOpen(currentContainer))
								{
									int currentPosX;
									int currentPosY;
									oCItemContainer::GetPosition(currentContainer, currentPosX, currentPosY);

									if (currentPosX > posX)
									{
										activeContainer = currentContainer;
										posX = currentPosX;
									}
								}
							}
						}

						//.text:0070993E loc_70993E:
						openContainers = openContainers->next;
					} while (openContainers != nullptr);

					if (activeContainer != nullptr)
					{
						oCItemContainer::Activate(activeContainer);
					}

					
				}
			}
		}
		
	} 
	//.text:00709952 loc_709952:  	

	if (selectedItemIndex >= 0)
	{
		if (container->m_bManipulateItemsDisabled != 0)
		{
			if (oCItemContainer::GetSelectedItem(container) != nullptr)
			{
				if (oCItemContainer::GetSelectedItem(container)->HasFlag(ITEM_ACTIVE))
				{
					container->selectedItem = -1;
					container->offset = containerOffset;
				}
			}
		}
	}

}

// TODO: Floor aligning doesn't work properly on certain cases; keyword: consider poly normals!
void doFloorAligning(zVEC3* finalPosition, zMAT4* mat)
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
};

zVEC3 levitate() {

	zVEC3 positionAdd(0, 0, 0);

	if (!Levitation::adjustHeroPosition) return positionAdd;
	if (Levitation::pausedGame) return positionAdd;

	std::stringstream logStream;

	oCNpc* hero = oCNpc::GetHero();
	//if (oCNpcIsMovLock(hero)) return positionAdd;

	float angle = 0;

	//Levitation::noCollision = true;
	float speed = 100.0f; //100cm per second
	float distance = speed * float(frameTimePast) / 1000;
	zVEC3 oldLook = heroLevitationBean->oldLook;
	zCInput* input = zCInput::GetInput();

	hero->ResetXZRotationsLocal();
	int arrowLeft = input->KeyPressed(203);
	int arrowRight = input->KeyPressed(205);
	int arrowUp = input->KeyPressed(200);
	int moveUp = input->KeyPressed(0x1A);
	int moveDown = input->KeyPressed(0x28);

	if (!arrowLeft && !arrowRight)
	{
		zMAT4* mat = &hero->trafoObjToWorld;
		mat->m[0][2] = oldLook.x;
		mat->m[1][2] = oldLook.y;
		mat->m[2][2] = oldLook.z;
	}

	if (arrowLeft)
	{
		float speed = -90.0;
		angle += speed * float(frameTimePast) / 1000;
	}
	else if (arrowRight)
	{
		float speed = 90.0;
		angle += speed * float(frameTimePast) / 1000;
	}

	// Pressed � (levitation up)
	if (moveUp) {
		positionAdd.y += distance;
	}
	// Pressed � (levitation down)
	else if (moveDown) {
		positionAdd.y -= distance;
	}

	// Pressed Arrow_Up (levitation forward)
	if (input->KeyPressed(0xC8)) {
		zMAT4* mat = &hero->trafoObjToWorld;
		zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		positionAdd.x += look.x * 5;
		positionAdd.y += look.y * 5;
		positionAdd.z += look.z * 5;
	}
	// Pressed Arrow_Down (levitation backward)
	else if (input->KeyPressed(0xD0)) {
		//Motion motion = getCollideForwardDir(hero, 0, true);
	}

	//finally apply changes
	rotateLocalY(hero, angle);
	zVEC3 finalPosition = hero->GetPositionWorld();
	heroLevitationBean->oldXPos = finalPosition.x;
	heroLevitationBean->oldYPos = finalPosition.y;
	heroLevitationBean->oldZPos = finalPosition.z;
	finalPosition += positionAdd;

	hero->SetPositionWorld(finalPosition);
	//heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(finalPosition));

	return positionAdd;
};

Motion getCollideYDir(zVEC3 pos, float middlePointDist, bool up) {
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
	LevitationBean::zCWorldTraceRayNearestHit(world, pos, dir, NULL, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs(intersection->y - pos.y);
	motion.objectPos = pos;
	motion.intersect = *intersection;
	return motion;
};

Motion getCollideForwardDir(oCNpc* npc, float middlepointDistance, bool backward) {
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
	LevitationBean::zCWorldTraceRayNearestHit(world, pos, posOutside, NULL, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs((intersection->x - pos.x) + (intersection->y - pos.y) + (intersection->z - pos.z));
	motion.objectPos = pos;
	motion.intersect = *intersection;
	return motion;
};


int calcPastFrameTime() {
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
	zTBBox3D box = LevitationBean::zCModelGetBBox3D(model);

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
			zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);
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

bool check_prePass(oCNpc* hero, const zMAT4& mat)
{
	bool intersected = false;
	zVEC3 pos(mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)hero + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int third = 0;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = LevitationBean::zCModelGetBBox3D(model);

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

	std::stringstream logStream;
	zCArray<void*>* leafObjects = &vob->vobLeafList;
	zCPolygon** polys;
	//int third;
	logStream << "checkVobCollision__checkVob(): Check object with leaf number: " << leafObjects->GetSize() << std::endl;
	logStream << "checkVobCollision__checkVob(): visual name: " << vob->GetObjectName().ToChar() << std::endl;
	util::logWarning(&logStream);

	for (unsigned int i = 0; i < leafObjects->GetSize(); ++i)
	{
		zCBspBase_Small* leaf = (zCBspBase_Small*)leafObjects->GetItem(i);
		//Levitation::zCBspBaseCollectPolysInBBox3D(leaf, boundingBox, polys, third);
		logStream << "checkVobCollision__checkVob(): leave has poly count: " << leaf->numPolys << std::endl;
		//logStream << "checkVobCollision__checkBspLeaf(): Found polys: " << third << std::endl;
		util::logWarning(&logStream);

		zCPolygon** polyList = (zCPolygon**)leaf->polyList;

		for (int j = 0; j < leaf->numPolys; ++j)
		{
			zCPolygon* poly = polyList[j];

			//logStream << "checkVobCollision__checkVob(): pos = " << pos << std::endl;
			//logStream << "checkVobCollision__checkVob(): vob pos = " << vob->GetPositionWorld() << std::endl;
			//util::logWarning(&logStream);
			if (poly->CheckBBoxPolyIntersection(boundingBox))
			{
				logStream << "checkVobCollision__checkVob(): Intersection found! " << std::endl;
				util::logWarning(&logStream);
				return true;
			}
		}
		
	}
	return false;
}


bool checkVobCollision(void* zCBspBaseObject, zCVob* testedVob, const zTBBox3D& boundingBox)
{
	zCArray<zCVob*> collectedVobs;
	Levitation::zCBspBaseCollectVobsInBBox3D(zCBspBaseObject, collectedVobs, boundingBox);

	if (collectedVobs.GetSize() > 0)
	{
		std::stringstream logStream;
		logStream << "checkVobCollision(): Found vobs!: " << collectedVobs.GetSize() << std::endl;
		util::logWarning(&logStream);

		for (unsigned int i = 0; i < collectedVobs.GetSize(); ++i)
		{
			logStream << "checkVobCollision(): test vob with number: " << i << std::endl;
			util::logWarning(&logStream);

			zCVob* vob = collectedVobs.GetItem(i);
			if (vob != testedVob && vob != nullptr)
			{
				int zCVob_bitfield0_collDetectionDynamic = ((1 << 1) - 1) << 7;
				int zCVob_bitfield0_staticVob = ((1 << 1) - 1) << 4;
				bool collidesWithDynamicVobs = vob->bitfield[0] & zCVob_bitfield0_collDetectionDynamic;
				bool isStaticVob = vob->bitfield[0] & zCVob_bitfield0_staticVob;
				//bool hasCollisionObject = vob->m_poCollisionObject != nullptr;
				if (collidesWithDynamicVobs && isStaticVob)
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
	zTBBox3D box = LevitationBean::zCModelGetBBox3D(model);

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
			zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);
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
					char bitflag = *((char*)(char*)material + 0x70);
					bool hasNoCollision = bitflag & (1 << 4);

					const char* materialName = material->GetName().ToChar();
					bool isGhostOccluder = strcmp(materialName, "GHOSTOCCLUDER") == 0;
					bool isNW_MISC_FULLALPHA_01 = strcmp(materialName, "NW_MISC_FULLALPHA_01") == 0;

					if (hasNoCollision)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with no collision material" << std::endl;
						//util::logWarning(&logStream);
						continue;
					}

					if (isGhostOccluder)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with GHOSTOCCLUDER material" << std::endl;
						//util::logWarning(&logStream);
						continue;
					}

					if (isNW_MISC_FULLALPHA_01)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with NW_MISC_FULLALPHA_01 material" << std::endl;
						//util::logWarning(&logStream);
						continue;
					}

				

					if (!intersected && !hasNoCollision && !isGhostOccluder && !isNW_MISC_FULLALPHA_01) {
						intersected = true;

						//std::stringstream logStream;
						//logStream << "checkCollision(): intersection found!: " << materialName << std::endl;
						//util::logWarning(&logStream);
						//we have found what we wanted -> a collision!
						break;
					}
				}
			}
		}
	}

	//if (!intersected)
		//intersected = checkVobCollision(pointer, hero, rec);

	return intersected;
}


void Test(oCNpc* hero) {

	std::stringstream logStream;
	zTBBox3D bBox = LevitationBean::zCModelGetBBox3D(hero->GetModel());// ->GetBBox3D();
	zVEC3 pos = hero->GetPosition();
	zMAT4* mat = &(hero->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);

	if (checkCollision(hero, *mat))
	{
		hero->SetPositionWorld(oldPos);

		//if (checkCollision(hero, *mat))
		//{
		//	logStream << "Test: stuck detected!" << std::endl;
		//	util::logInfo(&logStream);
		//	zVEC3 normal = getCollidingPolyNormal(hero, *mat);
			//hero->SetPositionWorld(oldPos + normal*20);
		//} 
	}
}

void __cdecl zCVobCheckAutoUnlinkHook(zCVob* vob) {
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (vob == hero) && Levitation::adjustHeroPosition) {
		//return;
	}
	zCVobCheckAutoUnlink(vob);
};

std::ostream & operator<<(std::ostream & os, const Plane & p)
{
	os << "(" << p.a << ", " << p.b << ", " << p.c << ", "<< 
		p.d << ")";
	return os;
}

__int32 __cdecl sysGetTime() {
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