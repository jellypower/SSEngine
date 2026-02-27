#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/ModuleEntry/SSContentsBaseModuleEntry.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

IRenderer* g_Renderer = nullptr;

IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IRawInputProcessor* g_RawInputProcessor = nullptr;
IThreadManager* g_ThreadManager = nullptr;

void SSContentsBaseModuleEntry(
	IRenderer* InRenderer,
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor,
	IThreadManager* InThreadManager)
{
	g_Renderer = InRenderer;
	g_FrameInfoProcessor = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
	g_ThreadManager = InThreadManager;
}
