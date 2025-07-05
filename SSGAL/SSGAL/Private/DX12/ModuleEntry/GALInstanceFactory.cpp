#define SSGAL_MODULE_EXPORT
#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

GALRenderDevice* CreateGALRenderDevice(
	HINSTANCE InhInst,
	HWND InhWnd,
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion)
{
	DX12GALRenderDevice* NewRenderDevice = DBG_NEW DX12GALRenderDevice(
		InhInst, InhWnd, EnableDebugLayer, EnableGPUBaseValidataion);


	PSOPool* PSOPool = NewRenderDevice->GetPSOPool();
	{
		// TEMP_InitializePSOPool
	}


	return NewRenderDevice;
}