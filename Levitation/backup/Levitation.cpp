#include <Levitation.h>
#include <HookManager.h>
#include <api/g2/zcworld.h>
#include <api/g2/zcinput.h>
#include "zCModelExtended.h"
#include <api/g2/ocgame.h>
#include <LevitationBean.h>
#include <Util.h>
#include <queue>
#include <Util_Constants.h>
#include <zCPolygon.h>
#include <CollisionDetector.h>


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



//.text:007380E0 public: int __thiscall oCNpc::IsMovLock(void) proc near
typedef int(__thiscall* OCNpcIsMovLock)(oCNpc*);
OCNpcIsMovLock oCNpcIsMovLock = (OCNpcIsMovLock)0x007380E0;

//.text:00579170 public: void __thiscall zCModel::CalcModelBBox3DWorld(void) proc near
typedef void(__thiscall* ZCModelCalcModelBBox3DWorld)(zCModel*);
ZCModelCalcModelBBox3DWorld zCModelCalcModelBBox3DWorld = (ZCModelCalcModelBBox3DWorld)0x00579170;

//.text:00511320 public: int __thiscall zCAIPlayer::CheckEnoughSpaceMoveDir(class zVEC3 const &, int) proc near
typedef int(__thiscall* ZCAIPlayerCheckEnoughSpaceMoveDir)(void*, zVEC3 const &, int); ZCAIPlayerCheckEnoughSpaceMoveDir zCAIPlayerCheckEnoughSpaceMoveDir;


//.text:00602930 public: void __thiscall zCVob::SetSleeping(int) proc near
typedef void(__thiscall* ZCVobSetSleeping)(void*, int); ZCVobSetSleeping zCVobSetSleeping = (ZCVobSetSleeping)0x00602930;

//.text:0061D2D0 private: void __thiscall zCVob::UpdatePhysics(void) proc near
typedef void(__thiscall* ZCVobUpdatePhysics)(void*); ZCVobUpdatePhysics zCVobUpdatePhysics = (ZCVobUpdatePhysics)0x0061D2D0;


//.text:00699510 protected: int __thiscall oCAIHuman::PC_CheckSpecialStates(void) proc near
typedef int(__thiscall* OCAI_HumanPC_CheckSpecialStates)(void*); 
OCAI_HumanPC_CheckSpecialStates oCAI_HumanPC_CheckSpecialStates = (OCAI_HumanPC_CheckSpecialStates)0x00699510;

//.text:006B4290 protected: void __thiscall oCAniCtrl_Human::CheckSpecialStates(void) proc near
typedef int(__thiscall* OCAI_HumanCheckSpecialStates)(void*);
OCAI_HumanCheckSpecialStates oCAI_HumanCheckSpecialStates = (OCAI_HumanCheckSpecialStates)0x006B4290;

//.text:0069B9B0 protected: void __thiscall oCAIHuman::Moving(void) proc near
typedef void (__thiscall* OCAI_HumanMoving)(void*);
OCAI_HumanMoving oCAI_HumanMoving = (OCAI_HumanMoving)0x0069B9B0;

//.text:0050DC40 private: void __thiscall zCAIPlayer::CheckPhysics(void) proc near
typedef void(__thiscall* ZCAIPlayerCheckPhysics)(void*);
ZCAIPlayerCheckPhysics zCAIPlayerCheckPhysics = (ZCAIPlayerCheckPhysics)0x0050DC40;

//.text:006ADC30 public: int __thiscall oCAniCtrl_Human::IsFallen(void) proc near
typedef int(__thiscall* OCAniCtrl_HumanIsFallen)(void*);
OCAniCtrl_HumanIsFallen oCAniCtrl_HumanIsFallen = (OCAniCtrl_HumanIsFallen)0x006ADC30;

//.text:0050E6C0 private: void __thiscall zCAIPlayer::DoProceduralMovement(void) proc near
typedef void(__thiscall* ZCAIPlayerDoProceduralMovement)(void*);
ZCAIPlayerDoProceduralMovement zCAIPlayerDoProceduralMovement = (ZCAIPlayerDoProceduralMovement)0x0050E6C0;

//.text:0050E750 public: void __thiscall zCAIPlayer::Begin(class zCVob *) proc near
typedef void(__thiscall* ZCAIPlayerBegin)(void*, void*);
ZCAIPlayerBegin zCAIPlayerBegin = (ZCAIPlayerBegin)0x0050E750;

//.text:0050E8F0 public: void __thiscall zCAIPlayer::End(void) proc near
typedef void(__thiscall* ZCAIPlayerEnd)(void*);
ZCAIPlayerEnd zCAIPlayerEnd = (ZCAIPlayerEnd)0x0050E8F0;


//.text:0077D130 public: int __thiscall oCVob::GetFloorPosition(class zVEC3 &) proc near
typedef int(__thiscall* OCVobGetFloorPosition)(void*, zVEC3&);
OCVobGetFloorPosition oCVobGetFloorPosition = (OCVobGetFloorPosition)0x0077D130;


//.text:005B66D0 public: void __thiscall zCRigidBody::SetVelocity(class zVEC3 const &) proc near
typedef void(__thiscall* ZCRigidBodySetVelocity)(void*, zVEC3 const &);
ZCRigidBodySetVelocity zCRigidBodySetVelocity = (ZCRigidBodySetVelocity)0x005B66D0;

//.text:005FE960 public: class zCRigidBody * __thiscall zCVob::GetRigidBody(void) proc near
typedef void*(__thiscall* ZCVobGetRigidBody)(void* pThis);
ZCVobGetRigidBody zCVobGetRigidBody = (ZCVobGetRigidBody)0x005FE960;

