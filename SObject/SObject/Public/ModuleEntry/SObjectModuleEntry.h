#pragma once
#include "SObject/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

class SSFrameInfo;
class SObjectGlobalHashMap;

SOBJECT_MODULE_NATIVE SObjectGlobalHashMap* CreateSObjectGlobalHashMap();
SOBJECT_MODULE_NATIVE void SObjectModuleEntry(
	SObjectGlobalHashMap* InSObjectHashMap,
	IThreadManager* InThreadManager);
