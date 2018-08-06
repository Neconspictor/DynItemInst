#pragma once

#include <Module.h>
#include <api/g2/ztypes.h>
#include <chrono>

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

	static std::stringstream logStream;
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
};