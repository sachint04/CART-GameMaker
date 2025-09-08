#pragma once
#include <chrono>
#include "Core.h"

using namespace std::chrono;
using namespace std::literals;

namespace cart {

	class Clock {
	public:
		static Clock& Get();
		static void Release();
		uint64_t getCurrentTimeMillis();
		void Reset();
		double ElapsedTime();
		double DeltaTime();
		void TimeScale(float _t);
		double TimeScale();
		void Tick();
		time_t Now();
		~Clock();
	protected:
		Clock();
	private:
		static unique<Clock> __Clock;

		steady_clock::time_point m_startTime;
		steady_clock::time_point m_previousTime;
		
		duration<double> m_elapsedTime;
		duration<double> m_deltaTime;

		float m_timeScale;
	};

}