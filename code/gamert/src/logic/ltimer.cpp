#include "ltimer.hpp"

#if defined(WIN32) || defined(_WIN32)

LTimer::LTimer()
	: _snapshot(0)
	, _freq(0)
{
	LARGE_INTEGER freq;
	if (!QueryPerformanceFrequency(&freq))
	{
		GRT_CHECK(false, "failed to query performance frequency.");
	}
	_freq = freq.QuadPart;
}

LTimer::~LTimer()
{}

void LTimer::snapshot()
{
	LARGE_INTEGER lc;
	QueryPerformanceCounter(&lc);
	
	_snapshot = lc.QuadPart;
}

float LTimer::elapsed()
{
	LARGE_INTEGER lc;
	QueryPerformanceCounter(&lc);

	double e = ((double)(lc.QuadPart - _snapshot) / (double)_freq) * 1000.f;

	return (float)e;
}

#endif
