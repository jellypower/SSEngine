#include "pch.h"
#include "DX12GALSkinnedMeshAssetWrapper.h"

#include <SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h>
#include <SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h>
#include <SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h>

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Public/GALStructuredBufferAccessorTypes/SBASkinningJointMatrix.h"

DX12GALSkinnedMeshAssetWrapper::DX12GALSkinnedMeshAssetWrapper(IMeshAsset* ownerMeshAsset, DX12GALRenderDeviceContext* ResourceUpdateExecutor):
	DX12GALMeshAssetWrapper(ownerMeshAsset, ResourceUpdateExecutor)
{
	HRESULT hr;

	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();
	SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = _OwnerRenderDevice->GetDescriptorTableAllocatorForTex();

	const MeshRawDataBase* MeshRawData = _OwnerMeshAsset->GetMeshRawData();
	SS::SHasherW MeshName = _OwnerMeshAsset->GetAssetName();

	if (MeshRawData->GetMeshType() != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}
	const MeshRawDataSkinned* SkinnedMeshData = static_cast<const MeshRawDataSkinned*>(MeshRawData);

	int32 BoneCnt = SkinnedMeshData->_BoneHeader._BoneCnt;


	// Create Resource
	{
		SCOPE_PROFILE(SKM_CreateBoneBuffer);
		CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(SBASkinningJointMatrix) * BoneCnt);
		hr = D3DDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_OriginalJointInverseResource));
		if (FAILED(hr))
		{
			DEBUG_BREAK();
			return;
		}
		_OriginalJointInverseResource->SetName(MeshName.C_Str());
	}

	// Alloc descriptor Heap
	{
		_OriginalJointInverseSRVDescriptorChunk = DescriptorTableAllocatorForTex->AllocChunk(1, MeshName);
		ID3D12DescriptorHeap* AllocatedDescHeap = (ID3D12DescriptorHeap*)_OriginalJointInverseSRVDescriptorChunk.PageContent;
		CD3DX12_CPU_DESCRIPTOR_HANDLE SRVHandle(
			AllocatedDescHeap->GetCPUDescriptorHandleForHeapStart(),
			_OriginalJointInverseSRVDescriptorChunk.ChunkOffset,
			D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

		_OriginalJointInverseSRVDescHandle = SRVHandle;
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
		D3DDevice->CreateShaderResourceView(_OriginalJointInverseResource, &SRVDesc, _OriginalJointInverseSRVDescHandle);
	}

	// WriteData
	{
		SCOPE_PROFILE(SKM_Bone);

		CD3DX12_RANGE writeRange(0, 0);
		void* pData = nullptr;
		hr = _OriginalJointInverseResource->Map(0, &writeRange, reinterpret_cast<void**>(&pData));
		if (FAILED(hr))
		{
			SS_INTERRUPT();
			return;
		}
		SBASkinningJointMatrix* JointInverseData = (SBASkinningJointMatrix*)pData;

		for (int32 i = 0; i < BoneCnt; i++)
		{
			JointInverseData[i].WMatrix = XMMatrixTranspose(SkinnedMeshData->_BonePlacements[i].AsInverseMatrix());
			JointInverseData[i].RotMatrix = XMMatrixTranspose(SkinnedMeshData->_BonePlacements[i].Rotation.AsInverseMatrix());
		}

		_OriginalJointInverseResource->Unmap(0, nullptr);
	}
}

void DX12GALSkinnedMeshAssetWrapper::Release()
{
	if (_VertexBuffer != nullptr)
	{
		_VertexBuffer->Release();
	}

	if (_IndexBuffer != nullptr)
	{
		_IndexBuffer->Release();
	}


	_OriginalJointInverseResource->Release();
	SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = _OwnerRenderDevice->GetDescriptorTableAllocatorForTex();
	DescriptorTableAllocatorForTex->ReleaseChunk(_OriginalJointInverseSRVDescriptorChunk);

	delete this;
}