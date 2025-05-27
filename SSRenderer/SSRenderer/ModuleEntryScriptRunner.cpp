#include "ModuleEntryScriptRunner.h"

#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"
#include "SSEngineDefault/Private/PCommon/ModuleEntry/SSEngineDefaultModuleEntry.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/SSFrameInfo.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/SSContainer/SHasherW.h"

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
	SS::SHashPoolNode* NewHsherPool = DBG_NEW SS::SHashPoolNode[SHASHER_DEFAULT_POOL_SIZE];
	SSFrameInfo* NewFrameInfo = DBG_NEW SSFrameInfo();


	SSEngineDefaultModuleEntry(
		NewHsherPool,
		SHASHER_DEFAULT_POOL_SIZE,
		NewFrameInfo);

	SObjectModuleEntry(
		NewHsherPool,
		SHASHER_DEFAULT_POOL_SIZE,
		NewFrameInfo);

}

void RunModuleExitScript()
{
	CleanupSObjSystem();
	CleanupSHasher();

	delete g_FrameInfo;
	g_FrameInfo = nullptr;
}
