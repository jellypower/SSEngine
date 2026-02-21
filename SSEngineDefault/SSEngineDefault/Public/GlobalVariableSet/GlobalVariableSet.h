#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IThreadManager;
class IHasherPool;
class IRawInputProcessor;
class IFrameInfoProcessor;


extern IHasherPool* g_HasherPool;
extern IFrameInfoProcessor* g_FrameInfoProcessor;
extern IRawInputProcessor* g_RawInputProcessor;
extern IThreadManager* g_ThreadManager;