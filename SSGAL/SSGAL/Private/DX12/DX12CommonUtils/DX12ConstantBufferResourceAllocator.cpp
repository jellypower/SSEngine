#include "pch.h"

#include "DX12ConstantBufferResourceAllocator.h"

#include "DX12ConstantBufferResourcePage.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

DX12ConstantBufferResourceCustomAllocator::DX12ConstantBufferResourceCustomAllocator(DX12GALRenderDevice* InRenderDevice, int32 InEachPageSize, int32 MinAllocSize, const utf16* AllocatorName) :
	SSCustomMemChunkAllocator(InEachPageSize, MinAllocSize, 256, AllocatorName),
	_RenderDevice(InRenderDevice)
{

}

void* DX12ConstantBufferResourceCustomAllocator::AllocPage(int32 PageSize)
{
	ID3D12Device5* DX12Device = _RenderDevice->GetD3DDevice();
	ID3D12Resource* newResource = nullptr;
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetEachPageSize());


	HRESULT hr = DX12Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&newResource));
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return nullptr;
	}
	newResource->SetName(GetAllocatorName().C_Str());


	byte* newResourceSysmem = nullptr;
	CD3DX12_RANGE writeRange(0, 0);
	hr = newResource->Map(0, &writeRange, reinterpret_cast<void**>(&newResourceSysmem));
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return nullptr;
	}


	DX12ConstantBufferResourcePage* NewResourcePage = DBG_NEW DX12ConstantBufferResourcePage;
	NewResourcePage->D3D12Resource = newResource;
	NewResourcePage->ResourceSysMem = newResourceSysmem;

	return NewResourcePage;
}

void DX12ConstantBufferResourceCustomAllocator::FreePage(void* Page)
{
	DX12ConstantBufferResourcePage* ResourcePage = (DX12ConstantBufferResourcePage*)Page;
	ResourcePage->D3D12Resource->Release();
	delete ResourcePage;
}
