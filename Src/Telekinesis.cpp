#include <Telekinesis.hpp>
#include <chrono>
#include <algorithm>
#include "Util.h"

std::stringstream Interpolator::logStream;

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

Telekinesis::Telekinesis()
{
	moduleDesc = "Telekinesis";
}

void Telekinesis::hookModule()
{
}

void Telekinesis::unHookModule()
{
}