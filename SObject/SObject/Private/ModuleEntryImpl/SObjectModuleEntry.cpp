#define SOBJECT_MODULE_EXPORT
#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"




SObjectGlobalHashMap* g_ObjectHashMap = nullptr;
IThreadManager* g_ThreadManager = nullptr;

SObjectGlobalHashMap* CreateSObjectGlobalHashMap()
{
	SObjectGlobalHashMap* NewSObjectHashMap = DBG_NEW SObjectGlobalHashMap();
	return NewSObjectHashMap;
}

void SObjectModuleEntry(
	SObjectGlobalHashMap* InSObjectHashMap, 
	IThreadManager* InThreadManager)
{
	g_ObjectHashMap = InSObjectHashMap;
	g_ThreadManager = InThreadManager;
}