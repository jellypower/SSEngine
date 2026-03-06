#include "pch.h"
#define SSFBXIMPORTER_MODULE_EXPORT
#include "SSFBXImporter/Public/ModuleEntry/SSFBXImporterFactory.h"

#include "SSFBXImporter/Private/SSFBXImporter.h"


IThreadManager* g_ThreadManager = nullptr;
IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;

ISSFBXImporter* CreateSSFBXImporter()
{
	return DBG_NEW SSFBXImporter();
}

void SSFBXImporterModuleEntry(
	IThreadManager* InThreadManager, 
	IFrameInfoProcessor* InFrameInfoProcessor)
{
	g_ThreadManager = InThreadManager;
	g_FrameInfoProcessor = InFrameInfoProcessor;
}