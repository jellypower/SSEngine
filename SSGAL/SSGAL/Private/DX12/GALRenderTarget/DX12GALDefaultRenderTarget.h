#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"

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
	virtual CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const override;
	virtual CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentSRV() const override;

	virtual void ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From, EResourceStateType To) override;
	virtual void ClearRenderTarget(ID3D12GraphicsCommandList* CmdList) override;

protected:
	ID3D12Resource* _RenderTargetResource = nullptr;

private:
	GALRenderTargetDesc _InitializedDesc;

	ID3D12DescriptorHeap* _RenderTargetDescHeap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _RTVHandle;
	uint32 _RTVDescriptorSize = 0;

	AllocatedChunkHeader _SRVDescTableChunk;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _SRVHandle;

	Vector2i32 _ResourceSize;
	int32 _ResrouceRowPitch;
};
