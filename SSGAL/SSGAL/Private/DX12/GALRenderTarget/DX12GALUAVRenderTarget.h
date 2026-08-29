#pragma once
#include "DX12GALDefaultRenderTarget.h"

class DX12GALUAVRenderTarget : public DX12GALDefaultRenderTarget
{
public:
	DX12GALUAVRenderTarget(DX12GALRenderDevice* InRenderDevice, const GALRenderTargetDesc& Desc, const utf16* ResourceName);
	void Release() override;
	
public:
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentUAV() const;

public:
	ERenderTargetType GetRenderTargetType() const override;


private:
	uint32 _UAVDescSize = 0;
	ID3D12DescriptorHeap* _UAVDescHeap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _UAVDescHandle;
};
