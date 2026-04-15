#pragma once
// #include <d3d12.h>
// #include <d3dx12.h>

struct DX12ConstantBufferResourcePage
{
	ID3D12Resource* D3D12Resource;
	byte* ResourceSysMem;
};
