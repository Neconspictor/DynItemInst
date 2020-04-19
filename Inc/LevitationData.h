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

#include <api/g2/ocnpc.h>

//.text:00621FA0 public: int __fastcall zCWorld::TraceRayNearestHit(class zVEC3 const &, class zVEC3 const &, class zCVob *, int) proc near
typedef int(__fastcall* ZCWorldTraceRayNearestHit)(void* pThis, zVEC3 const &, zVEC3 const &, zCVob*, int);

//.text:0057DF20 public: virtual struct zTBBox3D __thiscall zCModel::GetBBox3D(void) proc near
typedef zTBBox3D(__thiscall* ZCModelGetBBox3D)(void* pThis);


class LevitationData
{
public:

	static const float TRACE_RAY_DISTANCE;
	static const float HOVER_DISTANCE;	// 40cm is the default hover distance
	static const float JUMP_VALUE;
	static const float MOVE_UP_SPEED;	// 50cm/s

	float oldXPos;
	float oldYPos;
	float oldZPos;

	zVEC3 oldLook;

	LevitationData();

	zVEC3 getOldPosition();

	float getHoverDistance();

	void update(oCNpc* npc, float distance);

	/**
	* Provides the distance from the position pos to the world ground (worldmesh and vobs with collision).
	* @return: Is smaller zero if the BBox of the underlying npc collides with the ground.
	Otherwise the result will be greater or equal zero;
	*/
	float getDistanceToGround(zVEC3& pos);

	void setOldPosition(zVEC3 position);

	void setOldDistanceToGround(float distance);

	bool getMoveDown();
	void setMoveDown(bool down);

	void setHoverDistance(float distance);

	static ZCWorldTraceRayNearestHit zCWorldTraceRayNearestHit;
	static ZCModelGetBBox3D zCModelGetBBox3D;

private:
	zVEC3 oldPosition;
	float middlePointDistance;
	float oldDistanceToGround;
	float currentHoverDistance;
	bool moveDown;

	bool isGreaterOrEqual(zVEC3& newPosition);

	float getMiddlePointDistance(oCNpc* npc);

	void adaptHoverDistance(zVEC3& newPosition, float newDistanceToGround);
};
