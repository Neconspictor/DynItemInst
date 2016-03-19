#pragma once
#include <api/g2/ocnpc.h>

//.text:00621FA0 public: int __fastcall zCWorld::TraceRayNearestHit(class zVEC3 const &, class zVEC3 const &, class zCVob *, int) proc near
typedef int(__fastcall* ZCWorldTraceRayNearestHit)(void* pThis, zVEC3 const &, zVEC3 const &, zCVob*, int);

//.text:0057DF20 public: virtual struct zTBBox3D __thiscall zCModel::GetBBox3D(void) proc near
typedef zTBBox3D(__thiscall* ZCModelGetBBox3D)(void* pThis);


class LevitationBean
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

	LevitationBean(oCNpc* npc);

	zVEC3 getOldPosition();

	float getHoverDistance();

	void update(float distance);

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
	oCNpc* npc;
	zVEC3 oldPosition;
	float middlePointDistance;
	float oldDistanceToGround;
	float currentHoverDistance;
	bool moveDown;

	bool isGreaterOrEqual(zVEC3& newPosition);

	float getMiddlePointDistance();

	void adaptHoverDistance(zVEC3& newPosition, float newDistanceToGround);
};
