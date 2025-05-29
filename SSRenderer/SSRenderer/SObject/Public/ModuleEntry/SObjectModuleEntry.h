#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

class SSFrameInfo;
class SObjectGlobalHashMap;

void SObjectModuleEntry(
	FrameInfoProcessorBase* InFrameInfoProcessor);
void CheckLiveSobjects();
void CleanupSObjSystem();