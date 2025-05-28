#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"


// SSEngineDefault Module
SS::SHashPoolNode* g_SHasherPool = nullptr;
uint32 g_sHasherPoolCnt = 0;
FrameInfoProcessorBase* g_FrameInfoProcessor = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;

// SObject Module
SObjectGlobalHashMap* g_ObjectHashMap;

void SObjectModuleEntry(SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	FrameInfoProcessorBase* InFrameInfoProcessor)
{
	if (g_ObjectHashMap != nullptr)
	{
		SS_INTERRUPT(L"g_ObjectHashMap already initialized.");
		return;
	}

	g_ObjectHashMap = DBG_NEW SObjectGlobalHashMap();

	g_SHasherPool = InHasherPool;
	g_sHasherPoolCnt = InHasherPoolCnt;
	g_FrameInfoProcessor = InFrameInfoProcessor;
}

void CheckLiveSobjects()
{
	bool AnyAlive = g_ObjectHashMap->AnySObjectAlive();
	SS_ASSERT(AnyAlive == false);
}

void CleanupSObjSystem()
{
	CheckLiveSobjects();
	delete g_ObjectHashMap;
	g_ObjectHashMap = nullptr;
}