//.text:005B5EB0 public: void __thiscall zCRigidBody::ApplyImpulseCM(class zVEC3 const &) proc near
typedef void(__thiscall* ZCRigidBodyApplyImpulseCM)(void*, zVEC3 const &);
ZCRigidBodyApplyImpulseCM zCRigidBodyApplyImpulseCM = (ZCRigidBodyApplyImpulseCM)0x005B5EB0;

//.text:005B5F90 public: void __thiscall zCRigidBody::CollisionImpulseStatic(class zVEC3 const &, class zVEC3 const &, float) proc near
typedef void(__thiscall* ZCRigidBodyCollisionImpulseStatic)(void*, zVEC3 const &, zVEC3 const &, float);
ZCRigidBodyCollisionImpulseStatic zCRigidBodyCollisionImpulseStatic = (ZCRigidBodyCollisionImpulseStatic)0x005B5F90;

//.text:005B6290 public: void __thiscall zCRigidBody::CollisionImpulse(class zVEC3 const &, class zVEC3 const &, float, class zCRigidBody *) proc near
typedef void(__thiscall* ZCRigidBodyCollisionImpulse)(void*, zVEC3 const &, zVEC3 const &, float, void*);
ZCRigidBodyCollisionImpulse zCRigidBodyCollisionImpulse = (ZCRigidBodyCollisionImpulse)0x005B6290;

//.text:005B60A0 public: void __thiscall zCRigidBody::CollisionImpulseCenterStatic(class zVEC3 const &, float) proc near
typedef void(__thiscall* ZCRigidBodyCollisionImpulseCenterStatic)(void*, zVEC3 const &, zVEC3 const &, float);
ZCRigidBodyCollisionImpulseCenterStatic zCRigidBodyCollisionImpulseCenterStatic = (ZCRigidBodyCollisionImpulseCenterStatic)0x005B60A0;

//.text:005B57C0 public: int __thiscall zCRigidBody::SetSliding(class zVEC3 const &) proc near
typedef int(__thiscall* ZCRigidBodySetSliding)(void*, zVEC3 const &);
ZCRigidBodySetSliding zCRigidBodySetSliding = (ZCRigidBodySetSliding)0x005B57C0;

//.text:005B5DB0 public: void __thiscall zCRigidBody::ApplyForceCM(class zVEC3 const &) proc near
typedef void(__thiscall* ZCRigidBodyApplyForceCM)(void*, zVEC3 const &);
ZCRigidBodyApplyForceCM zCRigidBodyApplyForceCM = (ZCRigidBodyApplyForceCM)0x005B5DB0;

//.text:005B5F20 public: void __thiscall zCRigidBody::ApplyImpulse(class zVEC3 const &, class zVEC3 const &) proc near
typedef void(__thiscall* ZCRigidBodyCollisionApplyImpulse)(void*, zVEC3 const &, zVEC3 const &);
ZCRigidBodyCollisionApplyImpulse zCRigidBodyCollisionApplyImpulse = (ZCRigidBodyCollisionApplyImpulse)0x005B5F20;

//.text:005B6790 public: int __thiscall zCRigidBody::ShouldSlide(class zVEC3 const &)const proc near
typedef int(__thiscall* ZCRigidBodyShouldSlide)(void*, zVEC3 const &);
ZCRigidBodyShouldSlide zCRigidBodyShouldSlide = (ZCRigidBodyShouldSlide)0x005B6790;

//.text:005B5CF0 public: void __thiscall zCRigidBody::RunSimulation(void) proc near
typedef void(__thiscall* ZCRigidBodyRunSimulation)(void*);
ZCRigidBodyRunSimulation zCRigidBodyRunSimulation = (ZCRigidBodyRunSimulation)0x005B5CF0;

//.text:005B5D20 public: void __thiscall zCRigidBody::SetInertiaTensorBlock(float, float, float) proc near
typedef void(__thiscall* ZCRigidBodySetInertiaTensorBlock)(void* pThis, float, float, float);
ZCRigidBodySetInertiaTensorBlock zCRigidBodySetInertiaTensorBlock = (ZCRigidBodySetInertiaTensorBlock)0x005B5D20;

//.text:0061BB70 public: void __thiscall zCVob::SetPositionWorld(class zVEC3 const &) proc near
typedef void(__thiscall* ZCVobSetPositionWorld)(void*, zVEC3 const &);
ZCVobSetPositionWorld zCVobSetPositionWorld = (ZCVobSetPositionWorld)0x0061BB70;

//.text:0061B2E0 public: void __thiscall zCVob::Move(float, float, float) proc near
typedef void(__thiscall* ZCVobMove)(void* pThis, float, float, float);
ZCVobMove zCVobMove = (ZCVobMove)0x0061B2E0;

//.text:0061B350 public: void __thiscall zCVob::MoveWorld(float, float, float) proc near
typedef void(__thiscall* ZCVobMoveWorld)(void* pThis, float, float, float);
ZCVobMoveWorld zCVobMoveWorld = (ZCVobMoveWorld)0x0061B350;

//.text:0061B3C0 public: void __thiscall zCVob::MoveLocal(float, float, float) proc near
typedef void(__thiscall* ZCVobMoveLocal)(void* pThis, float, float, float);
ZCVobMoveLocal zCVobMoveLocal = (ZCVobMoveLocal)0x0061B3C0;

//.text:0061EAD0 private: void __fastcall zCVob::CollectCollisionContext_Static(struct zTBBox3D const &, class zCArray<class zCCollisionObject *> &) proc near
typedef void(__fastcall* ZCVobCollectCollisionContext_Static)(void* pThis, zTBBox3D const&, zCArray<void*>& collisionArray);
ZCVobCollectCollisionContext_Static zCVobCollectCollisionContext_Static = (ZCVobCollectCollisionContext_Static)0x0061EAD0;

