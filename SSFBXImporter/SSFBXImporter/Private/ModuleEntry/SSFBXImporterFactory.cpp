#define SSFBXIMPORTER_MODULE_EXPORT
#include "SSFBXImporter/Public/ModuleEntry/SSFBXImporterFactory.h"

#include "SSFBXImporter/Private/SSFBXImporter.h"


IHasherPool* g_HasherPool = nullptr;
IRenderer* g_Renderer = nullptr;

ISSFBXImporter* CreateSSFBXImporter()
{
	return DBG_NEW SSFBXImporter();
}

void SSFBXImporterModuleEntry(IHasherPool* InHasherPool, IRenderer* InRenderer)
{
	g_HasherPool = InHasherPool;
	g_Renderer = InRenderer;
}
