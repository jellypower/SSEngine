#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/GlobalVariableSet.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"


// TODO: 25/05/26 주석 풀기
//SS::SHashPoolNode* g_SHasherPool = nullptr;
//uint32 g_sHasherPoolCnt = 0;
//SSFrameInfo* g_FrameInfo = nullptr;
SObjectGlobalHashMap* g_ObjectHashMap = nullptr;

void SObjectModuleEntry(SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	SSFrameInfo* InFrameInfo)
{
	if (g_ObjectHashMap != nullptr)
	{
		SS_INTERRUPT(L"g_ObjectHashMap already initialized.");
		return;
	}

	g_ObjectHashMap = DBG_NEW SObjectGlobalHashMap();

	g_SHasherPool = InHasherPool;
	g_sHasherPoolCnt = InHasherPoolCnt;
	g_FrameInfo = InFrameInfo;
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