#include "ModuleEntryScriptRunner.h"

#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/RawInput/SSRawInputProcessorBase.h"
#include "SSEngineDefault/Public/RawProfiler/FrameInfoProcessorBase.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"


SS::SHashPoolNode* g_SHasherPool = nullptr;
uint32 g_sHasherPoolCnt = 0;
FrameInfoProcessorBase* g_FrameInfoProcessor = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;

void CleanupSHasher()
{

	for (uint32 i = 0; i < g_sHasherPoolCnt; i++)
	{
		if (g_SHasherPool[i]._str != nullptr)
		{
			SS::SHashPoolNode* curNode = &g_SHasherPool[i];
			delete curNode->_str;

			curNode = curNode->_next;
			while (curNode != nullptr)
			{
				SS::SHashPoolNode* nextNode = curNode->_next;
				delete curNode->_str;
				delete curNode;
				curNode = nextNode;
			}
		}
	}

	delete g_SHasherPool;
	g_SHasherPool = nullptr;
}

void RunModuleEntryScript()
{
	g_SHasherPool = DBG_NEW SS::SHashPoolNode[SHASHER_DEFAULT_POOL_SIZE];
	g_sHasherPoolCnt = SHASHER_DEFAULT_POOL_SIZE;
	g_FrameInfoProcessor = CreateFrameInfo();
	g_RawInputProcessor = CreateInputProcessor();


	SSEngineDefaultModuleEntry(
		g_SHasherPool,
		SHASHER_DEFAULT_POOL_SIZE,
		g_FrameInfoProcessor,
		g_RawInputProcessor);

	SObjectModuleEntry(
		g_SHasherPool,
		SHASHER_DEFAULT_POOL_SIZE,
		g_FrameInfoProcessor);

}

void RunModuleExitScript()
{
	CleanupSObjSystem();
	CleanupSHasher();

	delete g_RawInputProcessor;
	g_RawInputProcessor = nullptr;

	delete g_FrameInfoProcessor;
	g_FrameInfoProcessor = nullptr;
}
