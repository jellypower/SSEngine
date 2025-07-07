#define SSRENDERER_MODULE_EXPORT
#include "SSRenderer/Public/ModuleEntry/SSRendererFactory.h"

#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"


#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"
#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"


extern IRenderer* g_Renderer = nullptr;
IHasherPool* g_HasherPool = nullptr;

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

void SSRendererModuleEntry(IHasherPool* InHasherPool)
{
	g_HasherPool = InHasherPool;
}
