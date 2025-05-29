#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"


// TODO: 25/05/26 주석 풀기
//SSFrameInfo* g_FrameInfo = nullptr;
SObjectGlobalHashMap* g_ObjectHashMap = nullptr;

void SObjectModuleEntry(
	FrameInfoProcessorBase* InFrameInfoProcessor)
{
	if (g_ObjectHashMap != nullptr)
	{
		SS_INTERRUPT(L"g_ObjectHashMap already initialized.");
		return;
	}

	g_ObjectHashMap = DBG_NEW SObjectGlobalHashMap();

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