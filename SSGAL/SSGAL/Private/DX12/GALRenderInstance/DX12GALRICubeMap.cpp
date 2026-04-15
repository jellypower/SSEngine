#include "pch.h"
#include "DX12GALRICubeMap.h"

#include "Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Public/GALConstantBufferAccessorTypes/CBAModelBuffer.h"
#include "Public/GALConstantBufferAccessorTypes/CBARenderEnvParam.h"
#include "SSRenderer/Public/RenderInstance/IRICubeMap.h"

DX12GALRICubeMap::DX12GALRICubeMap(DX12GALRenderDevice* InOwnerRenderDevice, IRICubeMap* InOwnerCubeMap)
{
	_OwnerRenderDevice = InOwnerRenderDevice;
	_OwnerCubeMap = InOwnerCubeMap;


	ITextureAsset* Texture = _OwnerCubeMap->GetCubemapTexture();
	SS::SHasherW TextureName = Texture->GetAssetName();
	SS_ASSERT(Texture->GetTextureType() == ETextureType::CubeMap);

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

	// Alloc Model CBBuffer
	{
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
		_CubemapModelCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBAModelBuffer), "__CUBEMAP__");
		DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)_CubemapModelCBChunk.PageContent;
		_CubemapCBModelSysmem = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + _CubemapModelCBChunk.ChunkOffset);
		_CubemapCBModelGPUMem = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + _CubemapModelCBChunk.ChunkOffset;
	}

	// Alloc VPMatrix Buffer for Cubemap
	{
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
		_CubemapRenderEnvCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBARenderEnvParam), "__CUBEMAP__");
		DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)_CubemapRenderEnvCBChunk.PageContent;
		_CubemapCBRenderEnvParamSysmem = reinterpret_cast<CBARenderEnvParam*>(ModelCBPage->ResourceSysMem + _CubemapRenderEnvCBChunk.ChunkOffset);
		_CubemapCBRenderEnvParamGPUMem = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + _CubemapRenderEnvCBChunk.ChunkOffset;
	}
}

DX12GALRICubeMap::~DX12GALRICubeMap()
{
	{
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
		ConstantBufferAllocator->ReleaseChunk(_CubemapRenderEnvCBChunk);
		ConstantBufferAllocator->ReleaseChunk(_CubemapModelCBChunk);
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
