#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"

#include "SSEngineDefault/Private/PCommon/RawProfiler/FrameInfoProcessorBase.h"
#include "SSEngineDefault/Private/PCommon/SHasher/HasherPoolBase.h"
#include "SSEngineDefault/Private/PWin32/RawInput/Win32SSRawInputProcessor.h"
#include "SSEngineDefault/Private/PWin32/SSThread/Win32ThreadManager.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"


IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IRawInputProcessor* g_RawInputProcessor = nullptr;
IThreadManager* g_ThreadManager = nullptr;

void SSEngineDefaultModuleEntry(
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor,
	IThreadManager* InThreadManager)
{
	g_FrameInfoProcessor = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
	g_ThreadManager = InThreadManager;
}

IRawInputProcessor* CreateInputProcessor()
{
	Win32SSRawInputProcessor* InputProcessor = DBG_NEW Win32SSRawInputProcessor();
	return InputProcessor;
}

IFrameInfoProcessor* CreateFrameInfo()
{
	IFrameInfoProcessor* FrameInfo = DBG_NEW FrameInfoProcessorBase();
	return FrameInfo;
}

IThreadManager* CreateThreadManager()
{
	Win32ThreadManager* ThreadManager = DBG_NEW Win32ThreadManager();
	return ThreadManager;
}
