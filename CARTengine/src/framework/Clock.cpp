#include "Clock.h"

namespace cart {

	unique<Clock> Clock::__Clock{ nullptr };

	Clock& Clock::Get()
	{
		if (!__Clock) {
			__Clock = unique<Clock>{ new Clock };
		}
		return *__Clock;
	}

	void Clock::Release()
	{
		__Clock = nullptr;
	}

	void Clock::Reset()
	{
		m_previousTime = steady_clock::now();
		m_startTime = steady_clock::now();
	}

	double Clock::ElapsedTime()
	{
		return m_elapsedTime.count();
	}

	double Clock::DeltaTime()
	{		
		return  m_deltaTime.count();
	}

	void Clock::TimeScale(float _t)
	{
		m_timeScale = _t;
	}

	double Clock::TimeScale()
	{
		return m_timeScale;
	}

	Clock::Clock() :m_startTime{}, m_elapsedTime(), m_timeScale(1.f), m_deltaTime()
	{
	}

	time_t Clock::Now() {
		auto now = system_clock::now();
		std::time_t t = system_clock::to_time_t(now);		
		return t;
	}
	void Clock::Tick()
	{
		steady_clock::time_point curTime = steady_clock::now();
		m_deltaTime = curTime - m_previousTime;
		m_previousTime = steady_clock::now();
		m_elapsedTime = m_previousTime - m_startTime;
	}
	Clock::~Clock()
	{
		//LOG("Clock Deleted!");
	}
}