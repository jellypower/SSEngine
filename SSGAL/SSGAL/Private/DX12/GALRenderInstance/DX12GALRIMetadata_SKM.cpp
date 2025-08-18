#include "pch.h"
#include "DX12GALRIMetadata_SKM.h"

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

	IMeshAsset* OwnerMesh = InOwnerRenderInstance->GetModelAsset()->GetMeshAsset();
	SS::SHasherW MeshAssetName = OwnerMesh->GetAssetName();
	const MeshRawDataBase* MeshRawData = OwnerMesh->GetMeshRawData();
	if (MeshRawData->_MeshType != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}

	const MeshRawDataSkinned* SkinnedMeshRawData = (const MeshRawDataSkinned*)MeshRawData;
	int32 BoneCnt = SkinnedMeshRawData->_BoneCnt;


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
		D3D12_DESCRIPTOR_HEAP_DESC RTHeapDesc = {};
		RTHeapDesc.NumDescriptors = 1;
		RTHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		RTHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = D3DDevice->CreateDescriptorHeap(&RTHeapDesc, IID_PPV_ARGS(&_JointSBDescHeap));
		if (FAILED(hr))
		{
			SS_INTERRUPT();
		}
		_JointSBDescHeap->SetName(MeshAssetName.C_Str());
	}

	// Create SRV
	{
		_JointSRVDescHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_JointSBDescHeap->GetCPUDescriptorHandleForHeapStart());

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Buffer.FirstElement = 0;
		SRVDesc.Buffer.NumElements = BoneCnt;
		SRVDesc.Buffer.StructureByteStride = sizeof(SBASkinningJointMatrix);
		SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		D3DDevice->CreateShaderResourceView(_JointSBResource, &SRVDesc, _JointSRVDescHandle);
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
		_SBAJoints = (SBASkinningJointMatrix*)pData;
	}
}

ERenderInstanceType DX12GALRIMetadata_SKM::GetMetadataRenderInstanceType()
{
	return ERenderInstanceType::SkinnedMesh;
}
