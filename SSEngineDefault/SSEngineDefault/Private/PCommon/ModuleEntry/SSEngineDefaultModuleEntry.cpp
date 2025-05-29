#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"

#include "SSEngineDefault/Private/PCommon/SHasher/HasherPoolBase.h"
#include "SSEngineDefault/Private/PWin32/RawInput/Win32SSRawInputProcessor.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"


IHasherPool* g_HasherPool = nullptr;
FrameInfoProcessorBase* g_FrameInfoProcessor = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;

void SSEngineDefaultModuleEntry(
	uint32 InHasherPoolCnt,
	IHasherPool* InHasherPool,
	FrameInfoProcessorBase* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor)
{
	g_HasherPool = InHasherPool;
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

IHasherPool* CreateHasherPool(int32 InBucketCnt)
{
	HasherPoolBase* NewHasherPool = DBG_NEW HasherPoolBase(InBucketCnt);
	return NewHasherPool;
}
