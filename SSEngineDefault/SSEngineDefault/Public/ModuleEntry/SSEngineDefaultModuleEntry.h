#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class SSRawInputProcessorBase;
class FrameInfoProcessorBase;

namespace SS
{
	class SHashPoolNode;
}



SSENGINEDEFAULT_MODULE void SSEngineDefaultModuleEntry(
	SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	FrameInfoProcessorBase* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor);

SSENGINEDEFAULT_MODULE SSRawInputProcessorBase* CreateInputProcessor();
SSENGINEDEFAULT_MODULE FrameInfoProcessorBase* CreateFrameInfo();