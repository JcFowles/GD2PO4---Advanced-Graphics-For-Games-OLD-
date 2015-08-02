#include "Timer.h"
#include <windows.h>

Timer::Timer()
{
	m_SecondsPerCount = (0.0);
	m_DeltaTime = (-1.0);
	m_BaseTime = (0);
	m_PausedTime = (0);
	m_PreviousTime = (0);
	m_CurrentTime = (0);
	m_Stopped = (false);
	
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
}

// Returns the total time elapsed since reset() was called, NOT counting any
// time when the clock is stopped.
float Timer::GetGameTime()const
{
	// If we are stopped, do not count the time that has passed since we stopped.
	//
	// ----*---------------*------------------------------*------> time
	//  mBaseTime       mStopTime                      mCurrentTime

	if( m_Stopped )
	{
		return (float)((m_StopTime - m_BaseTime)*m_SecondsPerCount);
	}

	// The distance mCurrentTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrentTime:  
	//
	//  (mCurrentTime - mPausedTime) - mBaseTime 
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrentTime
	
	else
	{
		return (float)(((m_CurrentTime-m_PausedTime)-m_BaseTime)*m_SecondsPerCount);
	}
}

float Timer::GetDeltaTime()const
{
	return (float)m_DeltaTime;
}

void Timer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PreviousTime = currTime;
	m_StopTime = 0;
	m_Stopped  = false;
}

void Timer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     

	if( m_Stopped )
	{
		m_PausedTime += (startTime - m_StopTime);	

		m_PreviousTime = startTime;
		m_StopTime = 0;
		m_Stopped  = false;
	}
}

void Timer::Stop()
{
	if( !m_Stopped )
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_StopTime = currTime;
		m_Stopped  = true;
	}
}

void Timer::Tick()
{
	if( m_Stopped )
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrentTime = currTime;

	// Time difference between this frame and the previous.
	m_DeltaTime = (m_CurrentTime - m_PreviousTime)*m_SecondsPerCount;

	// Prepare for next frame.
	m_PreviousTime = m_CurrentTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if(m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}