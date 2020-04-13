#include <api/g2/ztypes.h>
#include <api/g2/ocnpc.h>
#include <api/g2/zcmodel.h>
#include <api/g2/ocgame.h>
#include <api/g2/zcworld.h>
#include "LevitationData.h"
#include <iostream>
#include <math.h>


const float LevitationData::TRACE_RAY_DISTANCE = 1000000000;
const float LevitationData::HOVER_DISTANCE = 100.0f;	// 40cm is the default hover distance
const float LevitationData::JUMP_VALUE = 0.25f;
const float LevitationData::MOVE_UP_SPEED = 50.0f;	// 50cm/s

ZCWorldTraceRayNearestHit LevitationData::zCWorldTraceRayNearestHit = (ZCWorldTraceRayNearestHit)0x00621FA0;
ZCModelGetBBox3D LevitationData::zCModelGetBBox3D = (ZCModelGetBBox3D)0x0057DF20;


LevitationData::LevitationData()
{
	//oldPosition = npc->GetPosition();
	//this->middlePointDistance = getMiddlePointDistance();
	//oldDistanceToGround = getDistanceToGround(oldPosition);
	currentHoverDistance = HOVER_DISTANCE;
	moveDown = false;
}

zVEC3 LevitationData::getOldPosition()
{
	return oldPosition;
}

float LevitationData::getHoverDistance()
{
	return currentHoverDistance;
}

void LevitationData::update(oCNpc* npc, float distance)
{
	zVEC3 newPosition = npc->GetPosition();
	middlePointDistance = getMiddlePointDistance(npc);
	if (isGreaterOrEqual(newPosition)) {
		float distDiff = oldDistanceToGround - distance;
		if (distDiff > JUMP_VALUE * HOVER_DISTANCE) {
			adaptHoverDistance(newPosition, distance);
		}
	}
}

bool LevitationData::isGreaterOrEqual(zVEC3& newPosition)
{
	return newPosition.y >= oldPosition.y;
}

float LevitationData::getDistanceToGround(zVEC3& pos)
{
	zVEC3 posOfBottomBBox = pos;
	// If colliding pos could be beneath the ground. So we go one middle point distance up.
	// It is assumed that there is enough space above pos' y-axis;
	posOfBottomBBox.y += middlePointDistance;
	zVEC3 dir = posOfBottomBBox;
	dir.y -= TRACE_RAY_DISTANCE;
	zCWorld* world = oCGame::GetGame()->GetWorld();
	int flags = (1 << 0) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14);
	zCWorldTraceRayNearestHit(world, posOfBottomBBox, dir, 0, flags);
	//world->TraceRayNearestHit(posOfBottomBBox, dir, 0, flags);

	zVEC3* intersection = &(world->foundIntersection);
	return pos.y - intersection->y;
}

float LevitationData::getMiddlePointDistance(oCNpc* npc)
{
	zTBBox3D bBox = zCModelGetBBox3D(npc->GetModel()); //->GetBBox3D();
	float yDownExt = bBox.bbox3D_mins.y;
	return fabs(5 * yDownExt / 6.0f);
}

void LevitationData::adaptHoverDistance(zVEC3& newPosition, float newDistanceToGround)
{
	if (newDistanceToGround <= HOVER_DISTANCE) {
		//don't adapt npc's position. Update just current hover Distance
		currentHoverDistance = newDistanceToGround;
	}
	else
	{
		currentHoverDistance = HOVER_DISTANCE;
	}
}

void LevitationData::setOldPosition(zVEC3 position)
{
	oldPosition = position;
}

void LevitationData::setOldDistanceToGround(float distance)
{
	oldDistanceToGround = distance;
	if (oldDistanceToGround < HOVER_DISTANCE) {
		currentHoverDistance = oldDistanceToGround;
	}
}

bool LevitationData::getMoveDown()
{
	return moveDown;
}

void LevitationData::setMoveDown(bool down)
{
	moveDown = down;
}

void LevitationData::setHoverDistance(float distance)
{
	if (distance > HOVER_DISTANCE)
	{
		currentHoverDistance = HOVER_DISTANCE;
	}
	else
	{
		currentHoverDistance = distance;
	}
}