#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/ModuleEntry/SSContentsBaseModuleEntry.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

IRenderer* g_Renderer = nullptr;

IHasherPool* g_HasherPool = nullptr;
IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IRawInputProcessor* g_RawInputProcessor = nullptr;


void SSContentsBaseModuleEntry(IRenderer* InRenderer, IHasherPool* InHasherPool, IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor)
{
	g_Renderer = InRenderer;
	g_HasherPool = InHasherPool;
	g_FrameInfoProcessor = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
}
