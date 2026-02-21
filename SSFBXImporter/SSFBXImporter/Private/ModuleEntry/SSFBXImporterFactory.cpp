#include "pch.h"
#define SSFBXIMPORTER_MODULE_EXPORT
#include "SSFBXImporter/Public/ModuleEntry/SSFBXImporterFactory.h"

#include "SSFBXImporter/Private/SSFBXImporter.h"


IHasherPool* g_HasherPool = nullptr;
IThreadManager* g_ThreadManager = nullptr;

IRenderer* g_Renderer = nullptr;

ISSFBXImporter* CreateSSFBXImporter()
{
	return DBG_NEW SSFBXImporter();
}

void SSFBXImporterModuleEntry(
	IHasherPool* InHasherPool,
	IThreadManager* InThreadManager, 
	IRenderer* InRenderer)
{
	g_HasherPool = InHasherPool;
	g_ThreadManager = InThreadManager;
	g_Renderer = InRenderer;
}