#include "pch.h"
#define SSCOLLISION_MODULE_EXPORT
#include "SSCollision/Public/ModuleEntry/SSCollisionModuleEntry.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"



IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IThreadManager* g_ThreadManager = nullptr;

void SSCollisionModuleEntry(IFrameInfoProcessor* InFrameInfo, IThreadManager* InThreadManager)
{
	g_FrameInfoProcessor = InFrameInfo;
	g_ThreadManager = InThreadManager;
}
