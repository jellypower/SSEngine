#pragma once
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"
#include "SSEngineDefault/Public/SSNativeTypes.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

class SSFrameInfo
{
public:
	static double GetDeltaTime() { return g_FrameInfoProcessor->GetDeltaTime(); }
	static double GetSmoothDeltaTime() { return g_FrameInfoProcessor->GetSmoothDeltaTime(); }
	static double GetElapsedTime() { return g_FrameInfoProcessor->GetElapsedTime(); }
	static uint64 GetFrameCnt() { return g_FrameInfoProcessor->GetFrameCnt(); }
};
