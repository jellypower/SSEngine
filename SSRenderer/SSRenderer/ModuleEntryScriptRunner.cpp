#include "ModuleEntryScriptRunner.h"

#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"
#include "SSEngineDefault/Public/RawInput/SSRawInputProcessorBase.h"
#include "SSEngineDefault/Public/RawProfiler/FrameInfoProcessorBase.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"


IHasherPool* g_HasherPool = nullptr;
FrameInfoProcessorBase* g_FrameInfoProcessor = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;


void RunModuleEntryScript()
{
	g_HasherPool = CreateHasherPool(SHASHER_DEFAULT_POOL_SIZE);
	g_FrameInfoProcessor = CreateFrameInfo();
	g_RawInputProcessor = CreateInputProcessor();


	SSEngineDefaultModuleEntry(
		SHASHER_DEFAULT_POOL_SIZE,
		g_HasherPool,
		g_FrameInfoProcessor,
		g_RawInputProcessor);

	SObjectModuleEntry(
		g_FrameInfoProcessor);

}

void RunModuleExitScript()
{
	CleanupSObjSystem();

	delete g_RawInputProcessor;
	g_RawInputProcessor = nullptr;

	delete g_FrameInfoProcessor;
	g_FrameInfoProcessor = nullptr;

	delete g_HasherPool;
	g_HasherPool = nullptr;
}
