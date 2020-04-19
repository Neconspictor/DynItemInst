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
#include <api/g2/ocnpc.h>

#include <chrono>
#include <vector>
#include <memory>

class Interpolator
{
public:

	using time_point = std::chrono::system_clock::time_point;
	using milliseconds = std::chrono::milliseconds;

	Interpolator(const zVEC3& start, time_point startTime, milliseconds duration);
	virtual ~Interpolator() = default;

	virtual zVEC3 interpolate(time_point timePoint) const = 0;
	virtual bool finished(time_point timePoint) const;
	virtual time_point getStartTime() const;
	virtual time_point getEndTime() const;

protected:
	virtual float calcFactor(time_point timePoint) const;

	const zVEC3 m_start;
	const time_point m_startTime;
	const time_point m_endTime;

	static std::stringstream mLogStream;
};


class LinearInterpolator : public Interpolator
{
public:
	LinearInterpolator(const zVEC3& start, const zVEC3& end, time_point startTime, milliseconds duration);
	virtual ~LinearInterpolator() = default;

	zVEC3 interpolate(time_point timePoint) const override;

private:

	zVEC3 m_end;
};

class TelekinesisInterpolator : public Interpolator
{
public:
	TelekinesisInterpolator(const zVEC3& start, const zVEC3& pitStop, const zVEC3& end, time_point startTime, milliseconds duration);
	virtual ~TelekinesisInterpolator() = default;

	zVEC3 interpolate(time_point timePoint) const override;

	static std::unique_ptr<TelekinesisInterpolator> createTelekinesisInterpolator(const zVEC3& start, 
		const zVEC3& end, 
		float pitStopHeight, 
		float speed // cm / s
	);

private:

	static milliseconds calcPitStopDuration(const zVEC3& vec3, const zVEC3& pitStop, const zVEC3& end, milliseconds duration);

	milliseconds m_PitStopDuration; // the duration of the pit stop
	std::unique_ptr<LinearInterpolator> m_pitStopInterpolator; // used to calculate the pit stop
	std::unique_ptr<LinearInterpolator> m_lastStopInterpolator; // used to calculate the last stop
};


class Telekinesis : public Module
{
public:
	Telekinesis();
	virtual ~Telekinesis() = default;
	virtual void hookModule() override;
	virtual void unHookModule() override;

	static int TELEKINESIS_IsVobSeeable(oCNpc* npc, zCVob* vob);

	static TelekinesisInterpolator* TELEKINESIS_CreateInterpolator(const zVEC3* vobPosition,
		const zVEC3* npcPosition,
		int upMoveAmount,  // cm
		int speed // cm / s
	);

	static void TELEKINESIS_GetInterpolatedVec(TelekinesisInterpolator* interpolatorPtr, zVEC3* dest);

	static void TELEKINESIS_DeleteInterpolator(TelekinesisInterpolator* interpolatorPtr);

	static void TELEKINESIS_Interpolate(TelekinesisInterpolator* interpolatorPtr, zCVob* vob);

	static void TELEKINESIS_ClearInterpolators();


private:

	static std::vector<std::unique_ptr<TelekinesisInterpolator>> mInterpolators;
};