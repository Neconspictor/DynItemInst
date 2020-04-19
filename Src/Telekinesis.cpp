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

std::stringstream Interpolator::mLogStream;
std::vector<std::unique_ptr<TelekinesisInterpolator>> Telekinesis::mInterpolators;


Interpolator::Interpolator(const zVEC3& start, time_point startTime, milliseconds duration)
	:
	m_start(start),
	m_startTime(startTime),
	m_endTime(startTime + duration)
{
}

bool Interpolator::finished(time_point timePoint) const
{
	return timePoint.time_since_epoch() >= m_endTime.time_since_epoch();
}

Interpolator::time_point Interpolator::getStartTime() const
{
	return m_startTime;
}

Interpolator::time_point Interpolator::getEndTime() const
{
	return m_endTime;
}

float Interpolator::calcFactor(time_point timePoint) const
{

	using namespace std;
	timePoint = min<>(timePoint, m_endTime);
	timePoint = max<>(timePoint, m_startTime);

	auto targetDuration = chrono::duration_cast<chrono::duration<long double>>(timePoint - m_startTime);
	auto maxDuration = chrono::duration_cast<chrono::duration<long double>>(m_endTime - m_startTime);
	float factor =  static_cast<float>(targetDuration.count() / maxDuration.count());

	return factor;
}

LinearInterpolator::LinearInterpolator(const zVEC3& start, const zVEC3& end, time_point startTime,
	milliseconds duration)
	:
	Interpolator(start, startTime, duration),
	m_end(end)
{
}

zVEC3 LinearInterpolator::interpolate(time_point timePoint) const
{
	float factor = calcFactor(timePoint);
	zVEC3 dir = m_end - m_start;
	return m_start + factor * dir;
}

TelekinesisInterpolator::TelekinesisInterpolator(const zVEC3& start, const zVEC3& pitStop, const zVEC3& end, time_point startTime,
	milliseconds duration)
	:
	Interpolator(start, startTime, duration),
	m_PitStopDuration(0),
	m_pitStopInterpolator(nullptr),
	m_lastStopInterpolator(nullptr)
{
	m_PitStopDuration = calcPitStopDuration(start, pitStop, end, duration);
	m_pitStopInterpolator = std::make_unique<LinearInterpolator>(start, pitStop, startTime, m_PitStopDuration);

	milliseconds lastStopMillis = duration - m_PitStopDuration;

	m_lastStopInterpolator = std::make_unique<LinearInterpolator>(pitStop, end, startTime + m_PitStopDuration, lastStopMillis);

	//m_pitStopInterpolator(start, pitStop, startTime, m_PitStopDuration),
	//m_lastStopInterpolator(pitStop, end, startTime + m_PitStopDuration, duration - m_PitStopDuration)
}

zVEC3 TelekinesisInterpolator::interpolate(time_point timePoint) const
{

	if (!m_pitStopInterpolator->finished(timePoint))
	{
		return m_pitStopInterpolator->interpolate(timePoint);
	}

	return m_lastStopInterpolator->interpolate(timePoint);
}

std::unique_ptr<TelekinesisInterpolator> TelekinesisInterpolator::createTelekinesisInterpolator(const zVEC3& start, const zVEC3& end,
	float pitStopHeight, float speed)
{
	zVEC3 pitStop = start;
	pitStop.y += pitStopHeight;
	std::chrono::system_clock clock;

	// calc duration from speed
	float startPitStopDistance = (pitStop - start).Length();
	float pitStopEndDistance = (end - pitStop).Length();
	float distance = startPitStopDistance + pitStopEndDistance;

	// distance is measured in cm
	// speed is cm / s
	// we need milliseconds
	long long millis = (distance * 1000.0f)/ speed;

	//logStream << "TelekinesisInterpolator::createTelekinesisInterpolator(): distance = " << distance << std::endl;
	//logStream << "TelekinesisInterpolator::createTelekinesisInterpolator(): millis = " << millis << std::endl;
	//util::logWarning(&logStream);


	return std::make_unique<TelekinesisInterpolator>(start, pitStop, end, clock.now(), milliseconds(millis));
}


