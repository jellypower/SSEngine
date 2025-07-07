#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/ModuleEntry/SSContentsBaseModuleEntry.h"

IRenderer* g_Renderer = nullptr;

IHasherPool* g_HasherPool = nullptr;
FrameInfoProcessorBase* g_FrameInfoProcessor = nullptr;
SSRawInputProcessorBase* g_RawInputProcessor = nullptr;


void SSContentsBaseModuleEntry(IRenderer* InRenderer, IHasherPool* InHasherPool, FrameInfoProcessorBase* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor)
{
	g_Renderer = InRenderer;
	g_HasherPool = InHasherPool;
	g_FrameInfoProcessor = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
}
