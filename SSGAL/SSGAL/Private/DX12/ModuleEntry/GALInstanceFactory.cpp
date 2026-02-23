#include "pch.h"
#define SSGAL_MODULE_EXPORT

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "Private/DX12/GALRenderTarget/DX12GALSwapChainRenderTarget.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"

IHasherPool* g_HasherPool = nullptr;
IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IThreadManager* g_ThreadManager = nullptr;

ICommonRenderAssetSet* g_RenderAssetSet = nullptr;



GALRenderTarget* CreateGALSwapChain(GALRenderDeviceContext* DeviceContext, HWND InhWnd)
{
	DX12GALRenderDeviceContext* DX12GalRenderDeviceContext = static_cast<DX12GALRenderDeviceContext*>(DeviceContext);
	DX12GALRenderDevice* DX12GalRenderDevice= static_cast<DX12GALRenderDevice*>(DeviceContext->GetOwnerRenderDevice());
	GALRenderTarget* NewSwapChain = DBG_NEW DX12GALSwapChainRenderTarget(
		DX12GalRenderDeviceContext,
		InhWnd,
		DX12GalRenderDevice->GetDXGIFactory());

	return NewSwapChain;
}

GALRenderDevice* CreateGALRenderDevice(
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion)
{
	DX12GALRenderDevice* NewRenderDevice = DBG_NEW DX12GALRenderDevice(EnableDebugLayer, EnableGPUBaseValidataion);

	return NewRenderDevice;
}

void SSGALModuleEntry(
	IHasherPool* InHasherPool,
	IFrameInfoProcessor* InFrameInfoProcessor,
	IThreadManager* InThreadManager)
{
	g_HasherPool = InHasherPool;
	g_FrameInfoProcessor = InFrameInfoProcessor;
	g_ThreadManager = InThreadManager;
}
