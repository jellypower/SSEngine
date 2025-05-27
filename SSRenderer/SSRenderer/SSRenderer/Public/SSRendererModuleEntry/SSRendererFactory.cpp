#include "SSRendererFactory.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"


#include "SSGAL/Public/SSGALModuleEntry/GPUAssetInstanceFactory.h"
#include "SSRenderer/Private/SSRendererGlobalVariablePrivate.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

SSRenderer* CreateRenderer(HINSTANCE InhInst, HWND InhWnd)
{
	GALRenderDevice* NewRenderDevice = nullptr;
	GALRenderDeviceContext* MainDeviceContext = nullptr;
	SSRenderer* newRenderer = nullptr;


	newRenderer = DBG_NEW SSRenderer();


	NewRenderDevice = CreateGALRenderDevice(
		newRenderer,
		InhInst,
		InhWnd,
		ENABLE_DEBUG_LAYER,
		ENABLE_GPU_BASE_VALIDATIION);
	if (NewRenderDevice == nullptr)
	{
		SS_ASSERT(false);
		goto lb_error;
	}
	newRenderer->_GALRenderDevice = NewRenderDevice;


	MainDeviceContext = NewRenderDevice->CreateRenderDeviceContext();
	if (MainDeviceContext == nullptr)
	{
		SS_ASSERT(false);
		goto lb_error;
	}
	newRenderer->_MainDeviceContext = MainDeviceContext;


	SSRendererModule::Private::g_Renderer = newRenderer;
	return newRenderer;

lb_error:
	if (newRenderer != nullptr)
	{
		delete newRenderer;
	}

	if (NewRenderDevice != nullptr)
	{
		delete NewRenderDevice;
	}

	if (MainDeviceContext != nullptr)
	{
		delete MainDeviceContext;
	}

	return nullptr;
}
