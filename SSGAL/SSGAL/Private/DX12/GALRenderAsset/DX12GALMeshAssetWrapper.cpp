#include "pch.h"


#include "DX12GALMeshAssetWrapper.h"

#include <SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h>

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "Private/DX12/GALResourceUpdater/DX12GALResourceUpdater.h"


DX12GALMeshAssetWrapper::DX12GALMeshAssetWrapper(IMeshAsset* ownerMeshAsset, DX12GALRenderDeviceContext* ResourceUpdateExecutor)
{
	_OwnerMeshAsset = ownerMeshAsset;
	_OwnerRenderDevice = static_cast<DX12GALRenderDevice*>(ResourceUpdateExecutor->GetOwnerRenderDevice());

	HRESULT hr = S_OK;

	DX12GALResourceUpdater* DX12ResourceUpdater = static_cast<DX12GALResourceUpdater*>(ResourceUpdateExecutor->GetResourceUpdater());
	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();
	ID3D12GraphicsCommandList* CurCommandList = ResourceUpdateExecutor->GetCurrentDrawWorkerCmdList();


	const MeshRawDataBase* MeshRawData = _OwnerMeshAsset->GetMeshRawData();
	SS::SHasherW MeshName = _OwnerMeshAsset->GetAssetName();

	EMeshType MeshType = MeshRawData->GetMeshType();
	if (MeshType != EMeshType::Rigid && MeshType != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}
	const MeshRawDataDefault* DefaultMeshRawData = static_cast<const MeshRawDataDefault*>(MeshRawData);

	
	// Create Vertex buffer and Update
	{
		SCOPE_PROFILE(SM_Vertex);
		int32 EachVertexSize = EachVertexSizeOfType(DefaultMeshRawData->_VertexHeader.MeshType);
		int32 VertexCnt = DefaultMeshRawData->_VertexHeader.vertexCnt;
		uint64 VertexBufferSize = VertexCnt * EachVertexSize;
		const void* VertexData = DefaultMeshRawData->_vertexData;

		ID3D12Resource* NewVertexBuffer = nullptr;


		CD3DX12_HEAP_PROPERTIES DefaultHeapTypeProp(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
		hr = D3DDevice->CreateCommittedResource(
			&DefaultHeapTypeProp,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&NewVertexBuffer));
		if (FAILED(hr))
		{
			SS_INTERRUPT();
			return;
		}
		NewVertexBuffer->SetName(MeshName.C_Str());

		hr = DX12ResourceUpdater->UpdateBuffer(CurCommandList, NewVertexBuffer, VertexData, VertexBufferSize,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		if (FAILED(hr))
		{
			SS_INTERRUPT();
			return;
		}

		D3D12_VERTEX_BUFFER_VIEW NewVertexBufferView;
		NewVertexBufferView.BufferLocation = NewVertexBuffer->GetGPUVirtualAddress();
		NewVertexBufferView.StrideInBytes = EachVertexSize;
		NewVertexBufferView.SizeInBytes = VertexBufferSize;

		_VertexBuffer = NewVertexBuffer;
		_VertexBufferView = NewVertexBufferView;
	}

	// Create Index buffer and Update
	{
		SCOPE_PROFILE(SM_Index);
		int32 SubMeshCnt = DefaultMeshRawData->_VertexHeader.subMeshCnt;
		_SubMeshCnt = SubMeshCnt;
		int32 WholeIdxDataCnt = DefaultMeshRawData->_VertexHeader.wholeIndexDataCnt;
		const uint32* IndexData = DefaultMeshRawData->_indexData;
		int32 WholeIndexBufferSize = sizeof(uint32) * WholeIdxDataCnt;

		ID3D12Resource* NewIndexBuffer = nullptr;

		CD3DX12_HEAP_PROPERTIES DefaultHeapProp(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(WholeIndexBufferSize);
		hr = D3DDevice->CreateCommittedResource(
			&DefaultHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&NewIndexBuffer));
		if (FAILED(hr))
		{
			SS_INTERRUPT();
			return;
		}
		NewIndexBuffer->SetName(MeshName.C_Str());

		hr = DX12ResourceUpdater->UpdateBuffer(CurCommandList, NewIndexBuffer, IndexData, WholeIndexBufferSize,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		if (FAILED(hr))
		{
			SS_INTERRUPT();
			return;
		}

		_IndexBuffer = NewIndexBuffer;

		int32 CurIdxDataCnt = 0;
		int32 Offset = 0;
		for (int32 i = 0; i < SubMeshCnt; i++)
		{
			CurIdxDataCnt = DefaultMeshRawData->_VertexHeader.indexDataCnt[i];

			D3D12_INDEX_BUFFER_VIEW NewIndexBufferView;
			NewIndexBufferView.BufferLocation = NewIndexBuffer->GetGPUVirtualAddress() + (sizeof(uint32) * Offset);
			NewIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			NewIndexBufferView.SizeInBytes = CurIdxDataCnt * sizeof(uint32);

			_IndexBufferView[i] = NewIndexBufferView;
			Offset += CurIdxDataCnt;
		}
	}
}

DX12GALMeshAssetWrapper::~DX12GALMeshAssetWrapper()
{
	if (_VertexBuffer != nullptr)
	{
		_VertexBuffer->Release();
	}

	if (_IndexBuffer != nullptr)
	{
		_IndexBuffer->Release();
	}
}
