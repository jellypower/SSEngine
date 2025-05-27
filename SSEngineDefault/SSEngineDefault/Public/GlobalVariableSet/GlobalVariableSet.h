#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class SSFrameInfo;
namespace SS
{
	class SHashPoolNode;
}



extern SS::SHashPoolNode* g_SHasherPool;
extern uint32 g_sHasherPoolCnt;
extern SSFrameInfo* g_FrameInfo;