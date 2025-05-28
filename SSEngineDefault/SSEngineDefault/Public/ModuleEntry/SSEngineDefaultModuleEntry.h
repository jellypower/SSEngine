#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IHasherPool;
class SSRawInputProcessorBase;
class FrameInfoProcessorBase;

namespace SS
{
	class SHashPoolNode;
}



SSENGINEDEFAULT_MODULE_NATIVE void SSEngineDefaultModuleEntry(
	SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	FrameInfoProcessorBase* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor);

SSENGINEDEFAULT_MODULE_NATIVE SSRawInputProcessorBase* CreateInputProcessor();
SSENGINEDEFAULT_MODULE_NATIVE FrameInfoProcessorBase* CreateFrameInfo();
SSENGINEDEFAULT_MODULE_NATIVE IHasherPool* CreateHasherPool(int32 InBucketCnt);