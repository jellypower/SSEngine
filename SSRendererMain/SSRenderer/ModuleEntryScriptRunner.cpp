#include "ModuleEntryScriptRunner.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"

#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"
#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"
#include "SSEngineDefault/Public/RawInput/SSRawInputProcessorBase.h"
#include "SSEngineDefault/Public/RawProfiler/FrameInfoProcessorBase.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"


SObjectGlobalHashMap* g_ObjectHashMap = nullptr;

IHasherPool* g_HasherPool = nullptr;
FrameInfoProcessorBase* g_FrameInfoProcessor = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;


void RunModuleEntryScript()
{
	g_HasherPool = CreateHasherPool(SHASHER_DEFAULT_POOL_SIZE);
	g_FrameInfoProcessor = CreateFrameInfo();
	g_RawInputProcessor = CreateInputProcessor();
	g_ObjectHashMap = CreateSObjectGlobalHashMap();


	SSEngineDefaultModuleEntry(
		SHASHER_DEFAULT_POOL_SIZE,
		g_HasherPool,
		g_FrameInfoProcessor,
		g_RawInputProcessor);

	SObjectModuleEntry(
		g_ObjectHashMap,
		g_HasherPool);

}

void RunModuleExitScript()
{
	delete g_ObjectHashMap;
	g_ObjectHashMap = nullptr;

	delete g_RawInputProcessor;
	g_RawInputProcessor = nullptr;

	delete g_FrameInfoProcessor;
	g_FrameInfoProcessor = nullptr;

	delete g_HasherPool;
	g_HasherPool = nullptr;
}
