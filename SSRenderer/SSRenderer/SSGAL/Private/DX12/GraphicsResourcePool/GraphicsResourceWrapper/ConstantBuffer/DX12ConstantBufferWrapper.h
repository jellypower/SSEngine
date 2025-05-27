#pragma once
#include <d3d12.h>

#include "SSGAL/Public/GraphicsResourcePool/GraphicsResourceWrapper/ConstantBuffer/ConstantBufferWrapper.h"

struct DX12ConstantBufferWrapper : public ConstantBufferWrapper
{
	D3D12_CPU_DESCRIPTOR_HANDLE	CBVHandle;
	D3D12_GPU_VIRTUAL_ADDRESS	GPUMemAddr;
	UINT8* SystemMemAddr;
};
