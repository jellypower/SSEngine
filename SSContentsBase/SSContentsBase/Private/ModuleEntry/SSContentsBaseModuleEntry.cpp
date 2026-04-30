#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/ModuleEntry/SSContentsBaseModuleEntry.h"

#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"

IRenderer* g_Renderer = nullptr;

ICollDevice* g_CollDevice = nullptr;

IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IRawInputProcessor* g_RawInputProcessor = nullptr;
IThreadManager* g_ThreadManager = nullptr;

void SSContentsBaseModuleEntry(
	IRenderer* InRenderer,
	ICollDevice* InCollDevice,
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor,
	IThreadManager* InThreadManager)
{
	g_Renderer = InRenderer;

	g_CollDevice = InCollDevice;

	g_FrameInfoProcessor = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
	g_ThreadManager = InThreadManager;
}