//.text:0061E870 private: void __fastcall zCVob::CollectCollisionContext_Vobs(struct zTBBox3D const &, struct zCVob::zTCollisionContext &) proc near
typedef void(__fastcall* ZCVobCollectCollisionContext_Vobs)(void* pThis, zTBBox3D const&, void* collisionContext);
ZCVobCollectCollisionContext_Vobs zCVobCollectCollisionContext_Vobs = (ZCVobCollectCollisionContext_Vobs)0x0061E870;

//.text:0061EEB0 private: void __fastcall zCVob::NotifyCollisionPair(class zCCollisionReport *, class zCArray<class zCCollisionReport *> const &, int) proc near
typedef void(__fastcall* ZCVobNotifyCollisionPair)(void* pThis, void* report, void* collisionArray, int);
ZCVobNotifyCollisionPair zCVobNotifyCollisionPair = (ZCVobNotifyCollisionPair)0x0061EEB0;

//.text:0061EF80 private: void __fastcall zCVob::ExecuteCollisionResponse(class zCArray<class zCCollisionReport *> const &, int &) proc near
typedef void(__fastcall* ZCVobExecuteCollisionResponse)(void* pThis, void* report, int&);
ZCVobExecuteCollisionResponse zCVobExecuteCollisionResponse = (ZCVobExecuteCollisionResponse)0x0061EF80;

//.text:0054D400 public: void __fastcall zCCollisionObject::CalcIntermediateTrafo(float, class zMAT4 &) proc near
typedef void(__fastcall* ZCCollisionObjectCalcIntermediateTrafo)(void* pThis, float, zMAT4&);
ZCCollisionObjectCalcIntermediateTrafo zCCollisionObjectCalcIntermediateTrafo = (ZCCollisionObjectCalcIntermediateTrafo)0x0054D400;

//.text:0061DC30 private: void __fastcall zCVob::UpdateTouchList(class zCArray<class zCVob *> const &) proc near
typedef void(__fastcall* ZCVobUpdateTouchList)(void* pThis, void*);
ZCVobUpdateTouchList zCVobUpdateTouchList = (ZCVobUpdateTouchList)0x0061DC30;

//.text:0054CE50 public: virtual void __fastcall zCCollisionDetector::DetectCollisions(class zCCollisionObject *, class zCArray<class zCCollisionObject *> const &, class zCArray<class zCCollisionReport *> &) proc near
typedef void(__fastcall* ZCCollisionObjectDetectCollisions)(void* pThis, void* collisionObject, void* collisionArray, void* reports);
ZCCollisionObjectDetectCollisions zCCollisionObjectDetectCollisions = (ZCCollisionObjectDetectCollisions)0x0054CE50;

//.text:0054CAA0 private: void __fastcall zCCollisionDetector::DoStaticIntersectionTests(class zCCollisionObject *, class zCArray<class zCCollisionObject *> &, class zCArray<class zCCollisionReport *> &) proc near
typedef void(__fastcall* ZCCollisionObjectDoStaticIntersectionTests)(void* pThis, void* collisionObject, void* collisionArray, void* reports);
ZCCollisionObjectDoStaticIntersectionTests zCCollisionObjectDoStaticIntersectionTests = (ZCCollisionObjectDoStaticIntersectionTests)0x0054CAA0;

//.text:00553230 public: virtual void __fastcall zCCollObjectCharacter::DetectCollisionsSelf(class zCArray<class zCCollisionObject *> const &, class zCArray<class zCCollisionReport *> &) proc near
typedef void(__fastcall* ZCCollisionObjectDetectCollisionsSelf)(void* pThis, void* collisionArray, void* reports);
ZCCollisionObjectDetectCollisionsSelf zCCollisionObjectDetectCollisionsSelf = (ZCCollisionObjectDetectCollisionsSelf)0x00553230;

//.text:00550E20 private: void __fastcall zCCollObjectCharacter::FindFloorWaterCeiling(class zVEC3 const &, struct zCCollObjectCharacter::zTSpatialState &) proc near
typedef void(__fastcall* ZCCollisionCharacterFindFloorWaterCeiling)(void* pThis, zVEC3 const &, void* spatialState);
ZCCollisionCharacterFindFloorWaterCeiling zCCollisionCharacterFindFloorWaterCeiling = (ZCCollisionCharacterFindFloorWaterCeiling)0x00550E20;

//.text:00551150 private: int __fastcall zCCollObjectCharacter::TestHardCollisions(class zVEC3 const &, class zVEC3 const &, class zVEC3 &, struct zCCollObjectCharacter::zTSpatialState &, class zVEC3 &) proc near
typedef int(__fastcall* ZCCollisionCharacterTestHardCollisions)(void* pThis, zVEC3 const & first, zVEC3 const & second, zVEC3& third, void* spatialState, zVEC3 & fifth);
ZCCollisionCharacterTestHardCollisions zCCollisionCharacterTestHardCollisions = (ZCCollisionCharacterTestHardCollisions)0x00551150;

//.text:00550870 private: int __fastcall zCCollObjectCharacter::CalcSlideVector(class zVEC3 const &, class zVEC3 const &, class zVEC3 &, class zVEC3 &, float &) proc near
typedef int(__fastcall* ZCCollisionCharacterCalcSlideVector)(void* pThis, zVEC3 const & first, zVEC3 const & second, zVEC3& third, zVEC3& fourth, float& fifth);
ZCCollisionCharacterCalcSlideVector zCCollisionCharacterCalcSlideVector = (ZCCollisionCharacterCalcSlideVector)0x00550870;

