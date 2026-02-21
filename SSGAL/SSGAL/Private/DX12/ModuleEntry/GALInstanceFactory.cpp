#include "pch.h"
#define SSGAL_MODULE_EXPORT

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

IHasherPool* g_HasherPool = nullptr;
IThreadManager* g_ThreadManager = nullptr;

ICommonRenderAssetSet* g_RenderAssetSet = nullptr;

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

void SSGALModuleEntry(
	IHasherPool* InHasherPool,
	IThreadManager* InThreadManager)
{
	g_HasherPool = InHasherPool;
	g_ThreadManager = InThreadManager;
}
