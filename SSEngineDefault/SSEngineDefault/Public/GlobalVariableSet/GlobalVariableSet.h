#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IHasherPool;
class SSRawInputProcessorBase;
class IFrameInfoProcessor;


extern IHasherPool* g_HasherPool;
extern IFrameInfoProcessor* g_FrameInfoProcessor;
extern SSRawInputProcessorBase* g_RawInputProcessor;