//.text:00552180 private: int __fastcall zCCollObjectCharacter::TestSoftCollisions(class zVEC3 const &, class zVEC3 const &, class zVEC3 &, class zVEC3 &) proc near
typedef int(__fastcall* ZCCollisionCharacterTestSoftCollisions)(void* pThis, zVEC3 const & first, zVEC3 const & second, zVEC3& third, zVEC3& fourth);
ZCCollisionCharacterTestSoftCollisions zCCollisionCharacterTestSoftCollisions = (ZCCollisionCharacterTestSoftCollisions)0x00552180;




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

	HookManager* hookManager = HookManager::getHookManager();
	hookManager->addFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobSetPhysicsEnabled, zCVobSetPhysicsEnabledHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGamePause, oCGamePauseHook, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&oCGameUnpause, oCGameUnpauseHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&doSurfaceAlignment, DoSurfaceAlignmentHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobCheckAndResolveCollisions, zCVobCheckAndResolveCollisionsHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobCollectCollisionContext_Static, zCVobCollectCollisionContext_StaticHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobCollectCollisionContext_Vobs, zCVobCollectCollisionContext_VobsHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobNotifyCollisionPair, zCVobNotifyCollisionPairHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobExecuteCollisionResponse, zCVobExecuteCollisionResponseHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCCollisionObjectCalcIntermediateTrafo, zCCollisionObjectCalcIntermediateTrafoHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobUpdateTouchList, zCVobUpdateTouchListHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCCollisionObjectDetectCollisions, zCCollisionObjectDetectCollisionsHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCCollisionObjectDoStaticIntersectionTests, zCCollisionObjectDoStaticIntersectionTestsHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCCollisionObjectDetectCollisionsSelf, zCCollisionObjectDetectCollisionsSelfHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCCollisionCharacterFindFloorWaterCeiling, zCCollisionCharacterFindFloorWaterCeilingHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCCollisionCharacterTestHardCollisions, zCCollisionCharacterTestHardCollisionsHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCCollisionCharacterCalcSlideVector, zCCollisionCharacterCalcSlideVectorHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCCollisionCharacterTestSoftCollisions, zCCollisionCharacterTestSoftCollisionsHook, moduleDesc);

	//hookManager->addFunctionHook((LPVOID*)&zCTriggerOnTouch, zCTriggerOnTouchHook, moduleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobHasEnoughSpace, zCVobHasEnoughSpaceHook, moduleDesc);

	//hookManager->addFunctionHook((LPVOID*)&zCAIPlayerCheckEnoughSpaceMoveDir, zCAIPlayerCheckEnoughSpaceMoveDirHook, moduleDesc);

	/*hookManager->addFunctionHook((LPVOID*)&zCAIPlayerCheckFloorSliding, zCAIPlayerCheckFloorSlidingHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobUpdatePhysics, zCVobUpdatePhysicsHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCAI_HumanPC_CheckSpecialStates, oCAI_HumanPC_CheckSpecialStatesHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCAI_HumanMoving, oCAI_HumanMovingHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCAIPlayerCheckPhysics, zCAIPlayerCheckPhysicsHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCAI_HumanCheckSpecialStates, oCAI_HumanCheckSpecialStatesHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCAniCtrl_HumanIsFallen, oCAniCtrl_HumanIsFallenHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCAIPlayerDoProceduralMovement, zCAIPlayerDoProceduralMovementHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCAIPlayerBegin, zCAIPlayerBeginHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCAIPlayerEnd, zCAIPlayerEndHook, moduleDesc);*/


	/*hookManager->addFunctionHook((LPVOID*)&zCRigidBodySetVelocity, zCRigidBodySetVelocityHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodyApplyImpulseCM, zCRigidBodyApplyImpulseCMHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodyCollisionImpulseStatic, zCRigidBodyCollisionImpulseStaticHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodyCollisionImpulse, zCRigidBodyCollisionImpulseHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodyCollisionImpulseCenterStatic, zCRigidBodyCollisionImpulseCenterStaticHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodySetSliding, zCRigidBodySetSlidingHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodyApplyForceCM, zCRigidBodyApplyForceCMHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodyCollisionApplyImpulse, zCRigidBodyCollisionApplyImpulseHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodyShouldSlide, zCRigidBodyShouldSlideHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodyRunSimulation, zCRigidBodyRunSimulationHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCRigidBodySetInertiaTensorBlock, zCRigidBodySetInertiaTensorBlockHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobSetPositionWorld, zCVobSetPositionWorldHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobMoveWorld, zCVobMoveWorldHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobMoveLocal, zCVobMoveLocalHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobMove, zCVobMoveHook, moduleDesc);*/
}

void Levitation::unHookModule()
{
}

void  Levitation::zCVobMoveWorldHook(void* pThis, float first, float second, float third)
{
	if (adjustHeroPosition && !noCollision)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCVobMoveWorld(pThis, first, second, third);
}

void  Levitation::zCVobMoveLocalHook(void* pThis, float first, float second, float third)
{
	if (adjustHeroPosition && !noCollision)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCVobMoveLocal(pThis, first, second, third);
}

void Levitation::zCVobCollectCollisionContext_StaticHook(void * pThis, zTBBox3D const & bbox, zCArray<void*>& collisionArray)
{
	if (adjustHeroPosition)
	{
		logStream << "zCVobCollectCollisionContext_StaticHook called!" << std::endl;
		util::logAlways(&logStream);
		return;
	}
	zCVobCollectCollisionContext_Static(pThis, bbox, collisionArray);
}

void Levitation::zCVobCollectCollisionContext_VobsHook(void * pThis, zTBBox3D const & bbox, void * collisionContext)
{
	if (adjustHeroPosition)
	{
		return;
	}
	zCVobCollectCollisionContext_Vobs(pThis, bbox, collisionContext);
}

void Levitation::zCVobNotifyCollisionPairHook(void * pThis, void * report, void * collisionArray, int third)
{
	if (adjustHeroPosition)
	{
		return;
	}
	zCVobNotifyCollisionPair(pThis, report, collisionArray, third);
}

