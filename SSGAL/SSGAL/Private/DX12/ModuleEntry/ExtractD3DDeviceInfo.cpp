#include "pch.h"

#define SSGAL_MODULE_EXPORT
#include "SSGAL/Public/ModuleEntry/ExtractD3DDeviceInfo.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "Private/DX12/GALRenderTarget/DX12GALSwapChainRenderTarget.h"

void QueryDX12GALDeviceContextInfo(GALRenderDeviceContext* DeviceToQuery, ID3D12Device** OutDevice,
                          ID3D12CommandQueue** OutCommandQueue, int32* OutNestedFrameCnt)
{
	DX12GALRenderDeviceContext* DX12GALDeviceContext = static_cast<DX12GALRenderDeviceContext*>(DeviceToQuery);
	DX12GALRenderDevice* DX12GALDevice = static_cast<DX12GALRenderDevice*>(DX12GALDeviceContext->GetOwnerRenderDevice());

	*OutDevice = DX12GALDevice->GetD3DDevice();
	*OutCommandQueue = DX12GALDeviceContext->GetD3DCommandQueue();
	*OutNestedFrameCnt = SWAP_CHAIN_FRAME_COUNT;
}

void QueryD3DSwapChainInfo(
	GALRenderTarget* SwapChainToQuery, 
	ID3D12Resource** OutBackBuffer,
	D3D12_CPU_DESCRIPTOR_HANDLE* OutBackBufferDescHeap)
{
	if (SwapChainToQuery->GetRenderTargetType() != ERenderTargetType::SwapChain)
	{
		SS_INTERRUPT();
		return;
	}

	DX12GALSwapChainRenderTarget* SwapChainRT = static_cast<DX12GALSwapChainRenderTarget*>(SwapChainToQuery);

	*OutBackBufferDescHeap = SwapChainRT->GetCurrentRTV();
	*OutBackBuffer = SwapChainRT->GetCurrentResource();
}