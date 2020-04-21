#pragma once
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

#include <Module.h>
#include <api/g2/ztypes.h>
#include "api/g2/ocnpc.h"
#include <math.h>

#include <GothicFunctions.h>

struct Motion
{
	zVEC3 normal;
	float distance;
	zVEC3 objectPos;
	zVEC3 intersect;
};


class Levitation : public Module
{
public:

	constexpr static int COLLISION_HEAD_SIZE_Y = 25;

	static bool gameIsPaused;
	static bool noCollision;
	static int frameTime;
	static int diffFrameTime;
	static bool mCustomCollisionDetected;

	Levitation();
	virtual ~Levitation();
	virtual void hookModule() override;
	virtual void unHookModule() override;


	static int LEVITATION_IsGamePaused();

	static void __thiscall zCVobDoFrameActivityHook(void* pThis);
	static void __thiscall zCVobSetPhysicsEnabledHook(void* pThis, int second);
	static void __thiscall oCGamePauseHook(void* pThis, int second);
	static void __thiscall oCGameUnpauseHook(void* pThis);
	static void __thiscall zCVobSetCollDetHook(void* pThis, int second); //not used!!!
	static int __thiscall zCAIPlayerCheckFloorSlidingHook(void* pThis); //not used!!!
	static void __thiscall DoSurfaceAlignmentHook(void* pThis); //not really used!!!
	static void __thiscall zCVobCheckAndResolveCollisionsHook(void* pThis);
	static void __thiscall zCTriggerOnTouchHook(void* pThis, zCVob*); //not really(?) used!!!
	static int __thiscall zCVobHasEnoughSpaceHook(void* pThis, zVEC3&);  //not really(?) used!!!
	static zVEC3 __thiscall zCVobGetVelocityHook(void* pThis); //not really used!!!

	static int __cdecl TestStatic_Char_CharHook(float, void* collisionObject1, zMAT4 const& mat1, void* collisionObject2, zMAT4 const& mat2, void* collisionReportPtr);



	//.text:00550870 ; private: int __fastcall zCCollObjectCharacter::CalcSlideVector(class zVEC3 const &, class zVEC3 const &, class zVEC3 &, class zVEC3 &, float &)
	static int __fastcall zCCollObjectCharacterCalcSlideVectorHook(void* pThis, zVEC3 const & vec1, zVEC3 const & vec2, zVEC3 & vec3, zVEC3 & vec4, float& floatValue);

	//.text:00487C50; public: class zMAT4 & __thiscall zMAT4::SetTranslation(class zVEC3 const &)
	static zMAT4& __thiscall zMAT4SetTranslationHook(zMAT4* pThis, zVEC3 const & translation);

	//.text:0061ACD0 ; public: void __thiscall zCVob::CalcGroundPoly(void)
	static void __thiscall zCVobCalcGroundPolyHook(void* pThis);


	//.text:0061D2D0 ; private: void __thiscall zCVob::UpdatePhysics(void)
	static void __thiscall zCVobUpdatePhysicsHook(void* pThis);

	//.text:0061E0D0 ; public: virtual void __thiscall zCVob::EndMovement(int)
	static void __thiscall zCVobEndMovementHook(void* pThis, int arg0);

	//.text:00552180 ; private: int __fastcall zCCollObjectCharacter::TestSoftCollisions(class zVEC3 const &, class zVEC3 const &, class zVEC3 &, class zVEC3 &)
	static int __fastcall zCCollObjectCharacterTestSoftCollisionsHook(void* pThis, zVEC3 const& vec1, zVEC3& vec2, zVEC3& vec3, zVEC3& vec4);

	static int __fastcall zCCollObjectCharacterTestHardCollisionsHook(void* pThis, zVEC3& vec1, zVEC3& vec2, zVEC3& vec3,
		zTSpatialState* zTSpatialState, zVEC3& vec4);

	//.text:00553230 ; public: virtual void __fastcall zCCollObjectCharacter::DetectCollisionsSelf(class zCArray<class zCCollisionObject *> const &, class zCArray<class zCCollisionReport *> &)
	static  void __fastcall zCCollObjectCharacterDetectCollisionsSelfHook(void* pThis, void* collisionObjectArray, void* collisionReports);

	static void __thiscall zCAIPlayerCheckPhysicsHook(void* pThis);

	static void __fastcall zCCollObjectCharacterFindFloorWaterCeilingHook(void* pThis, zVEC3 const & vec1, zTSpatialState* zTSpatialState);

															   //non hooks
	static int __thiscall zCVobIsColliding(void* pThis);
	static int __fastcall zCBspBaseCollectPolysInBBox3D(void*, zTBBox3D const &, zCPolygon** &, int&);

	static void __fastcall zCBspBaseCollectVobsInBBox3D(void* pThis, zCArray<zCVob*>& collectedVobs, zTBBox3D const& boundingBox);

	//.text:00511320 public: int __thiscall zCAIPlayer::CheckEnoughSpaceMoveDir(class zVEC3 const &, int) proc near
	static int __thiscall zCAIPlayerCheckEnoughSpaceMoveDirHook(void* pThis, zVEC3 const &, int);

	//.text:00699F60 ; protected: int __thiscall oCAIHuman::PC_ActionMove(int)
	static int __thiscall oCAIHumanPC_ActionMoveHook(void* pThis, int param1);




	static __int32 __cdecl sysGetTime();
	static zVEC3 levitate();
	static bool doCustomCollisionCheck(oCNpc* npc);
	static int calcPastFrameTime();
	static Motion getCollideYDir(zVEC3, float, bool);
	static Motion getCollideForwardDir(oCNpc*, float, bool);
	static bool check_prePass(oCNpc* hero, const zMAT4& mat);
	static bool checkVobCollision(void* zCBspBaseObject, zCVob* testedVob, zTBBox3D const& boundingBox);
	static void doFloorAligning(zVEC3* finalPosition, zMAT4* mat);
	static bool isLevitationActive();
	static int getMinHoverDistance();
	static float getSpeedVertical();
	static float getSpeedForward();
	static float getSpeedBackward();
	static float getGravity();
	static float getTurnSpeed();

	static int getMoveForwardKey();
	static int getMoveBackwardKey();
	static int getTurnLeftKey();
	static int getTurnRightKey();
	static int getMoveUpKey();
	static int getMoveDownKey();

	static bool customCollisionDetected();

};