#include "SSRenderer/Public/ModuleEntry/SSRendererFactory.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"


#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"


extern SSRenderer* g_Renderer = nullptr;


SSRenderer* CreateRenderer(GALRenderDevice* InRenderDevice)
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
