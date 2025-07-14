#include "pch.h"

#include "DX12GALDefaultPBRMaterialAsset.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "SSGAL/Private/DX12/GALRenderAsset/DX12GALTextureAssetWrapper.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBADefaultPbrMtlParam.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"



DX12GALDefaultPBRMaterialAsset::DX12GALDefaultPBRMaterialAsset(IMaterialAsset* ownerMaterial, DX12GALRenderDevice* InOwnerRenderDevice)
{
	_OwnerMaterialAsset = ownerMaterial;
	_OwnerRenderDevice = InOwnerRenderDevice;

	SS::SHasherW MaterialName = _OwnerMaterialAsset->GetAssetName();
	const MtlDataBase* BaseMtlData = _OwnerMaterialAsset->GetMtlData();
	if (BaseMtlData == nullptr || BaseMtlData->_Type != EMaterialType::DefaultPBR)
	{
		SS_ASSERT(false);
		return;
	}
	const MtlDataDefaultPBR* DefaultPbrMtlData = (const MtlDataDefaultPBR*)BaseMtlData;


	DX12GALRenderDevice* DX12OwnerRenderDevice = (DX12GALRenderDevice*)_OwnerRenderDevice;
	ID3D12Device5* D3DDevice = DX12OwnerRenderDevice->GetD3DDevice();
	int32 DescriptorIncrementalSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	// Alloc Constant Buffer
	{
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();

		_MtlCBParamChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBADefaultPbrMtlParam), MaterialName);

		DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)_MtlCBParamChunk.PageContent;
		_MtlCBSysMemAddr = reinterpret_cast<CBADefaultPbrMtlParam*>(ModelCBPage->ResourceSysMem + _MtlCBParamChunk.ChunkOffset);
		_MtlCBGPUMemAddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + _MtlCBParamChunk.ChunkOffset;
	}


	// Alloc DescriptorTable
	{
		SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();

		_MtlTexSRVDescTableChunk = DescriptorTableAllocator->AllocChunk((int32)EDefaultPBRMatTexTypes::Count, MaterialName);
		ID3D12DescriptorHeap* PageContentDescHeap = (ID3D12DescriptorHeap *)_MtlTexSRVDescTableChunk.PageContent;

		_MtlTexSRVDescTableCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			PageContentDescHeap->GetCPUDescriptorHandleForHeapStart(),
			_MtlTexSRVDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);

		_MtlTexSRVDescTableGPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			PageContentDescHeap->GetGPUDescriptorHandleForHeapStart(),
			_MtlTexSRVDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);
	}

	DX12GALDefaultPBRMaterialAsset::SyncMtlParam();
}

DX12GALDefaultPBRMaterialAsset::~DX12GALDefaultPBRMaterialAsset()
{
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
	SSCustomMemChunkAllocator* DesciptorHandleAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();

	ConstantBufferAllocator->ReleaseChunk(_MtlCBParamChunk);
	DesciptorHandleAllocator->ReleaseChunk(_MtlTexSRVDescTableChunk);
}

void DX12GALDefaultPBRMaterialAsset::SyncMtlParam()
{
	SS::SHasherW MaterialName = _OwnerMaterialAsset->GetAssetName();
	const MtlDataBase* BaseMtlData = _OwnerMaterialAsset->GetMtlData();
	if (BaseMtlData == nullptr || BaseMtlData->_Type != EMaterialType::DefaultPBR)
	{
		SS_ASSERT(false);
		return;
	}
	const MtlDataDefaultPBR* DefaultPbrMtlData = (const MtlDataDefaultPBR*)BaseMtlData;

	DX12GALRenderDevice* DX12OwnerRenderDevice = (DX12GALRenderDevice*)_OwnerRenderDevice;
	ID3D12Device5* D3DDevice = DX12OwnerRenderDevice->GetD3DDevice();
	int32 DescriptorIncrementalSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	_MtlCBSysMemAddr->baseColorFactor = DefaultPbrMtlData->_BaseColorScale.SimdVec;
	_MtlCBSysMemAddr->emissiveFactor = DefaultPbrMtlData->_EmissiveScale.SimdVec;
	_MtlCBSysMemAddr->normalTextureScale = DefaultPbrMtlData->_NormalTexScale;
	_MtlCBSysMemAddr->metallicFactor = DefaultPbrMtlData->_Metallic;
	_MtlCBSysMemAddr->roughnessFactor = DefaultPbrMtlData->_Roughness;

	for (int32 i = 0; i < (int32)EDefaultPBRMatTexTypes::Count; i++)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE DescHandleToCopy = _MtlTexSRVDescTableCPU;
		DescHandleToCopy.Offset(i, DescriptorIncrementalSize);

		ITextureAsset* TexItem = DefaultPbrMtlData->_Textures[i];
		if (TexItem == nullptr)
		{
			SS_ASSERT(false);
			continue;
		}

		GALTextureAssetWrapperBase* GALTexAsset = TexItem->GetGALTextureAsset();
		if (GALTexAsset == nullptr)
		{
			SS_ASSERT(false);
			continue;
		}

		DX12GALTextureAssetWrapper* DX12GALTexAsset = (DX12GALTextureAssetWrapper*)GALTexAsset;
		D3DDevice->CopyDescriptorsSimple(1, DescHandleToCopy, DX12GALTexAsset->_SRVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

}
