#include "GPUAssetInstanceFactory.h"

#include "SSGAL/Private/GALInstanceGlobalVariablePrivate.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

GALRenderDevice* CreateGALRenderDevice(
	SSRenderer* Renderer, 
	HINSTANCE InhInst,
	HWND InhWnd,
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion)
{
	DX12GALRenderDevice* NewRenderDevice = DBG_NEW DX12GALRenderDevice(
		InhInst, InhWnd, Renderer, EnableDebugLayer, EnableGPUBaseValidataion);

	NewRenderDevice->TEMP_InitializePSOInstances();


	return NewRenderDevice;
}

void InjectRendererToGALModule(SSRenderer* InRenderer)
{
	SSGALModule::Private::g_Renderer = InRenderer;
}
