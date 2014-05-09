/*
*
*/
#pragma once
#ifndef _CLEARSKY_TIMER_H_
#define _CLEARSKY_TIMER_H_

//#include "core/precompiled.h"
#include "core/utils.h"

namespace clearsky
{

	class CLEARSKY_API Timer
	{
		public:
			Timer();
			~Timer();

			void start();
			void stop();
			void reset();

			__int64 getStartValue();
			__int64 getEndValue();
			__int64 getDifferenceCounts();
			__int64 getCountsPerSec();
			double getDuration();

			static unsigned long getTickCount();
			static unsigned long getTime();
			static __int64 getPerformanceCount();

			int getFPS();

			double getDelta();
			
		private:
			bool m_enable;
			double m_secPerCount;

			__int64 m_countsPerSec;
			__int64 m_startCounts;
			__int64 m_endCounts;
			__int64 m_difference;
			double m_resolution;

			unsigned int m_start;
			unsigned int m_stop;
			

			//for getFPS
			double m_currTime;
			double m_prevTime;
			double m_numFPS;
			double m_tempFPS;

			//for getDelta
			__int64 m_last;
			__int64 m_now;
	};
}

#endif