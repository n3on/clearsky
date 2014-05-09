/*
*
*/

#include "core/precompiled.h"
#include "core/timer.h"

namespace clearsky
{
	Timer::Timer():m_enable(true),
				   m_start(0),
				   m_stop(0),
				   m_currTime(0),
				   m_prevTime(GetTickCount()*0.001f),
				   m_numFPS(0),
				   m_tempFPS(0),
				   m_now(0),
				   m_last(0),
				   m_difference(0)
	{
		//TODO: make normal timer if the system has no highres timer
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_countsPerSec);
		this->m_resolution = 1/(double)this->m_countsPerSec;
	}

	Timer::~Timer()
	{
	}

	void Timer::start()
	{
		if(this->m_enable)
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&this->m_startCounts);
		}
	}

	__int64 Timer::getStartValue()
	{
		return this->m_startCounts;
	}

	__int64 Timer::getEndValue()
	{
		return this->m_endCounts;
	}

	__int64 Timer::getDifferenceCounts()
	{
		return this->m_difference;
	}

	__int64 Timer::getCountsPerSec()
	{
		return this->m_countsPerSec;
	}

	double Timer::getDuration()
	{
		return (double)this->m_difference*this->m_resolution;
	}

	void Timer::stop()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&this->m_endCounts);
		//this->m_stop = GetTickCount();
		this->m_difference = this->m_endCounts - this->m_startCounts;
	}

	void Timer::reset()
	{
		this->m_start = 0;
		this->m_stop  = 0;
		this->m_enable = true;
	}

	unsigned long Timer::getTickCount()
	{
		return GetTickCount();
	}

	unsigned long Timer::getTime()
	{
		return timeGetTime();
	}

	__int64 Timer::getPerformanceCount()
	{
		__int64 counts;
		QueryPerformanceCounter((LARGE_INTEGER*)&counts);

		return counts;
	}

	int Timer::getFPS()
	{
		this->m_currTime = GetTickCount()*0.001f;

		this->m_numFPS++;

		if((this->m_currTime-this->m_prevTime) > 1)
		{
			this->m_prevTime = this->m_currTime;
			this->m_tempFPS = this->m_numFPS;
			this->m_numFPS = 0;
		}

		return (int)this->m_tempFPS;
	}

	double Timer::getDelta()
	{
		__int64 counts;
		QueryPerformanceCounter((LARGE_INTEGER*)&counts);

		this->m_now  = counts;
		double delta  = (double)(this->m_now-this->m_last)*this->m_resolution; 	
		this->m_last = this->m_now;

		if(delta>2)
			delta=1;

		return delta;
	}
}