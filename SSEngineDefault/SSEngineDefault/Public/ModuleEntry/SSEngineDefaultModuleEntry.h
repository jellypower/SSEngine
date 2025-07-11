#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IFrameInfoProcessor;
class IHasherPool;
class SSRawInputProcessorBase;



SSENGINEDEFAULT_MODULE_NATIVE void SSEngineDefaultModuleEntry(
	uint32 InHasherPoolCnt,
	IHasherPool* InHasherPool,
	IFrameInfoProcessor* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor);

SSENGINEDEFAULT_MODULE_NATIVE SSRawInputProcessorBase* CreateInputProcessor();
SSENGINEDEFAULT_MODULE_NATIVE IFrameInfoProcessor* CreateFrameInfo();
SSENGINEDEFAULT_MODULE_NATIVE IHasherPool* CreateHasherPool(int32 InBucketCnt);