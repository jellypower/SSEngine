#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

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