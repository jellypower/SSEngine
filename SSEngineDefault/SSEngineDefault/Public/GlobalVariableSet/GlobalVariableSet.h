#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class SSRawInputProcessorBase;
class FrameInfoProcessorBase;
namespace SS
{
	class SHashPoolNode;
}



extern SS::SHashPoolNode* g_SHasherPool;
extern uint32 g_sHasherPoolCnt;
extern FrameInfoProcessorBase* g_FrameInfoProcessor;
extern SSRawInputProcessorBase* g_RawInputProcessor;