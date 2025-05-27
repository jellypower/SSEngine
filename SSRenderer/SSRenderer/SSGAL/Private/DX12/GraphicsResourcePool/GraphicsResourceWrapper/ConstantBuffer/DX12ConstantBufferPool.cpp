#include "DX12ConstantBufferPool.h"
#include <d3d12.h>
#include <d3dx12.h>

#include "DX12ConstantBufferWrapper.h"
#include "SSGAL/Private/GALInstanceGlobalVariablePrivate.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"



DX12ConstantBufferPool::DX12ConstantBufferPool(const ConstantBufferPoolDesc& Desc)
	: ConstantBufferPool(Desc)
{

	_itemCntPerPage = Desc.ResourceSizePerPage / _constantBufferitemSize;
	_bufferAlignmentPaddingSize = Desc.ResourceSizePerPage % _constantBufferitemSize;

	_constantBufferResourcePages.Reserve(_resourceSizePerPage);
	_constantBufferResourceSysmemPages.Reserve(_resourceSizePerPage);
	for (int32 i = 0; i < Desc.DefaultPoolPageCnt; i++)
	{
		IncreaseBufferPage();
	}
}

DX12ConstantBufferPool::~DX12ConstantBufferPool()
{
	CleanUpResources();
}

bool DX12ConstantBufferPool::IsValid() const
{
	return _constantBufferResourcePages.GetSize() > 0;
}

ConstantBufferWrapper* DX12ConstantBufferPool::RentConstantBuffer()
{
	DX12ConstantBufferWrapper* CBPEntityArray = _entityPool[_curAllocOffsetPage];
	DX12ConstantBufferWrapper* CBEntity = (CBPEntityArray +_curAllocEntityOffset);
	IncreaseAllocOffset();
	return CBEntity;
}

void DX12ConstantBufferPool::ResetAllocState()
{
	_curAllocOffsetPage = 0;
	_curAllocEntityOffset = 0;
}

void DX12ConstantBufferPool::IncreaseAllocOffset()
{
	_curAllocEntityOffset++;
	if (_curAllocEntityOffset >= _itemCntPerPage)
	{
		_curAllocEntityOffset = 0;
		_curAllocOffsetPage++;
		if (_curAllocOffsetPage >= _entityPool.GetSize())
		{
			IncreaseBufferPage();
		}
	}
}

HRESULT DX12ConstantBufferPool::IncreaseBufferPage()
{
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)SSGALModule::Private::g_GALRenderDevice)->GetD3DDevice();
	ID3D12Resource* newResource = nullptr;
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_resourceSizePerPage);

	HRESULT hr = D3DDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&newResource));
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		CleanUpResources();
		return hr;
	}
	newResource->SetName(L"ConstantBufferPool");
	_constantBufferResourcePages.PushBack(newResource);


	byte* newResourceSysmem = nullptr;
	CD3DX12_RANGE writeRange(0, 0);
	hr = newResource->Map(0, &writeRange, reinterpret_cast<void**>(&newResourceSysmem));
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		CleanUpResources();
		return hr;
	}
	_constantBufferResourceSysmemPages.PushBack(newResourceSysmem);


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = _itemCntPerPage;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ID3D12DescriptorHeap* newDescriptorHeap = nullptr;
	hr = D3DDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&newDescriptorHeap));
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		CleanUpResources();
		return hr;
	}
	newDescriptorHeap->SetName(L"ConstantBufferPool_DescHeap");
	_descriptorHeaps.PushBack(newDescriptorHeap);


	CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(newDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = newResource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = _constantBufferitemSize;
	uint32 DescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	DX12ConstantBufferWrapper* newEntity = DBG_NEW DX12ConstantBufferWrapper[_itemCntPerPage];

	for (int32 i = 0; i < _itemCntPerPage; i++)
	{
		D3DDevice->CreateConstantBufferView(&cbvDesc, heapHandle);
		newEntity[i].CBVHandle = heapHandle;
		newEntity[i].GPUMemAddr = cbvDesc.BufferLocation;
		newEntity[i].SystemMemAddr = newResourceSysmem;

		heapHandle.Offset(1, DescriptorSize);
		cbvDesc.BufferLocation += _constantBufferitemSize;
		newResourceSysmem += _constantBufferitemSize;
	}
	_entityPool.PushBack(newEntity);

	return S_OK;
}

void DX12ConstantBufferPool::CleanUpResources()
{
	for (DX12ConstantBufferWrapper*  entityItem : _entityPool)
	{
		delete entityItem;
	}

	for (ID3D12DescriptorHeap* descriptorHeapItem : _descriptorHeaps)
	{
		descriptorHeapItem->Release();
	}

	for (ID3D12Resource* resourceItem : _constantBufferResourcePages)
	{
		if (resourceItem != nullptr)
		{
			resourceItem->Release();
		}
	}

	_entityPool.SetSizeDirectly(0);
	_descriptorHeaps.SetSizeDirectly(0);
	_constantBufferResourceSysmemPages.SetSizeDirectly(0);
	_constantBufferResourcePages.SetSizeDirectly(0);
}
