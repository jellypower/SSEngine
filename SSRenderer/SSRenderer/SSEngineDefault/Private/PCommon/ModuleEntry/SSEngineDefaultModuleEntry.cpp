#include "SSEngineDefault/Private/PCommon/ModuleEntry/SSEngineDefaultModuleEntry.h"

#include "SSEngineDefault/Public/SSFrameInfo.h"
#include "SSEngineDefault/Public/SSContainer/SHasherW.h"

SS::SHashPoolNode* g_SHasherPool = nullptr;
uint32 g_sHasherPoolCnt = 0;
SSFrameInfo* g_FrameInfo = nullptr;

void SSEngineDefaultModuleEntry(
	SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	SSFrameInfo* InFrameInfo)
{
	g_SHasherPool = InHasherPool;
	g_sHasherPoolCnt = InHasherPoolCnt;
	g_FrameInfo = InFrameInfo;
}