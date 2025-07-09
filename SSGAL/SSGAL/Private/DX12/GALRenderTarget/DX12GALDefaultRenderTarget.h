#pragma once
#include "DX12GALRenderTargetBase.h"

class DX12GALRenderDevice;


class DX12GALDefaultRenderTarget : public DX12GALRenderTargetBase
{
public:
	DX12GALDefaultRenderTarget(DX12GALRenderDevice* InRenderDevice, const GALRenderTargetDesc& Desc, const utf16* ResourceName = nullptr);
	virtual ~DX12GALDefaultRenderTarget();

public:
	virtual ERenderTargetType GetRenderTargetType() const override;
	virtual ERTColorFormat GetRTColorFormat() const override;

	virtual void ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From, EResourceStateType To) override;
	virtual void SetRenderTarget(ID3D12GraphicsCommandList* CmdList) override;
	virtual void ClearRenderTarget(ID3D12GraphicsCommandList* CmdList) override;

private:
	GALRenderTargetDesc _InitializedDesc;

	ID3D12Resource* _RenderTargetResource = nullptr;
	ID3D12DescriptorHeap* _RenderTargetDescHeap = nullptr;
	uint32 _RTVDescriptorSize = 0;

	int32 _ResourceWidth;
	int32 _ResourceHeight;
};
