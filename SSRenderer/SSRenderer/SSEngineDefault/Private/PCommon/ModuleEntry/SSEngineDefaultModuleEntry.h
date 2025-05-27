#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class SSFrameInfo;

namespace SS
{
	class SHashPoolNode;
}



void SSEngineDefaultModuleEntry(
	SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	SSFrameInfo* InFrameInfo);