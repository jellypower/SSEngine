#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"


SObjectGlobalHashMap* g_ObjectHashMap = nullptr;
IHasherPool* g_HasherPool = nullptr;
FrameInfoProcessorBase* g_FrameInfoProcessor = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;

SObjectGlobalHashMap* CreateSObjectGlobalHashMap()
{
	SObjectGlobalHashMap* NewSObjectHashMap = DBG_NEW SObjectGlobalHashMap();
	return NewSObjectHashMap;
}

void SObjectModuleEntry(
	SObjectGlobalHashMap* InSObjectHashMap, 
	IHasherPool* InHasherPool)
{
	g_ObjectHashMap = InSObjectHashMap;
	g_HasherPool = InHasherPool;
}