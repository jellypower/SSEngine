#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IHasherPool;
class SSRawInputProcessorBase;
class FrameInfoProcessorBase;



SSENGINEDEFAULT_MODULE_NATIVE void SSEngineDefaultModuleEntry(
	uint32 InHasherPoolCnt,
	IHasherPool* InHasherPool,
	FrameInfoProcessorBase* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor);

SSENGINEDEFAULT_MODULE_NATIVE SSRawInputProcessorBase* CreateInputProcessor();
SSENGINEDEFAULT_MODULE_NATIVE FrameInfoProcessorBase* CreateFrameInfo();
SSENGINEDEFAULT_MODULE_NATIVE IHasherPool* CreateHasherPool(int32 InBucketCnt);