void Levitation::zCVobExecuteCollisionResponseHook(void* pThis, void * report, int & second)
{
	if (adjustHeroPosition)
	{
		return;
	}
	zCVobExecuteCollisionResponse(pThis, report, second);
}

void Levitation::zCCollisionObjectCalcIntermediateTrafoHook(void * pThis, float second, zMAT4 & mat)
{
	if (adjustHeroPosition)
	{
		return;
	}
	zCCollisionObjectCalcIntermediateTrafo(pThis, second, mat);
}

void Levitation::zCVobUpdateTouchListHook(void* pThis, void * vobArray)
{
	if (adjustHeroPosition)
	{
		return;
	}
	zCVobUpdateTouchList(pThis, vobArray);
}

void Levitation::zCCollisionObjectDetectCollisionsHook(void * pThis, void * collisionObject, void * collisionArray, void * reports)
{
	if (adjustHeroPosition)
	{
		return;
	}
	zCCollisionObjectDetectCollisions(pThis, collisionObject, collisionArray, reports);
}

void Levitation::zCCollisionObjectDoStaticIntersectionTestsHook(void * pThis, void * collisionObject, void * collisionArray, void * reports)
{
	if (adjustHeroPosition)
	{
		return;
	}
	zCCollisionObjectDoStaticIntersectionTests(pThis, collisionObject, collisionArray, reports);
}

void Levitation::zCCollisionObjectDetectCollisionsSelfHook(void * pThis, void * collisionArray, void * reports)
{
	if (adjustHeroPosition)
	{
		//return;
	}
	zCCollisionObjectDetectCollisionsSelf(pThis, collisionArray, reports);
}

void Levitation::zCCollisionCharacterFindFloorWaterCeilingHook(void * pThis, zVEC3 const & vec, void * spatialState)
{
	if (adjustHeroPosition)
	{
		//return;
	}
	//logStream << "zCCollisionCharacterFindFloorWaterCeilingHook: spatialState= " << spatialState << std::endl;
	//util::logAlways(&logStream);
	zCCollisionCharacterFindFloorWaterCeiling(pThis, vec, spatialState);
}

int Levitation::zCCollisionCharacterTestHardCollisionsHook(void * pThis, zVEC3 const & currentPos, zVEC3 const & targetPos, zVEC3 & resultPos, void * spatialState, zVEC3 & fifth)
{
	if (adjustHeroPosition)
	{
		//return;
		int result = zCCollisionCharacterTestHardCollisions(pThis, currentPos, targetPos, resultPos, spatialState, fifth);
		resultPos.y = targetPos.y;
		return result;
	}
	/*logStream << "zCCollisionCharacterTestHardCollisionsHook: currentPos= " << currentPos << std::endl;
	logStream << "zCCollisionCharacterTestHardCollisionsHook: targetPos= " << targetPos << std::endl;
	logStream << "zCCollisionCharacterTestHardCollisionsHook: resultPos= " << resultPos << std::endl;
	logStream << "zCCollisionCharacterTestHardCollisionsHook: spatialState= " << spatialState << std::endl;
	logStream << "zCCollisionCharacterTestHardCollisionsHook: fifth= " << fifth << std::endl;
	util::logAlways(&logStream);*/
	return zCCollisionCharacterTestHardCollisions(pThis, currentPos, targetPos, resultPos, spatialState, fifth);
	//resultPos = targetPos;
	/*logStream << "zCCollisionCharacterTestHardCollisionsHook: resultPos after= " << resultPos << std::endl;
	logStream << "zCCollisionCharacterTestHardCollisionsHook: fifth after= " << fifth << std::endl;
	util::logAlways(&logStream);*/
}

int Levitation::zCCollisionCharacterCalcSlideVectorHook(void * pThis, zVEC3 const & first, zVEC3 const & second, zVEC3 & third, zVEC3 & fourth, float & fifth)
{
	if (adjustHeroPosition)
	{
		//return 0;
	}
	/*logStream << "zCCollisionCharacterCalcSlideVectorHook: first= " << first << std::endl;
	logStream << "zCCollisionCharacterCalcSlideVectorHook: second= " << second << std::endl;
	logStream << "zCCollisionCharacterCalcSlideVectorHook: third= " << third << std::endl;
	logStream << "zCCollisionCharacterCalcSlideVectorHook: fourth before= " << fourth << std::endl;
	logStream << "zCCollisionCharacterCalcSlideVectorHook: fifth before= " << fifth << std::endl;*/

	int result = zCCollisionCharacterCalcSlideVector(pThis, first, second, third, fourth, fifth);

	/*logStream << "zCCollisionCharacterCalcSlideVectorHook: fourth after= " << fourth << std::endl;
	logStream << "zCCollisionCharacterCalcSlideVectorHook: fifth after= " << fifth << std::endl;
	util::logAlways(&logStream);*/

	return result;
}

int Levitation::zCCollisionCharacterTestSoftCollisionsHook(void * pThis, zVEC3 const & first, zVEC3 const & second, zVEC3 & third, zVEC3 & fourth)
{
	if (adjustHeroPosition)
	{
		//return 0;
	}
	return zCCollisionCharacterTestSoftCollisions(pThis, first, second, third, fourth);
}

void  Levitation::zCVobMoveHook(void* pThis, float first, float second, float third)
{
	if (adjustHeroPosition && !noCollision)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCVobMove(pThis, first, second, third);
}

void Levitation::zCVobSetPositionWorldHook(void* pThis, zVEC3 const& vec)
{
	if (adjustHeroPosition && !noCollision)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		//return;
	}
	zCVobSetPositionWorld(pThis, vec);
}

void  Levitation::zCRigidBodySetInertiaTensorBlockHook(void* pThis, float first, float second, float third)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
   zCRigidBodySetInertiaTensorBlockHook(pThis, first, second, third);
}

