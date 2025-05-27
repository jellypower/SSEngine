#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

class SSFrameInfo;
class SObjectGlobalHashMap;

void SObjectModuleEntry(SS::SHashPoolNode* InHasherPool,
	uint32 InHasherPoolCnt,
	SSFrameInfo* InFrameInfo);
void CheckLiveSobjects();
void CleanupSObjSystem();