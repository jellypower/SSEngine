#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"

#include <Windows.h>

#include "SSEngineDefault/Public/SSNativeKeywords.h"

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
