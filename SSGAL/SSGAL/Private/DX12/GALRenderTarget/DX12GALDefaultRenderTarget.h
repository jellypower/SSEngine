#pragma once
#include "DX12GALRenderTargetBase.h"

class DX12GALRenderDevice;


class DX12GALDefaultRenderTarget : public DX12GALRenderTargetBase
{
public:
	DX12GALDefaultRenderTarget(DX12GALRenderDevice* InRenderDevice, const GALRenderTargetDesc& Desc, const utf16* ResourceName = nullptr);
	virtual ~DX12GALDefaultRenderTarget();

public:
	virtual Vector2i32 GetResourceSize() const override;
	virtual int32 GetResourceRowPitch() const override;
	virtual ERenderTargetType GetRenderTargetType() const override;
	virtual ERTColorFormat GetRTColorFormat() const override;

	virtual ID3D12Resource* GetCurrentResource() const override;

	virtual void ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From, EResourceStateType To) override;
	virtual void SetRenderTarget(ID3D12GraphicsCommandList* CmdList) override;
	virtual void ClearRenderTarget(ID3D12GraphicsCommandList* CmdList) override;

private:
	GALRenderTargetDesc _InitializedDesc;

	ID3D12Resource* _RenderTargetResource = nullptr;
	ID3D12DescriptorHeap* _RenderTargetDescHeap = nullptr;
	uint32 _RTVDescriptorSize = 0;

	uint32 _DSVDescriptorSize = 0;
	ID3D12DescriptorHeap* _DSVHeap = nullptr;
	ID3D12Resource* _DepthStencil = nullptr;

	Vector2i32 _ResourceSize;
	int32 _ResrouceRowPitch;
};