void Levitation::zCRigidBodyRunSimulationHook(void* pThis)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	return zCRigidBodyRunSimulation(pThis);
}

int Levitation::zCRigidBodyShouldSlideHook(void* pThis, zVEC3 const& vec)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return 0;
	}
	return zCRigidBodyShouldSlide(pThis, vec);
}

void  Levitation::zCRigidBodyCollisionApplyImpulseHook(void* pThis, zVEC3 const& vec, zVEC3 const & vec2)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCRigidBodyCollisionApplyImpulse(pThis, vec, vec2);
}

void  Levitation::zCRigidBodyApplyForceCMHook(void* pThis, zVEC3 const& vec)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCRigidBodyApplyForceCM(pThis, vec);
}

int Levitation::zCRigidBodySetSlidingHook(void* pThis, zVEC3 const& vec)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return 0;
	}
	return zCRigidBodySetSliding(pThis, vec);
}

void  Levitation::zCRigidBodyCollisionImpulseCenterStaticHook(void* pThis, zVEC3 const& vec, zVEC3 const & vec2, float third)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCRigidBodyCollisionImpulseCenterStatic(pThis, vec, vec2, third);
};

void Levitation::zCRigidBodyCollisionImpulseHook(void* pThis, zVEC3 const& vec, zVEC3 const & vec2, float third, void* fourth)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCRigidBodyCollisionImpulse(pThis, vec, vec2, third, fourth);
};

void Levitation::zCRigidBodyCollisionImpulseStaticHook(void* pThis, zVEC3 const& vec, zVEC3 const & vec2, float third)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCRigidBodyCollisionImpulseStatic(pThis, vec, vec2, third);
};

void Levitation::zCRigidBodyApplyImpulseCMHook(void* pThis, zVEC3 const& vec)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodyApplyImpulseCM(pThis, zVEC3(0, 0, 0));
		return;
	}
	zCRigidBodyApplyImpulseCM(pThis, vec);
};

void Levitation::zCRigidBodySetVelocityHook(void* pThis, zVEC3 const& vec)
{
	void* heroRigid = zCVobGetRigidBody(oCNpc::GetHero());
	if (adjustHeroPosition)
	{
		//zCRigidBodySetVelocity(pThis, zVEC3(0,0,0));
		return;
	}
	zCRigidBodySetVelocity(pThis, vec);
};


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
	if (adjust) {
		oldYPosition = hero->GetPosition().y;
		zMAT4* mat = &hero->trafoObjToWorld;
		oldLook = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		heroLevitationBean->oldLook = oldLook;
		heroLevitationBean->oldXPos = mat->m[0][3];
		heroLevitationBean->oldYPos = mat->m[1][3];
		heroLevitationBean->oldZPos = mat->m[2][3];
	}

	zCVobDoFrameActivity(pThis);

	if (pThis == hero)
	{
		frameTimePast = calcPastFrameTime();
	}
	else
	{
		//zCVobDoFrameActivity(pThis);
	}
	if (adjust) {
		noCollision = false;

		//hero->ResetXZRotationsWorld();
		zCVobSetCollDetHook(hero, 1);
		zCVobSetPhysicsEnabled(hero, false);
		noCollision = true;

		zVEC3 pos = hero->GetPosition();
		if (pos.y < oldYPosition) {
			hero->SetPositionWorld(zVEC3(pos.x, oldYPosition, pos.z));
		}

		float hoverDistance = heroLevitationBean->getHoverDistance();
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
		}
		levitate();
		noCollision = false;
		//zCVobSetCollDet(hero, 1);
		//zCVobSetPhysicsEnabled(hero, true);
		Test(hero);
	}
}

void Levitation::backup_zCVobDoFrameActivityHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	if (!heroLevitationBeanCalled) {
		heroLevitationBean = new LevitationBean(hero);
		heroLevitationBeanCalled = true;
	}
	bool adjust = (hero == pThis) && adjustHeroPosition;
	float oldYPosition = 0;
	zVEC3 oldLook;
	if (adjust) {
		oldYPosition = hero->GetPosition().y;
		zMAT4* mat = &hero->trafoObjToWorld;
		oldLook = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		heroLevitationBean->oldLook = oldLook;
		heroLevitationBean->oldXPos = mat->m[0][3];
		heroLevitationBean->oldYPos = mat->m[1][3];
		heroLevitationBean->oldZPos = mat->m[2][3];
	}

	zCVobDoFrameActivity(pThis);

	if (pThis == hero)
	{
		frameTimePast = calcPastFrameTime();
	}
	else
	{
		//zCVobDoFrameActivity(pThis);
	}
	if (adjust) {
		noCollision = false;
		zCVobSetCollDetHook(hero, 1);

		//hero->ResetXZRotationsWorld();
		zCVobSetPhysicsEnabled(hero, false);
		noCollision = true;

		zVEC3 pos = hero->GetPosition();
		if (pos.y < oldYPosition) {
			hero->SetPositionWorld(zVEC3(pos.x, oldYPosition, pos.z));
		}

		float hoverDistance = heroLevitationBean->getHoverDistance();
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
		}
		levitate();
		noCollision = false;
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
	if (hero != nullptr && (pThis == hero) && adjustHeroPosition && noCollision) {
		return zCVobSetCollDet(pThis, 0); //TODO very important for up lifting!!!
		//hero->SetCollDetDyn(1 && second);
		//hero->SetCollDetStat(0);
		//return;
	}
	zCVobSetCollDet(pThis, second);
}

void Levitation::oCAI_HumanMovingHook(void * pThis)
{
	if (adjustHeroPosition)
	{
		return;
	}

	oCAI_HumanMoving(pThis);
}

void Levitation::zCAIPlayerCheckPhysicsHook(void * pThis)
{
	if (adjustHeroPosition) return;
	zCAIPlayerCheckPhysics(pThis);
}

