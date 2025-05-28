#define DLL_EXPORT
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"

#include "SSEngineDefault/Private/PWin32/RawInput/Win32SSRawInputProcessor.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSFrameInfo.h"
#include "SSEngineDefault/Public/SSContainer/SHasherW.h"

SS::SHashPoolNode* g_SHasherPool = nullptr;
uint32 g_sHasherPoolCnt = 0;
SSFrameInfo* g_FrameInfo = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;

void SSEngineDefaultModuleEntry(
	SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	SSFrameInfo* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor)
{
	g_SHasherPool = InHasherPool;
	g_sHasherPoolCnt = InHasherPoolCnt;
	g_FrameInfo = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
}

SSRawInputProcessorBase* CreateInputProcessor()
{
	Win32SSRawInputProcessor* InputProcessor = DBG_NEW Win32SSRawInputProcessor();
	return InputProcessor;
}

SSFrameInfo* CreateFrameInfo()
{
	SSFrameInfo* FrameInfo = DBG_NEW SSFrameInfo();
	return FrameInfo;
}
