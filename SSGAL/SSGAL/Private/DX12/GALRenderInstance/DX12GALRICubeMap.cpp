#include "pch.h"
#include "DX12GALRICubeMap.h"

#include "Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Public/GALConstantBufferAccessorTypes/CBAModelBuffer.h"
#include "SSRenderer/Public/RenderInstance/IRICubeMap.h"

DX12GALRICubeMap::DX12GALRICubeMap(DX12GALRenderDevice* InOwnerRenderDevice, IRICubeMap* InOwnerCubeMap)
{
	_OwnerRenderDevice = InOwnerRenderDevice;
	_OwnerCubeMap = InOwnerCubeMap;


	ITextureAsset* Texture = _OwnerCubeMap->GetCubemapTexture();
	SS::SHasherW TextureName = Texture->GetAssetName();
	Texture->GetTextureType();

	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();

	// Alloc DescriptorTable
	{
		int32 DescriptorIncrementalSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();

		_CubemapTextureDescTableChunk = DescriptorTableAllocator->AllocChunk(1, TextureName);
		_CubemapDescHeap = (ID3D12DescriptorHeap*)_CubemapTextureDescTableChunk.PageContent;

		_CubemapDescTableCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_CubemapDescHeap->GetCPUDescriptorHandleForHeapStart(),
			_CubemapTextureDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);

		_CubemapDescTableGPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			_CubemapDescHeap->GetGPUDescriptorHandleForHeapStart(),
			_CubemapTextureDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);
	}

	// Alloc ConstantBuffer
	{
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
		_CubemapCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBAModelBuffer), "__CUBEMAP__");
		DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)_CubemapCBChunk.PageContent;
		_CubemapCBSysMemAddr = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + _CubemapCBChunk.ChunkOffset);
		_CubemapCBGPUMemAddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + _CubemapCBChunk.ChunkOffset;

		_CubemapCBSysMemAddr->ObjectID = 0; // 일단 사용 안함
		_CubemapCBSysMemAddr->RotMatrix = XMMatrixIdentity();
		_CubemapCBSysMemAddr->WMatrix = InOwnerCubeMap->GetWorldTransformMatrix();
	}
}

DX12GALRICubeMap::~DX12GALRICubeMap()
{
	{
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
		ConstantBufferAllocator->ReleaseChunk(_CubemapCBChunk);
	}

	{
		SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();
		DescriptorTableAllocator->ReleaseChunk(_CubemapTextureDescTableChunk);
	}
}

ERenderInstanceType DX12GALRICubeMap::GetMetadataRenderInstanceType()
{
	return ERenderInstanceType::CubeMap;
}
