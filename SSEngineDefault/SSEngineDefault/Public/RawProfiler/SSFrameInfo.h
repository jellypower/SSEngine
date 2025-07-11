#pragma once
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"
#include "SSEngineDefault/Public/SSNativeTypes.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

class SSFrameInfo
{
public:
	static double GetDeltaTime() { return g_FrameInfoProcessor->GetDeltaTime(); }
	static double GetFPS() { return g_FrameInfoProcessor->GetFPS(); }
	static double GetElapsedTime() { return g_FrameInfoProcessor->GetElapsedTime(); }
	static Vector2ui32 GetWindowSize() { return g_FrameInfoProcessor->GetWindowSize(); }
	static uint64 GetFrameCnt() { return g_FrameInfoProcessor->GetFrameCnt(); }
};
