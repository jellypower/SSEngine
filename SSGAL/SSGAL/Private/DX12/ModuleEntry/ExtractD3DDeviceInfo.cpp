#define SSGAL_MODULE_EXPORT
#include "pch.h"
#include "SSGAL/Public/ModuleEntry/ExtractD3DDeviceInfo.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderTarget/DX12GALSwapChainRenderTarget.h"

void ExtractD3DDeviceInfo(GALRenderDevice* DeviceToExtract, ID3D12Device** OutDevice,
                          ID3D12CommandQueue** OutCommandQueue, int32* OutNestedFrameCnt)
{
	DX12GALRenderDevice* DX12GALDevice = (DX12GALRenderDevice*)DeviceToExtract;

	*OutDevice = DX12GALDevice->GetD3DDevice();
	*OutCommandQueue = DX12GALDevice->GetD3DCommandQueue();
	*OutNestedFrameCnt = SWAP_CHAIN_FRAME_COUNT;
}

void QueryCurrentD3DGALDeviceContext(GALRenderDevice* DeviceToQuery, ID3D12Resource** OutBackBuffer,
	D3D12_CPU_DESCRIPTOR_HANDLE* OutBackBufferDescHeap)
{
	DX12GALRenderDevice* DX12GALDevice = (DX12GALRenderDevice*)DeviceToQuery;
	DX12GALSwapChainRenderTarget* SwapChainRT = (DX12GALSwapChainRenderTarget*)DX12GALDevice->GetDefaultViewportRenderTarget();

	*OutBackBufferDescHeap = SwapChainRT->GetCurrentDescHandle();
	*OutBackBuffer = SwapChainRT->GetCurrentResource();
}
