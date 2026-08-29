#pragma once
#include "DX12GALRenderTargetBase.h"

class DX12GALRenderDeviceContext;
class GALRenderDeviceContext;

class DX12GALSwapChainRenderTarget : public DX12GALRenderTargetBase
{
public:
	DX12GALSwapChainRenderTarget(DX12GALRenderDeviceContext* InRenderDeviceContext, HWND InhWnd, IDXGIFactory4* InFactory);
	void Release() override;

public:
	HRESULT Present();
	void UpdateViewportSize(uint32 BackBufferWidth, uint32 BackBufferHeight);

public:
	virtual Vector2i32 GetResourceSize() const override;
	virtual int32 GetResourceRowPitch() const override;
	virtual ERenderTargetType GetRenderTargetType() const override;
	virtual ERTColorFormat GetRTColorFormat() const override;

	virtual ID3D12Resource* GetCurrentResource() const override;
	virtual CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const override;
	virtual CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentSRV() const override;

	virtual void ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From, EResourceStateType To) override;
	virtual void ClearRenderTarget(ID3D12GraphicsCommandList* CmdList, const Vector4f& ClearColor) override;

private:
	Vector2i32 _ResourceSize;
	int32 _ResourceRowPitch;

	IDXGISwapChain3* _swapChain = nullptr;
	uint32 _SwapChainFlags = 0;


	ID3D12Resource* _DXRenderTargets[GAL_NESTED_FRAME_CNT];
	CD3DX12_CPU_DESCRIPTOR_HANDLE _RTDescHandles[GAL_NESTED_FRAME_CNT];
	ID3D12DescriptorHeap* _RTVDescHeap = nullptr;
	uint32 _RTVDescriptorSize = 0;
};
