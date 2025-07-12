#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"

#include "SSEngineDefault/Private/PCommon/RawProfiler/FrameInfoProcessorBase.h"
#include "SSEngineDefault/Private/PCommon/SHasher/HasherPoolBase.h"
#include "SSEngineDefault/Private/PWin32/RawInput/Win32SSRawInputProcessor.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"


IHasherPool* g_HasherPool = nullptr;
IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IRawInputProcessor* g_RawInputProcessor = nullptr;

void SSEngineDefaultModuleEntry(
	uint32 InHasherPoolCnt,
	IHasherPool* InHasherPool,
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor)
{
	g_HasherPool = InHasherPool;
	g_FrameInfoProcessor = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
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

IHasherPool* CreateHasherPool(int32 InBucketCnt)
{
	HasherPoolBase* NewHasherPool = DBG_NEW HasherPoolBase(InBucketCnt);
	return NewHasherPool;
}