void Levitation::zCAIPlayerBeginHook(void * pThis, void* vob)
{
	if (adjustHeroPosition) return;
	zCAIPlayerBegin(pThis, vob);
}

void Levitation::zCAIPlayerEndHook(void * pThis)
{
	if (adjustHeroPosition) return;
	zCAIPlayerEnd(pThis);
}

void Levitation::oCAI_HumanCheckSpecialStatesHook(void * pThis)
{
	if (adjustHeroPosition) return;
	oCAI_HumanCheckSpecialStates(pThis);
}

int Levitation::zCAIPlayerCheckFloorSlidingHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	//oCAI_AniCtrl: offset 0x97C
	// (pThis == (void*)((BYTE*)hero + 0x97C)) &&
	if (hero != NULL && (pThis == (void*)((BYTE*)hero + 0x97C)) && adjustHeroPosition) {
		logStream << "Levitation::zCAIPlayerCheckFloorSlidingHook: called!" << std::endl;
		util::logAlways(&logStream);
		return 0;
	}
	return zCAIPlayerCheckFloorSliding(pThis);
}

int Levitation::oCAniCtrl_HumanIsFallenHook(void * pThis)
{
	if (adjustHeroPosition) return false;
	return oCAniCtrl_HumanIsFallen(pThis);
}

void Levitation::zCAIPlayerDoProceduralMovementHook(void * pThis)
{
	if (adjustHeroPosition) return;
	zCAIPlayerDoProceduralMovement(pThis);
}

int Levitation::oCAI_HumanPC_CheckSpecialStatesHook(void * pThis)
{
	if (adjustHeroPosition)
	{
		logStream << "Levitation::oCAI_HumanPC_CheckSpecialStatesHook: called!" << std::endl;
		util::logAlways(&logStream);
		return 0;
	}
	return oCAI_HumanPC_CheckSpecialStates(pThis);
}

void Levitation::DoSurfaceAlignmentHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = /*(hero == pThis) &&*/ Levitation::adjustHeroPosition;
	if (adjust) {
		return; //TODO
	}
	return doSurfaceAlignment(pThis);
}

void Levitation::zCVobCheckAndResolveCollisionsHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (hero != NULL) && (hero == pThis) && Levitation::adjustHeroPosition;
	if (adjust) {
		//if (Levitation::noCollision && !DynItemInst::loadGame && !HookManager::changeLevel) return;
		return; //TODO
	}
	zCVobCheckAndResolveCollisions(pThis);
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

int Levitation::zCVobIsColliding(void* pThis)
{
	XCALL(ZCVOB_IS_COLLIDING);
}

void Levitation::zCVobUpdatePhysicsHook(void * pThis)
{
	if (adjustHeroPosition && pThis == oCNpc::GetHero())
	{
		return;
	}

	zCVobUpdatePhysics(pThis);
}