Interpolator::milliseconds TelekinesisInterpolator::calcPitStopDuration(const zVEC3& start, const zVEC3& pitStop, const zVEC3& end,
	milliseconds duration)
{
	float pitStopDistance = (pitStop - start).Length();
	float lastStopDistance = (end - pitStop).Length();
	float summedDistance = pitStopDistance + lastStopDistance;

	static const float e = 0.0001f;
	float factor = pitStopDistance / summedDistance;

	long long pitStopMillis = factor * duration.count();

	return milliseconds(pitStopMillis);
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



TelekinesisInterpolator* Telekinesis::TELEKINESIS_CreateInterpolator(const zVEC3* vobPosition, const zVEC3* npcPosition,
	int upMoveAmount, int speed)
{
	mLogStream << __FUNCSIG__ << ": vobPosition = " << *vobPosition << std::endl;
	mLogStream << __FUNCSIG__ << ": npcPosition = " << *npcPosition << std::endl;
	mLogStream << __FUNCSIG__ << ": upMoveAmount = " << upMoveAmount << std::endl;
	mLogStream << __FUNCSIG__ << ": speed = " << speed << std::endl;
	util::debug(&mLogStream);


	std::unique_ptr<TelekinesisInterpolator> interpolator = TelekinesisInterpolator::createTelekinesisInterpolator(*vobPosition, *npcPosition, upMoveAmount, speed);
	mInterpolators.emplace_back(std::move(interpolator));
	return mInterpolators.back().get();
}

void Telekinesis::TELEKINESIS_GetInterpolatedVec(TelekinesisInterpolator* interpolatorPtr, zVEC3* dest)
{
	zVEC3 result = interpolatorPtr->interpolate(std::chrono::system_clock::now());
	*dest = result;
}

void Telekinesis::TELEKINESIS_DeleteInterpolator(TelekinesisInterpolator* interpolatorPtr)
{

	auto newEnd = std::remove_if(mInterpolators.begin(), mInterpolators.end(), [&](auto& it)
		{
			return it.get() == interpolatorPtr;
		});

	if (newEnd != mInterpolators.end())
	{
		mLogStream << __FUNCSIG__ << ": successfully removed interpolator!" << std::endl;
		util::debug(&mLogStream);
	}

	mInterpolators.erase(newEnd, mInterpolators.end());
}

void Telekinesis::TELEKINESIS_Interpolate(TelekinesisInterpolator* interpolatorPtr, zCVob* vob)
{
	zVEC3 result = interpolatorPtr->interpolate(std::chrono::system_clock::now());
	zVEC3 current = vob->GetVobPosition();
	zVEC3 diff = result - current;
	vob->Move(diff.x, diff.y, diff.z);
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

	for (int i = 0; i < world->traceRayVobList.GetSize(); ++i)
	{
		zCVob* vob = world->traceRayVobList[i];
		if (vob != nullptr)
		{
			switch (vob->GetVobType())
			{
			case VOB_TYPE_MOB:
			{
				oCMob* mob = (oCMob*)vob;
				mLogStream << __FUNCSIG__ << ": vob is mob: " << mob->name.ToChar() << std::endl;
				util::debug(&mLogStream);
				break;
			}
			case VOB_TYPE_ITEM:
			{
				oCItem* item = (oCItem*)vob;
				mLogStream << __FUNCSIG__ << ": vob is item: " << item->name.ToChar() << std::endl;
				util::debug(&mLogStream);
				break;
			}
			case VOB_TYPE_NPC:
			{
				oCNpc* focusNpc = (oCNpc*)vob;
				mLogStream << __FUNCSIG__ << ": vob is npc: " << focusNpc->name[0].ToChar() << std::endl;
				util::debug(&mLogStream);
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