#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

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


	PSOPool* PSOPool = NewRenderDevice->GetPSOPool();
	{
		// TEMP_InitializePSOPool
	}


	return NewRenderDevice;
}