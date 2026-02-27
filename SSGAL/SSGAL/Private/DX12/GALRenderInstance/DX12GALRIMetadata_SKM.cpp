#include "pch.h"
#include "DX12GALRIMetadata_SKM.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "Private/DX12/GALRenderAsset/DX12GALSkinnedMeshAssetWrapper.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"

#include "SSRenderer/Public/RenderInstance/IRISkinnedMesh.h"

#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/Utils/SSDX12Utils.h"
#include "Public/GALStructuredBufferAccessorTypes/SBASkinningJointMatrix.h"

DX12GALRIMetadata_SKM::DX12GALRIMetadata_SKM(GALRenderDevice* InRenderDevice, const IRISkinnedMesh* InOwnerRenderInstance) :
	DX12GALRIMetadata_SM(InRenderDevice, InOwnerRenderInstance)
{
	HRESULT hr;
	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();
	int32 DescriptorIncrementalSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	IMeshAsset* OwnerMesh = InOwnerRenderInstance->GetMeshAsset();
	SS::SHasherW MeshAssetName = OwnerMesh->GetAssetName();
	const MeshRawDataBase* MeshRawData = OwnerMesh->GetMeshRawData();
	if (MeshRawData->GetMeshType() != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}

	const MeshRawDataSkinned* SkinnedMeshRawData = (const MeshRawDataSkinned*)MeshRawData;
	int32 BoneCnt = SkinnedMeshRawData->_BoneHeader._BoneCnt;
	const DX12GALSkinnedMeshAssetWrapper* GALSkinnedMesh = static_cast<const DX12GALSkinnedMeshAssetWrapper*>(OwnerMesh->GetGALMeshAsset());


	// Create Resource
	{
		CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(SBASkinningJointMatrix) * BoneCnt);
		hr = D3DDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_JointSBResource));
		if (FAILED(hr))
		{
			DEBUG_BREAK();
			return;
		}
		_JointSBResource->SetName(MeshAssetName.C_Str());
	}

	// Create descriptor Heap
	{
		SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();

		_JointSRVDescTableChunk = DescriptorTableAllocator->AllocChunk(2);
		_CachedJointSRVDescHeap = (ID3D12DescriptorHeap*)_JointSRVDescTableChunk.PageContent;

		_JointSRVDescTableCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_CachedJointSRVDescHeap->GetCPUDescriptorHandleForHeapStart(),
			_JointSRVDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);

		_JointSRVDescTableGPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			_CachedJointSRVDescHeap->GetGPUDescriptorHandleForHeapStart(),
			_JointSRVDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);

	}

	// Create SRV
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Buffer.FirstElement = 0;
		SRVDesc.Buffer.NumElements = BoneCnt;
		SRVDesc.Buffer.StructureByteStride = sizeof(SBASkinningJointMatrix);
		SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		D3DDevice->CreateShaderResourceView(_JointSBResource, &SRVDesc, _JointSRVDescTableCPU);


		CD3DX12_CPU_DESCRIPTOR_HANDLE JointOriginDescTableHandle = _JointSRVDescTableCPU;
		JointOriginDescTableHandle.Offset(1, DescriptorIncrementalSize);

		D3DDevice->CopyDescriptorsSimple(1, 
			JointOriginDescTableHandle, 
			GALSkinnedMesh->_OriginalJointInverseSRVDescHandle, 
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// Mapping
	{
		CD3DX12_RANGE writeRange(0, 0);
		void* pData = nullptr;
		hr = _JointSBResource->Map(0, &writeRange, reinterpret_cast<void**>(&pData));
		if (FAILED(hr))
		{
			DEBUG_BREAK();
			return;
		}
		SBASkinningJointMatrix* SBAJoints = (SBASkinningJointMatrix*)pData;

		for (int32 i = 0; i < BoneCnt; i++)
		{
			SBAJoints[i].WMatrix = XMMatrixTranspose(Transform::Identity.AsMatrix()); // TODO: 임시로 넣은 코드. 나중에 고치기
			SBAJoints[i].RotMatrix = XMMatrixTranspose(Transform::Identity.Rotation.AsMatrix());
		}

		_JointSBResource->Unmap(0, nullptr);
	}


}

DX12GALRIMetadata_SKM::~DX12GALRIMetadata_SKM()
{
	_JointSBResource->Release();

	SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();
	DescriptorTableAllocator->ReleaseChunk(_JointSRVDescTableChunk);
}

ERenderInstanceType DX12GALRIMetadata_SKM::GetMetadataRenderInstanceType()
{
	return ERenderInstanceType::SkinnedMesh;
}

void DX12GALRIMetadata_SKM::SyncBonePose()
{
	IRISkinnedMesh* OwnerSkinnedMesh = (IRISkinnedMesh*)_OwnerRenderInstance;
	const SS::PooledList<SBASkinningJointMatrix>&  BonePose = OwnerSkinnedMesh->GetSkeletonPose();
	int32 BoneCnt = BonePose.GetSize();

	// Mapping
	{
		CD3DX12_RANGE writeRange(0, 0);
		void* pData = nullptr;
		HRESULT hr = _JointSBResource->Map(0, &writeRange, reinterpret_cast<void**>(&pData));
		if (FAILED(hr))
		{
			DEBUG_BREAK();
			return;
		}
		SBASkinningJointMatrix* SBAJoints = (SBASkinningJointMatrix*)pData;

		for (int32 i = 0; i < BoneCnt; i++)
		{
			SBAJoints[i].WMatrix = XMMatrixTranspose(BonePose[i].WMatrix);
			SBAJoints[i].RotMatrix = XMMatrixTranspose(BonePose[i].RotMatrix);
		}

		_JointSBResource->Unmap(0, nullptr);
	}
}
