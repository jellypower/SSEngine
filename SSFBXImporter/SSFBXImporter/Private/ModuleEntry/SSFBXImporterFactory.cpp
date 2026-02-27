#include "pch.h"
#define SSFBXIMPORTER_MODULE_EXPORT
#include "SSFBXImporter/Public/ModuleEntry/SSFBXImporterFactory.h"

#include "SSFBXImporter/Private/SSFBXImporter.h"


IThreadManager* g_ThreadManager = nullptr;

IRenderer* g_Renderer = nullptr;

ISSFBXImporter* CreateSSFBXImporter()
{
	return DBG_NEW SSFBXImporter();
}

void SSFBXImporterModuleEntry(
	IThreadManager* InThreadManager, 
	IRenderer* InRenderer)
{
	g_ThreadManager = InThreadManager;
	g_Renderer = InRenderer;
}