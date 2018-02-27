#ifndef __LEVITATION_H
#define __LEVITATION_H

#include <Module.h>
#include <api/g2/ztypes.h>
#include "api/g2/ocnpc.h"
#include "Util_Constants.h"

#define ZCVOB_DO_FRAME_ACTIVITY_ADDRESS 0x00602C60
#define ZCVOB_SET_PHYSICS_ENABLED_ADDRESS 0x0061D190
#define ZCVOB_CHECK_AUTO_UNLINK_ADDRESS 0x0061AAC0
#define OCGAME_PAUSE_ADDRESS 0x006C8AD0
#define OCGAME_UNPAUSE_ADDRESS 0x006C8D50
#define ZCVOB_SET_COLL_DET_ADDRESS 0x006D0000		
#define ZCAIPLAYER_CHECK_FLOOR_SLIDING_ADDRESS 0x0050D4D0
#define DO_SURFACE_ALIGNMENT_ADDRESS 0x0050CE80 //0x0050D4D0//Original: 0x0050CE80
#define ZCVOB_CHECK_AND_RESOLVE_COLLISION_ADDRESS  0x0061F060//0x0061D190//0x0061E0D0 //Test: 0x0061F060
#define ZCTRIGGER_ON_TOUCH_ADDRESS 0x00610640
#define ZCVOB_HAS_ENOUGH_SPACE_ADDRESS 0x0077C6B0
#define ZCVOB_GET_VELOCITY_ADDRESS 0x0061D1C0
#define ZCVOB_IS_COLLIDING 0x0061F4F0


namespace LevitationUtil
{
	//.text:00490EA0 public: class zVEC3 & __thiscall zVEC3::Normalize(void) proc near
	typedef int(__thiscall* ZVEC3Normalize)(zVEC3*); 
	extern ZVEC3Normalize zVEC3Normalize;

	//.text:006824D0 public: virtual void __thiscall oCNpc::ResetPos(class zVEC3 &) proc near
	typedef void(__thiscall* OCNpcResetPos)(oCNpc*, zVEC3 &);
	extern OCNpcResetPos oCNpcResetPos;

	//.text:0061E0D0 public: virtual void __thiscall zCVob::EndMovement(int) proc near
	typedef void(__thiscall* ZCVobEndMovement)(zCVob*, int);
	extern ZCVobEndMovement zCVobEndMovement;

	//0061B720
	typedef void(__thiscall* RotateLocalY)(void*, float);
	extern RotateLocalY rotateLocalY;


	inline float getDistanceOfVectors(const zVEC3 & vec1, const zVEC3 & vec2) {
		float distance = abs(vec2.x - vec1.x + vec2.y - vec1.y + vec2.z - vec1.z);
		if (constants::areEqual(distance, 0)) {
			return 0;
		}
		return distance;
	}

	inline bool zVEC3IsNull(const zVEC3 & vec)
	{
		if (constants::areEqual(vec.x, 0)
			&& constants::areEqual(vec.y, 0)
			&& constants::areEqual(vec.z, 0))
		{
			return true;
		}
		return false;
	}

};


extern int COLLISION_HEAD_SIZE_Y;

class Levitation : public Module
{
public:

	static bool adjustHeroPosition;
	static bool pausedGame;
	static bool noCollision;
	static int frameTime;
	static int diffFrameTime;
	static float yPos; //not really used!!!

	Levitation();
	virtual ~Levitation();
	virtual void hookModule() override;
	virtual void unHookModule() override;

	static void __thiscall zCVobDoFrameActivityHook(void* pThis);
	static void backup_zCVobDoFrameActivityHook(void* pThis);
	static void __thiscall zCVobSetPhysicsEnabledHook(void* pThis, int second);
	static void __thiscall oCGamePauseHook(void* pThis, int second);
	static void __thiscall oCGameUnpauseHook(void* pThis);
	static void __thiscall zCVobSetCollDetHook(void* pThis, int second); //not used!!!
	static void __thiscall oCAI_HumanMovingHook(void* pThis); //not used!!!
	static void __thiscall zCAIPlayerCheckPhysicsHook(void* pThis); //not used!!!
	static void __thiscall zCAIPlayerBeginHook(void* pThis, void*); //not used!!!
	static void __thiscall zCAIPlayerEndHook(void* pThis); //not used!!!
	static void __thiscall oCAI_HumanCheckSpecialStatesHook(void* pThis); //not really used!!!
	static int __thiscall zCAIPlayerCheckFloorSlidingHook(void* pThis); //not used!!!
	static int __thiscall oCAniCtrl_HumanIsFallenHook(void* pThis); //not used!!!
	static void __thiscall zCAIPlayerDoProceduralMovementHook(void* pThis); //not really used!!!

