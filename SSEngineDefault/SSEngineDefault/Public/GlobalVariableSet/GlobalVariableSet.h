#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IHasherPool;
class SSRawInputProcessorBase;
class FrameInfoProcessorBase;


extern IHasherPool* g_HasherPool;
extern FrameInfoProcessorBase* g_FrameInfoProcessor;
extern SSRawInputProcessorBase* g_RawInputProcessor;