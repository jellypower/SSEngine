#define DLL_EXPORT
#include "SSEngineDefault/Public/ProfilerUtils.h"

#include <Windows.h>

uint64 GetPerofrmanceCounter()
{
	LARGE_INTEGER newTickLI;
	QueryPerformanceCounter(&newTickLI);
	return newTickLI.QuadPart;
}

uint64 GetPerformanceFrequency()
{
	LARGE_INTEGER freqLI;
	QueryPerformanceFrequency(&freqLI);
	return freqLI.QuadPart;
}