	static int __thiscall oCAI_HumanPC_CheckSpecialStatesHook(void* pThis); //not used!!!
	static void __thiscall DoSurfaceAlignmentHook(void* pThis); //not really used!!!
	static void __thiscall zCVobCheckAndResolveCollisionsHook(void* pThis);
	static void __thiscall zCTriggerOnTouchHook(void* pThis, zCVob*); //not really(?) used!!!
	static int __thiscall zCVobHasEnoughSpaceHook(void* pThis, zVEC3&);  //not really(?) used!!!
	static zVEC3 __thiscall zCVobGetVelocityHook(void* pThis); //not really used!!!

															   //non hooks
	static int __thiscall zCVobIsColliding(void* pThis);
	static void __thiscall zCVobUpdatePhysicsHook(void* pThis);

	//.text:00511320 public: int __thiscall zCAIPlayer::CheckEnoughSpaceMoveDir(class zVEC3 const &, int) proc near
	static int __thiscall zCAIPlayerCheckEnoughSpaceMoveDirHook(void* pThis, zVEC3 const &, int);

	//zCRigidBody-stuff
	static void __thiscall zCRigidBodySetVelocityHook(void* pThis, zVEC3 const& vec);
	static void __thiscall zCRigidBodyApplyImpulseCMHook(void* pThis, zVEC3 const& vec);

	static void __thiscall zCRigidBodyCollisionImpulseStaticHook(void* pThis, zVEC3 const& vec, zVEC3 const & vec2, float third);
	static void __thiscall zCRigidBodyCollisionImpulseCenterStaticHook(void* pThis, zVEC3 const& vec, zVEC3 const & vec2, float third);
	static void __thiscall zCRigidBodyCollisionImpulseHook(void* pThis, zVEC3 const& vec, zVEC3 const & vec2, float third, void* fourth);

	static int __thiscall zCRigidBodySetSlidingHook(void* pThis, zVEC3 const& vec);
	static void __thiscall zCRigidBodyApplyForceCMHook(void* pThis, zVEC3 const& vec);
	static void __thiscall zCRigidBodyCollisionApplyImpulseHook(void* pThis, zVEC3 const& vec, zVEC3 const & vec2);
	static int __thiscall zCRigidBodyShouldSlideHook(void* pThis, zVEC3 const& vec);
	static void __thiscall zCRigidBodyRunSimulationHook(void* pThis);
	static void __thiscall zCRigidBodySetInertiaTensorBlockHook(void* pThis, float first, float second, float third);
	static void __thiscall zCVobSetPositionWorldHook(void* pThis, zVEC3 const& vec);

	static void __thiscall zCVobMoveHook(void* pThis, float first, float second, float third);
	static void __thiscall zCVobMoveWorldHook(void* pThis, float first, float second, float third);
	static void __thiscall zCVobMoveLocalHook(void* pThis, float first, float second, float third);
	static void __fastcall zCVobCollectCollisionContext_StaticHook (void* pThis, zTBBox3D const&, zCArray<void*>& collisionArray);
	static void __fastcall zCVobCollectCollisionContext_VobsHook(void* pThis, zTBBox3D const&, void*);
	static void __fastcall zCVobNotifyCollisionPairHook (void* pThis, void* report, void* collisionArray, int);
	static void __fastcall zCVobExecuteCollisionResponseHook(void* pThis, void* report, int & second);
	static void __fastcall zCCollisionObjectCalcIntermediateTrafoHook(void* pThis, float second, zMAT4 & mat);
	static void __fastcall zCVobUpdateTouchListHook(void* pThis, void* vobArray);
	static void __fastcall zCCollisionObjectDetectCollisionsHook(void* pThis, void* collisionObject, void* collisionArray, void* reports);
	static void __fastcall zCCollisionObjectDoStaticIntersectionTestsHook(void* pThis, void* collisionObject, void* collisionArray, void* reports);
	static void __fastcall zCCollisionObjectDetectCollisionsSelfHook(void* pThis, void* collisionArray, void* reports);
	static void __fastcall zCCollisionCharacterFindFloorWaterCeilingHook (void* pThis, zVEC3 const &, void* spatialState);
	static int __fastcall zCCollisionCharacterTestHardCollisionsHook (void* pThis, zVEC3 const & first, zVEC3 const & second, zVEC3& third, void* spatialState, zVEC3 & fifth);
	static int __fastcall zCCollisionCharacterCalcSlideVectorHook (void* pThis, zVEC3 const & first, zVEC3 const & second, zVEC3& third, zVEC3& fourth, float& fifth);
	static int __fastcall zCCollisionCharacterTestSoftCollisionsHook(void* pThis, zVEC3 const & first, zVEC3 const & second, zVEC3& third, zVEC3& fourth);
};

struct Motion
{
	zVEC3 normal;
	float distance;
	zVEC3 objectPos;
	zVEC3 intersect;
};

/*class zCMaterial {
public:
	static int __thiscall IsPortalMaterial(void* pThis);
};*/

__int32 __cdecl sysGetTime();
void levitate();
void Test(oCNpc* npc);
int calcPastFrameTime();
Motion getCollideYDir(zVEC3, float, bool);
Motion getCollideForwardDir(oCNpc*, float, bool);
void __cdecl zCVobCheckAutoUnlinkHook(zCVob* vob);

#endif __LEVITATION_H