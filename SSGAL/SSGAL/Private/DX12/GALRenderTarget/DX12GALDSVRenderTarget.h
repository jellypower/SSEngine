#pragma once
#include <SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h>

#include "DX12GALRenderTargetBase.h"

class DX12GALRenderDevice;

class DX12GALDSVRenderTarget : public DX12GALRenderTargetBase
{
private:
	GALRenderTargetDesc _InitializedDesc;

	uint32 _DSVDescriptorSize = 0;
	ID3D12DescriptorHeap* _DSVHeap = nullptr;
	ID3D12Resource* _DepthStencil = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _DepthStencilDescHandle;

	AllocatedChunkHeader _SRVDescTableChunk;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _SRVDescHandle;

	Vector2i32 _ResourceSize;
	int32 _ResrouceRowPitch;

public:
	DX12GALDSVRenderTarget(DX12GALRenderDevice* InRenderDevice, const GALRenderTargetDesc& Desc, const utf16* ResourceName = nullptr);
	virtual ~DX12GALDSVRenderTarget();


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

};
