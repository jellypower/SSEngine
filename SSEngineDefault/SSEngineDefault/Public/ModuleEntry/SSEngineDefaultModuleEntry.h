#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class SSFrameInfo;

namespace SS
{
	class SHashPoolNode;
}



SSENGINEDEFAULT_MODULE void SSEngineDefaultModuleEntry(
	SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	SSFrameInfo* InFrameInfo);