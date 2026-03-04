#pragma once
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSGAL/Public/SSGALInlineSettings.h"



class RenderFrameInfo
{
public:
	static uint64 GetFrameMod() { return SSFrameInfo::GetFrameCnt() % GAL_NESTED_FRAME_CNT; }
};
