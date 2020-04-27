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

#include <Telekinesis.hpp>
#include <chrono>
#include <algorithm>
#include "Util.h"
#include <api/g2/ocmob.h>
#include <api/g2/ocnpc.h>
#include <api/g2/ocgame.h>
#include <api/g2/zcworld.h>
#include <api\g2\zctimer.h>
#include <list>

std::stringstream AbstractInterpolator::mLogStream;
std::vector<std::unique_ptr<PathInterpolator>> Telekinesis::mInterpolators;

float AbstractInterpolator::getFrameTime()
{
	return zCTimer::GetTimer()->frameTimeFloatSecs;
}

float AbstractInterpolator::getTotalTime()
{
	return zCTimer::GetTimer()->totalTimeFloatSecs;
}

LinearInterpolator::LinearInterpolator(const zVEC3& startPos, const zVEC3& endPos, Seconds startTime, Speed speed) :
	AbstractInterpolator(),
	mStartPosition(startPos), mEndPosition(endPos), mStartTime(startTime), mSpeed(speed)
{
}

zVEC3 LinearInterpolator::interpolate(Seconds timePoint)
{
	const auto factor = calcFactor(timePoint);
	const auto dir = mEndPosition - mStartPosition;
	return mStartPosition + factor.get() * dir;
}

AbstractInterpolator::Seconds LinearInterpolator::getStartTime() const
{
	return mStartTime;
}

AbstractInterpolator::Seconds LinearInterpolator::getEndTime() const
{
	return getStartTime() + getDuration();
}

AbstractInterpolator::Speed LinearInterpolator::getSpeed() const
{
	return mSpeed;
}

const zVEC3& LinearInterpolator::getStartPosition() const
{
	return mStartPosition;
}

const zVEC3& LinearInterpolator::getEndPosition() const
{
	return mEndPosition;
}

bool LinearInterpolator::finished(Seconds timePoint) const
{
	return timePoint >= getEndTime();
}

AbstractInterpolator::Distance LinearInterpolator::getDistance() const
{
	const auto differene = getEndPosition() - getStartPosition();
	return differene.Length();
}

AbstractInterpolator::Seconds LinearInterpolator::getDuration() const
{
	const auto speed = getSpeed();
	if (speed < 0.0001f) {
		return std::numeric_limits<Seconds>::infinity();
	}
	return getDistance() / getSpeed();
}

AbstractInterpolator::Factor LinearInterpolator::calcFactor(Seconds timePoint) const
{
	const auto startTime = getStartTime();
	const auto distance = getDistance();
	const auto passedTime = timePoint - startTime;
	const auto passedDistance = passedTime * getSpeed();

	return passedDistance / distance;
}

PathInterpolator::PathInterpolator(std::queue<zVEC3> stations,
	Seconds startTime,
	Speed speed)
	:
	AbstractInterpolator(),
	// at(0) throws std::out_of_range error
	mLinearInterpolator(zVEC3(), zVEC3(), startTime, speed),
	mStations(std::move(stations))
{
	if (!mStations.empty()) {
		mLinearInterpolator = LinearInterpolator(mStations.front(), mStations.front(), startTime, speed);
		mStations.pop();
	}

	//m_pitStopInterpolator(start, pitStop, startTime, m_PitStopDuration),
	//m_lastStopInterpolator(pitStop, end, startTime + m_PitStopDuration, duration - m_PitStopDuration)
}

zVEC3 PathInterpolator::interpolate(Seconds timePoint)
{

	if (mLinearInterpolator.finished(timePoint) && !mStations.empty()) {
		const auto& startPos = mLinearInterpolator.getEndPosition();
		const auto& endPos = mStations.front();
		const auto startTime = mLinearInterpolator.getEndTime();
		const auto& speed = mLinearInterpolator.getSpeed();
		

		mLinearInterpolator = LinearInterpolator(startPos, endPos, startTime, speed);
		mStations.pop();
	}

	return mLinearInterpolator.interpolate(timePoint);
}

