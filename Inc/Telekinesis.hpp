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
#include <queue>
#include <list>

/**
* Factor is a floating point number between zero and 1.
*/
template<class T>
class Factor {
public: 

	Factor(T value) : mDecimalFactor(value){
		mDecimalFactor = std::max<T>(mDecimalFactor, 0);
		mDecimalFactor = std::min<T>(mDecimalFactor, 1);
	}

	const T& get() const {
		return mDecimalFactor;
	}

private:
	T mDecimalFactor;
};

class AbstractInterpolator
{
public:

	using Decimal = float;

	using Seconds = Decimal;

	/** 
	 * Speed is measured in centimeters per second
	 */
	using Speed = Decimal;

	/**
	 * Distance is measured in centimeters
	 */
	using Distance = Decimal;

	using Factor = Factor<Decimal>;

	AbstractInterpolator() = default;
	AbstractInterpolator(const AbstractInterpolator&) = default;

	virtual ~AbstractInterpolator() = default;

	virtual zVEC3 interpolate(Seconds timePoint) = 0;
	virtual bool finished(Seconds timePoint) const = 0;

	/**
	 * Provides frame time in seconds
	 */
	static float getFrameTime();

	/**
	 * Provides total time in seconds
	 */
	static float getTotalTime();

	static inline constexpr float toSeconds(float milliSeconds) {
		return milliSeconds / 1000.0f;
	}

protected:

	static std::stringstream mLogStream;
};


class LinearInterpolator : public AbstractInterpolator
{
public:
	LinearInterpolator(const zVEC3& startPos,
		const zVEC3& endPos,
		Seconds startTime,
		Speed speed);
	virtual ~LinearInterpolator() = default;

	LinearInterpolator(const LinearInterpolator&) = default;
	LinearInterpolator& operator=(const LinearInterpolator&) = default;
	LinearInterpolator& operator=(LinearInterpolator&&) = default;

	zVEC3 interpolate(Seconds timePoint) override;

	Seconds getStartTime() const;
	Seconds getEndTime() const;
	Speed getSpeed() const;
	const zVEC3& getStartPosition() const;
	const zVEC3& getEndPosition() const;

	bool finished(Seconds timePoint) const  override;

private:
	zVEC3 mStartPosition;
	zVEC3 mEndPosition;
	Seconds mStartTime;
	Speed mSpeed;

	Distance getDistance() const;
	Seconds getDuration() const;
	virtual Factor calcFactor(Seconds timePoint) const;
};

class PathInterpolator : public AbstractInterpolator
{
public:
	PathInterpolator(std::queue<zVEC3> stations,
		Seconds startTime,
		Speed speed);

	virtual ~PathInterpolator() = default;

	zVEC3 interpolate(Seconds timePoint) override;
	bool finished(Seconds timePoint) const  override;

	static std::unique_ptr<PathInterpolator> createTelekinesisInterpolator(const zVEC3& start, 
		const zVEC3& end, 
		float pitStopHeight, 
		float speed, // cm / s
		float delay // ms
		
	);

private:

	LinearInterpolator mLinearInterpolator; // used to calculate the pit stop
	std::queue<zVEC3> mStations;
};


class Telekinesis : public Module
{
public:
	Telekinesis();
	virtual ~Telekinesis() = default;
	virtual void hookModule() override;
	virtual void unHookModule() override;

	static int TELEKINESIS_IsVobSeeable(oCNpc* npc, zCVob* vob);

	static PathInterpolator* TELEKINESIS_CreateInterpolator(const zVEC3* vobPosition,
		const zVEC3* npcPosition,
		int upMoveAmount,  // cm
		int speed, // cm / s
		int delay // ms
	);

	static void TELEKINESIS_GetInterpolatedVec(PathInterpolator* interpolatorPtr, zVEC3* dest);

	static void TELEKINESIS_DeleteInterpolator(PathInterpolator* interpolatorPtr);

	static void TELEKINESIS_Interpolate(PathInterpolator* interpolatorPtr, zVEC3* interpolatedPosition);

	static void TELEKINESIS_ClearInterpolators();


private:

	static std::vector<std::unique_ptr<PathInterpolator>> mInterpolators;
};