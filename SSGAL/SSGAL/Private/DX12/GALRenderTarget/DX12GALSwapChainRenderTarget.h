#pragma once
#include "DX12GALRenderTargetBase.h"

#include "SSGAL/Public/SSGALInlineSettings.h"

class GALRenderDeviceContext;
class DX12GALRenderDevice;

class DX12GALSwapChainRenderTarget : public DX12GALRenderTargetBase
{
public:
	DX12GALSwapChainRenderTarget(DX12GALRenderDevice* InRenderDevice, HWND InhWnd, IDXGIFactory4* InFactory);
	virtual ~DX12GALSwapChainRenderTarget();

public:
	HRESULT Present();
	void UpdateViewportSize(uint32 BackBufferWidth, uint32 BackBufferHeight);

public:
	virtual Vector2i32 GetResourceSize() const override;
	virtual int32 GetResourceRowPitch() const override;
	virtual ERenderTargetType GetRenderTargetType() const override;
	virtual ERTColorFormat GetRTColorFormat() const override;

	virtual ID3D12Resource* GetCurrentResource() const override;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentDescHandle() const override;

	virtual void ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From, EResourceStateType To) override;
	virtual void ClearRenderTarget(ID3D12GraphicsCommandList* CmdList) override;

private:
	void CreateDSVDescHeap();
	void RecreateDepthStencil(uint32 Width, uint32 Height);

private:

	Vector2i32 _ResourceSize;
	int32 _ResourceRowPitch;

	IDXGISwapChain3* _swapChain = nullptr;
	uint32 _SwapChainFlags = 0;

	uint32 _DSVDescriptorSize = 0;
	ID3D12DescriptorHeap* _DSVHeap = nullptr;
	ID3D12Resource* _DepthStencil = nullptr;

	SS::PooledList<ID3D12Resource*, SS::InlineAllocator<SWAP_CHAIN_FRAME_COUNT>> _DXRenderTargets;
	SS::PooledList<CD3DX12_CPU_DESCRIPTOR_HANDLE, SS::InlineAllocator<SWAP_CHAIN_FRAME_COUNT>> _RTDescHandles;
	ID3D12DescriptorHeap* _RTVDescHeap = nullptr;
	uint32 _RTVDescriptorSize = 0;
};
