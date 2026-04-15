#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IWindowManager;
class IThreadManager;
class IHasherPool;
class IRawInputProcessor;
class IFrameInfoProcessor;

extern IWindowManager* g_MainWindowManager;
extern IFrameInfoProcessor* g_FrameInfoProcessor;
extern IRawInputProcessor* g_RawInputProcessor;
extern IThreadManager* g_ThreadManager;