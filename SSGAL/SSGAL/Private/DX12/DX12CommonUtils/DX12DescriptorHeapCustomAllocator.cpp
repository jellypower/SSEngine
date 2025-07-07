#include "pch.h"

#include "DX12DescriptorHeapCustomAllocator.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

DX12DescriptorHeapCustomAllocator::DX12DescriptorHeapCustomAllocator(DX12GALRenderDevice* InRenderDevice, int32 InEachPageSize, int32 MinAllocSize, const utf16* AllocatorName):
	SSCustomMemChunkAllocator(InEachPageSize, MinAllocSize, AllocatorName),
	_RenderDevice(InRenderDevice)
{
	ID3D12Device5* D3DDevice = _RenderDevice->GetD3DDevice();
	_SRVDescriptorHandleIncrementSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void* DX12DescriptorHeapCustomAllocator::AllocPage(int32 PageSize)
{
	ID3D12Device5* DX12Device = _RenderDevice->GetD3DDevice();

	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.NumDescriptors = GetEachPageSize();
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	ID3D12DescriptorHeap* NewDescriptorHeap = nullptr;
	if (FAILED(DX12Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&NewDescriptorHeap))))
	{
		DEBUG_BREAK();
		return nullptr;
	}

	const SS::StringW AllocatorName = GetAllocatorName();
	if (AllocatorName.GetStrLen() > 0)
	{
		NewDescriptorHeap->SetName(AllocatorName.C_Str());
	}

	return NewDescriptorHeap;
}

void DX12DescriptorHeapCustomAllocator::FreePage(void* Page)
{
	ID3D12DescriptorHeap* DescHeap = (ID3D12DescriptorHeap*)Page;
	DescHeap->Release();
}
