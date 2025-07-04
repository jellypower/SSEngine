#include "DX12GALRIMetadata_SM.h"

#include <d3dx12.h>

#include "SSGAL/Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "SSGAL/Private/DX12/DX12CommonUtils/DX12DescriptorHeapCustomAllocator.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBAModelBuffer.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBARenderEnvParam.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"

DX12GALRIMetadata_SM::DX12GALRIMetadata_SM(GALRenderDevice* InRenderDevice, const IRIMesh* InOwnerRenderInstance)
{
	_OwnerRenderDevice = (DX12GALRenderDevice*)InRenderDevice;
	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
	DX12DescriptorHeapCustomAllocator* DescriptorTableAllocator = (DX12DescriptorHeapCustomAllocator*)_OwnerRenderDevice->GetDescriptorTableAllocator();
	_SubMeshCntCache = InOwnerRenderInstance->GetModelAsset()->GetSubMeshCnt();
	SS::SHasherW ModelName = InOwnerRenderInstance->GetModelAsset()->GetAssetName();

	_OwnerRenderInstance = InOwnerRenderInstance;

	_ModelCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBAModelBuffer), ModelName);
	_RenderEnvCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBARenderEnvParam), ModelName);
	_DescriptorTableChunk = DescriptorTableAllocator->AllocChunk(
		_SubMeshCntCache,
		ModelName);


	// Constant Buffer
	{

		DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)_ModelCBChunk.PageContent;
		_ModelCBSysMemAddr = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + _ModelCBChunk.ChunkOffset);
		_ModelCBGPUMemAddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + _ModelCBChunk.ChunkOffset;

		DX12ConstantBufferResourcePage* RenderEnvCBPage = (DX12ConstantBufferResourcePage*)_RenderEnvCBChunk.PageContent;
		_RenderEnvCBSysMemAddr = reinterpret_cast<CBARenderEnvParam*>(RenderEnvCBPage->ResourceSysMem + _RenderEnvCBChunk.ChunkOffset);
		_RenderEnvCBGPUMemAddr = RenderEnvCBPage->D3D12Resource->GetGPUVirtualAddress() + _RenderEnvCBChunk.ChunkOffset;
	}


	// Descriptor Table
	{
		int32 DescriptorSRVSize = DescriptorTableAllocator->GetSRVDescriptorHandleIncrementSize();

		ID3D12DescriptorHeap* DescriptorHeap = (ID3D12DescriptorHeap*)_DescriptorTableChunk.PageContent;


		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHeapStart = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDEscriptorHeapStart = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		for (int32 i = 0; i < _SubMeshCntCache; i++)
		{
			_CPUDescriptorHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				CPUDescriptorHeapStart,
				_DescriptorTableChunk.ChunkOffset + i,
				DescriptorSRVSize);
			_GPUDescriptorHandle[i] = CD3DX12_GPU_DESCRIPTOR_HANDLE(
				GPUDEscriptorHeapStart,
				_DescriptorTableChunk.ChunkOffset + i,
				DescriptorSRVSize);
		}
	}
}

DX12GALRIMetadata_SM::~DX12GALRIMetadata_SM()
{
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
	SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();

	DescriptorTableAllocator->ReleaseChunk(_DescriptorTableChunk);
	ConstantBufferAllocator->ReleaseChunk(_ModelCBChunk);
	ConstantBufferAllocator->ReleaseChunk(_RenderEnvCBChunk);
}

ERenderInstanceType DX12GALRIMetadata_SM::GetMetadataRenderInstanceType()
{
	return ERenderInstanceType::StaticMesh;
}
