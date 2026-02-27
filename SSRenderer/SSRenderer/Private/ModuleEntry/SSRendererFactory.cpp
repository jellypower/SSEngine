#define SSRENDERER_MODULE_EXPORT
#include "SSRenderer/Public/ModuleEntry/SSRendererFactory.h"

#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"


#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"
#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"


IRenderer* g_Renderer = nullptr;

IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IThreadManager* g_ThreadManager = nullptr;


IRenderer* CreateRenderer(GALRenderDevice* InRenderDevice)
{
	if (InRenderDevice == nullptr)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	GALRenderDeviceContext* MainDeviceContext = nullptr;
	SSRenderer* newRenderer = nullptr;


	newRenderer = DBG_NEW SSRenderer(InRenderDevice);
	InRenderDevice->BindRendererXXX(newRenderer);


	g_Renderer = newRenderer;
	return newRenderer;
}

void SSRendererModuleEntry(
	IFrameInfoProcessor* InFrameInfoProcessor,
	IThreadManager* InThreadManager)
{
	g_FrameInfoProcessor = InFrameInfoProcessor;
	g_ThreadManager = InThreadManager;
}
