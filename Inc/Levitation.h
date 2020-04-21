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
	static void __thiscall oCGamePauseHook(void* pThis, int second);
	static void __thiscall oCGameUnpauseHook(void* pThis);

	static void __thiscall zCVobCheckAndResolveCollisionsHook(void* pThis);
	static void __thiscall zCVobUpdatePhysicsHook(void* pThis);
															   //non hooks
	static int __thiscall zCVobIsColliding(void* pThis);
	static int __fastcall zCBspBaseCollectPolysInBBox3D(void*, zTBBox3D const &, zCPolygon** &, int&);
	static void __fastcall zCBspBaseCollectVobsInBBox3D(void* pThis, zCArray<zCVob*>& collectedVobs, zTBBox3D const& boundingBox);
	static int __thiscall oCAIHumanPC_ActionMoveHook(void* pThis, int param1);



	static __int32 __cdecl sysGetTime();
	static zVEC3 levitate();
	static bool doCustomCollisionCheck(oCNpc* npc);
	static int calcPastFrameTime();
	static Motion getCollideYDir(zVEC3, float, bool);
	static Motion getCollideForwardDir(oCNpc*, float, bool);
	static bool check_prePass(oCNpc* hero, const zMAT4& mat);
	static bool checkVobCollision(void* zCBspBaseObject, zCVob* testedVob, zTBBox3D const& boundingBox);
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


	static void removeModelGravity(zCModel* model);

};