bool PathInterpolator::finished(Seconds timePoint) const
{
	if (mStations.empty()) {
		return mLinearInterpolator.finished(timePoint);
	}
	return false;
}

std::unique_ptr<PathInterpolator> PathInterpolator::createTelekinesisInterpolator(const zVEC3& start, const zVEC3& end,
	float pitStopHeight, 
	float speed, // cm/s
	float delay // ms
)
{
	zVEC3 pitStop = start;
	pitStop.y += pitStopHeight;

	// calc duration from speed
	float startPitStopDistance = (pitStop - start).Length();
	float pitStopEndDistance = (end - pitStop).Length();
	float distance = startPitStopDistance + pitStopEndDistance;

	const auto startTime = getTotalTime() + toSeconds(delay);

	return std::make_unique<PathInterpolator>(std::queue<zVEC3>({ start, pitStop, end }), startTime, speed);
}

Telekinesis::Telekinesis() : Module("Telekinesis")
{
}

void Telekinesis::hookModule()
{
}

void Telekinesis::unHookModule()
{
}



PathInterpolator* Telekinesis::TELEKINESIS_CreateInterpolator(const zVEC3* vobPosition, const zVEC3* npcPosition,
	int upMoveAmount, int speed, int delay)
{
	mLogStream << __FUNCTION__ << ": vobPosition = " << *vobPosition << std::endl;
	mLogStream << __FUNCTION__ << ": npcPosition = " << *npcPosition << std::endl;
	mLogStream << __FUNCTION__ << ": upMoveAmount = " << upMoveAmount << std::endl;
	mLogStream << __FUNCTION__ << ": speed = " << speed << std::endl;
	mLogStream << __FUNCTION__ << ": delay = " << delay << std::endl;
	util::debug(mLogStream);


	std::unique_ptr<PathInterpolator> interpolator = PathInterpolator::createTelekinesisInterpolator(*vobPosition, *npcPosition, 
		static_cast<float>(upMoveAmount), 
		static_cast<float>(speed),
		static_cast<float>(delay));
	mInterpolators.emplace_back(std::move(interpolator));
	return mInterpolators.back().get();
}

void Telekinesis::TELEKINESIS_GetInterpolatedVec(PathInterpolator* interpolatorPtr, zVEC3* dest)
{
	zVEC3 result = interpolatorPtr->interpolate(AbstractInterpolator::getTotalTime());
	*dest = result;
}

void Telekinesis::TELEKINESIS_DeleteInterpolator(PathInterpolator* interpolatorPtr)
{

	auto newEnd = std::remove_if(mInterpolators.begin(), mInterpolators.end(), [&](auto& it)
		{
			return it.get() == interpolatorPtr;
		});

	if (newEnd != mInterpolators.end())
	{
		mLogStream << __FUNCTION__ << ": successfully removed interpolator!" << std::endl;
		util::debug(mLogStream);
	}

	mInterpolators.erase(newEnd, mInterpolators.end());
}

void Telekinesis::TELEKINESIS_Interpolate(PathInterpolator* interpolatorPtr, zVEC3* interpolatedPosition)
{
	*interpolatedPosition = interpolatorPtr->interpolate(AbstractInterpolator::getTotalTime());
	//zVEC3 current = vob->GetVobPosition();
	//zVEC3 diff = result - current;
	//vob->Move(diff.x, diff.y, diff.z);
	/*vob->SetPositionWorld(result);
	auto& trafo = vob->trafoObjToWorld;
	trafo.m[0][3] = result.x;
	trafo.m[1][3] = result.y;
	trafo.m[2][3] = result.z;*/
}

void Telekinesis::TELEKINESIS_ClearInterpolators()
{
	mInterpolators.clear();
}

