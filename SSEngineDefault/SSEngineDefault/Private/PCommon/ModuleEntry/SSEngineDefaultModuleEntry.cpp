#define DLL_EXPORT
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"

#include "SSEngineDefault/Private/PWin32/RawInput/Win32SSRawInputProcessor.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSContainer/SHasherW.h"

SS::SHashPoolNode* g_SHasherPool = nullptr;
uint32 g_sHasherPoolCnt = 0;
FrameInfoProcessorBase* g_FrameInfoProcessor = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;

void SSEngineDefaultModuleEntry(
	SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	FrameInfoProcessorBase* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor)
{
	g_SHasherPool = InHasherPool;
	g_sHasherPoolCnt = InHasherPoolCnt;
	g_FrameInfoProcessor = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
}

SSRawInputProcessorBase* CreateInputProcessor()
{
	Win32SSRawInputProcessor* InputProcessor = DBG_NEW Win32SSRawInputProcessor();
	return InputProcessor;
}

FrameInfoProcessorBase* CreateFrameInfo()
{
	FrameInfoProcessorBase* FrameInfo = DBG_NEW FrameInfoProcessorBase();
	return FrameInfo;
}