int Levitation::zCAIPlayerCheckEnoughSpaceMoveDirHook(void* pThis, zVEC3 const& vec, int second)
{
	int result = zCAIPlayerCheckEnoughSpaceMoveDir(pThis, vec, second);
	oCNpc* hero = oCNpc::GetHero();
	if (result && hero != nullptr && (pThis == hero) && adjustHeroPosition) {
		logStream << "zCAIPlayerCheckEnoughSpaceMoveDirHook: called!"  << std::endl;
		util::logInfo(&logStream);
		//return 0;
	}

	return result;
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

void levitate() {
	if (!Levitation::adjustHeroPosition) return;
	if (Levitation::pausedGame) return;

	std::stringstream logStream;

	oCNpc* hero = oCNpc::GetHero();
	if (oCNpcIsMovLock(hero)) return;

	float angle = 0;
	zVEC3 positionAdd(0, 0, 0);

	Levitation::noCollision = true;
	float speed = 100.0f; //100cm per second
	float distance = speed * float(frameTimePast) / 1000;
	zVEC3 pos = hero->GetPositionWorld();
	zVEC3 oldLook = heroLevitationBean->oldLook;
	zCInput* input = zCInput::GetInput();

	zCModelCalcModelBBox3DWorld(hero->GetModel());// ->CalcModelBBox3DWorld();
	zTBBox3D bBox = constants::zCModelGetBBox3D(hero->GetModel());// ->GetBBox3D();
	zMAT4* mat = &(hero->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;
	float yDownExt = bBox.bbox3D_mins.y;

	hero->ResetXZRotationsLocal();
	int arrowLeft = input->KeyPressed(203);
	int arrowRight = input->KeyPressed(205);
	int arrowUp = input->KeyPressed(200);
	int moveUp = input->KeyPressed(0x1A);
	int moveDown = input->KeyPressed(0x28);

	if (!arrowLeft && !arrowRight)
	{
		//adjust Rotation;
		hero->ResetXZRotationsLocal();
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
		float collisionYUpDist = getCollideYDir(hero->GetVobPosition(), abs(yUpExt), true).distance;
		//if (collisionYUpDist > COLLISION_HEAD_SIZE_Y) {
		positionAdd.y += distance;
		//}
	}
	// Pressed � (levitation down)
	else if (moveDown) {
		//if (collisionYDownDist > abs(yDownExt / 6)) {
		positionAdd.y -= distance;
		//}
	}

	// Pressed Arrow_Up (levitation forward)
	if (input->KeyPressed(0xC8)) {
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
	constants::rotateLocalY(hero, angle);
	zVEC3 finalPosition = hero->GetPositionWorld();
	finalPosition += positionAdd;
	if (!moveDown && !moveUp)
	{
		// TODO: Floor aligning doesn't work properly on certain cases; keyword: consider poly normals!
		//doFloorAligning(&finalPosition, mat);
	}
	hero->SetPositionWorld(finalPosition);
	heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(finalPosition));
};



void backup_levitate() {
	if (!Levitation::adjustHeroPosition) return;
	if (Levitation::pausedGame) return;

	std::stringstream logStream;

	oCNpc* hero = oCNpc::GetHero();
	if (oCNpcIsMovLock(hero)) return;

	float angle = 0;
	zVEC3 positionAdd(0, 0, 0);

	Levitation::noCollision = true;
	float speed = 100.0f; //100cm per second
	float distance = speed * float(frameTimePast) / 1000;
	zVEC3 pos = hero->GetPositionWorld();
	zVEC3 oldLook = heroLevitationBean->oldLook;
	zCInput* input = zCInput::GetInput();

	zCModelCalcModelBBox3DWorld(hero->GetModel());// ->CalcModelBBox3DWorld();
	zTBBox3D bBox = constants::zCModelGetBBox3D(hero->GetModel());// ->GetBBox3D();
	zMAT4* mat = &(hero->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;
	float yDownExt = bBox.bbox3D_mins.y;

	hero->ResetXZRotationsLocal();
	int arrowLeft = input->KeyPressed(203);
	int arrowRight = input->KeyPressed(205);
	int arrowUp = input->KeyPressed(200);
	int moveUp = input->KeyPressed(0x1A);
	int moveDown = input->KeyPressed(0x28);

	if (!arrowLeft && !arrowRight)
	{
		//adjust Rotation;
		hero->ResetXZRotationsLocal();
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
		float collisionYUpDist = getCollideYDir(hero->GetVobPosition(), abs(yUpExt), true).distance;
		//if (collisionYUpDist > COLLISION_HEAD_SIZE_Y) {
		positionAdd.y += distance;
		//}
	}
	// Pressed � (levitation down)
	else if (moveDown) {
		//if (collisionYDownDist > abs(yDownExt / 6)) {
		positionAdd.y -= distance;
		//}
	}

	// Pressed Arrow_Up (levitation forward)
	if (input->KeyPressed(0xC8)) {
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
	constants::rotateLocalY(hero, angle);
	zVEC3 finalPosition = hero->GetPositionWorld();
	finalPosition += positionAdd;
	if (!moveDown && !moveUp)
	{
		// TODO: Floor aligning doesn't work properly on certain cases; keyword: consider poly normals!
		//doFloorAligning(&finalPosition, mat);
	}
	hero->SetPositionWorld(finalPosition);
	heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(finalPosition));
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
	LevitationBean::zCWorldTraceRayNearestHit(world, pos, dir, nullptr, flags);
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
	LevitationBean::zCWorldTraceRayNearestHit(world, pos, posOutside, nullptr, flags);
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


void Test(oCNpc* hero) {

	std::stringstream logStream;
	zTBBox3D bBox = constants::zCModelGetBBox3D(hero->GetModel());// ->GetBBox3D();
	zVEC3 pos = hero->GetPosition();
	zMAT4* mat = &(hero->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;
	//writeToConsole("diff: ", diff);
	//Motion collideUp = getCollideYDir(hero, abs(2*yDownExt/3), false);
	Motion collideDown = getCollideYDir(hero->GetVobPosition(), 0, false);
	zVEC3 iSect = collideDown.intersect;
	zVEC3 objPos = collideDown.objectPos;
	float yIntersect = objPos.y - iSect.y; // always > 0
	zCInput* input = zCInput::GetInput();
	bool upKeyPressed = input->KeyPressed(0x1A); // ü = 0x1A
	bool downKeyPressed = input->KeyPressed(0x28); // ä = 0x1A
	heroLevitationBean->setMoveDown(false);
	float distanceToGround = heroLevitationBean->getDistanceToGround(pos);
	heroLevitationBean->update(distanceToGround);
	if (yIntersect < heroLevitationBean->HOVER_DISTANCE && !upKeyPressed) {
		//TODO : check status of previously initialized LevitationBean!
		float hoverDistance = heroLevitationBean->getHoverDistance();
		if (distanceToGround < hoverDistance)
		{
		}
	}
	else if (yIntersect < 120 && !upKeyPressed) {
		//heroLevitationBean->setMoveDown(true);
	}

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);
	if (CollisionDetector::detectCollision(hero, *mat))
	{
		pos = zVEC3(mat->m[0][3], mat->m[1][3], mat->m[2][3]);
		hero->SetPositionWorld(oldPos);
		pos = hero->GetPosition();
		heroLevitationBean->oldXPos = pos.x;
		heroLevitationBean->oldYPos = pos.y;
		heroLevitationBean->oldZPos = pos.z;

		if (CollisionDetector::detectCollision(hero, *mat))
		{
			logStream << "Test: stuck detected!" << std::endl;
			util::logInfo(&logStream);
			zVEC3 normal = CollisionDetector::getCollidingPolyNormal(hero, *mat);
			//hero->SetPositionWorld(oldPos + normal*20);
		} 
		pos = hero->GetPosition();
		heroLevitationBean->oldXPos = pos.x;
		heroLevitationBean->oldYPos = pos.y;
		heroLevitationBean->oldZPos = pos.z;
	}

	// update old position and old distance to ground of the heroLevitationBean
	heroLevitationBean->setOldPosition(hero->GetPosition());
	heroLevitationBean->setOldDistanceToGround(heroLevitationBean->getDistanceToGround(hero->GetPosition()));

};

void __cdecl zCVobCheckAutoUnlinkHook(zCVob* vob) {
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (vob == hero) && Levitation::adjustHeroPosition) {
		//return;
	}
	zCVobCheckAutoUnlink(vob);
};

__int32 __cdecl sysGetTime() {
	XCALL(0x00505280);
};

int __thiscall zCMaterial::IsPortalMaterial(void* pThis)
{
	XCALL(0x005648D0);
}