int Telekinesis::TELEKINESIS_IsVobSeeable(oCNpc* npc, zCVob* vob)
{
	//.text:00741C10 ; public: int __thiscall oCNpc::CanSee(class zCVob *, int)
	using OCNpcCanSee = int(__thiscall*)(void*, void*, int);
	OCNpcCanSee oCNpcCanSee = (OCNpcCanSee)0x00741C10;


	//.text:00621FA0 ; public: int __fastcall zCWorld::TraceRayNearestHit(class zVEC3 const &, class zVEC3 const &, class zCVob *, int)
	using ZCWorldTraceRayNearestHit = int(__fastcall*)(void*, zVEC3 const&, zVEC3 const&, void*, int);
	ZCWorldTraceRayNearestHit zCWorldTraceRayNearestHit = (ZCWorldTraceRayNearestHit)0x00621FA0;

	const int zTraceRay_vob_ignore_no_cd_dyn = 1 << 0;  // Ignore vobs without collision
	const int zTraceRay_vob_bbox = 1 << 2;  // Intersect bounding boxes (important to detect NPCs)
	const int zTraceRay_poly_normal = 1 << 7;  // Report normal vector of intersection
	const int zTraceRay_poly_ignore_transp = 1 << 8;  // Ignore alpha polys (without this, trace ray is bugged)
	const int zTraceRay_poly_test_water = 1 << 9;  // Intersect water
	const int zTraceRay_vob_ignore_projectiles = 1 << 14; // Ignore projectiles
	const int zTRACERAY_VOB_IGNORE = 1 << 1; //Alle Vobs ignorieren (nur statisches Mesh beachten)

	const int zTRACERAY_VOB_IGNORE_CHARACTER = 1 << 11; //Ignoriere Npcs



	int found = oCNpcCanSee(npc, vob, 0);

	zCWorld* world = oCGame::GetGame()->GetWorld();
	const int traceFlags = zTraceRay_vob_ignore_no_cd_dyn
		//| zTRACERAY_VOB_IGNORE
		| zTRACERAY_VOB_IGNORE_CHARACTER
		| zTraceRay_poly_ignore_transp
		| zTraceRay_vob_ignore_projectiles
		| zTraceRay_poly_test_water;

	const zVEC3 direction = vob->GetVobPosition() - npc->GetPosition();
	const zVEC3 npcPosition = npc->GetPosition();


	const int result = zCWorldTraceRayNearestHit(world, npcPosition, direction, npc, traceFlags);

	const auto vobListSize = static_cast<int>(world->traceRayVobList.GetSize());

	for (int i = 0; i < vobListSize; ++i)
	{
		zCVob* vob = world->traceRayVobList[i];
		if (vob != nullptr)
		{
			switch (vob->GetVobType())
			{
			case VOB_TYPE_MOB:
			{
				oCMob* mob = (oCMob*)vob;
				mLogStream << __FUNCTION__ << ": vob is mob: " << mob->name.ToChar() << std::endl;
				util::debug(mLogStream);
				break;
			}
			case VOB_TYPE_ITEM:
			{
				oCItem* item = (oCItem*)vob;
				mLogStream << __FUNCTION__ << ": vob is item: " << item->name.ToChar() << std::endl;
				util::debug(mLogStream);
				break;
			}
			case VOB_TYPE_NPC:
			{
				oCNpc* focusNpc = (oCNpc*)vob;
				mLogStream << __FUNCTION__ << ": vob is npc: " << focusNpc->name[0].ToChar() << std::endl;
				util::debug(mLogStream);
				break;
			}
			default:;
			}
			vob->GetVobType();
		}
	}


	if (result && !found)
	{
		float intersectLength = (world->foundIntersection - npcPosition).Length();
		float VobLength = direction.Length();

		zCVob* foundVob = world->foundVob;
		found = (world->foundVob == vob) ? 1 : 0;

		if (!found && foundVob != nullptr)
		{

			static const int zCVob_bitfield0_collDetectionStatic = ((1 << 1) - 1) << 6;
			static const int zCVob_bitfield0_collDetectionDynamic = ((1 << 1) - 1) << 7;

			bool hasStaticCollision = foundVob->bitfield[0] & zCVob_bitfield0_collDetectionStatic;
			bool hasDynamicCollision = foundVob->bitfield[0] & zCVob_bitfield0_collDetectionDynamic;
		}

		if (!found)
			found = (intersectLength + 20 < VobLength) ? 0 : 1;
	}

	return found;
}