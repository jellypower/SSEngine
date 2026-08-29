#include "pch.h"


#include "DX12GALSimpleLineMeshAssetWrapper.h"


#include "SSRenderer/Public/RenderCommon/SSRenderUtilFuncs.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSimpleLine.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "Private/DX12/GALResourceUpdater/DX12GALResourceUpdater.h"


DX12GALSimpleLineMeshAssetWrapper::DX12GALSimpleLineMeshAssetWrapper(IMeshAsset* ownerMeshAsset, DX12GALRenderDeviceContext* ResourceUpdateExecutor)
{
	_OwnerMeshAsset = ownerMeshAsset;
	_OwnerRenderDevice = static_cast<DX12GALRenderDevice*>(ResourceUpdateExecutor->GetOwnerRenderDevice());

	HRESULT hr = S_OK;

	int32 CurFrameMod = RenderFrameInfo::GetFrameMod();

	DX12GALResourceUpdater* DX12ResourceUpdater = static_cast<DX12GALResourceUpdater*>(ResourceUpdateExecutor->GetResourceUpdater(CurFrameMod));
	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();
	ID3D12GraphicsCommandList* CurCommandList = ResourceUpdateExecutor->GetCurrentDrawWorkerCmdList();

	const MeshRawDataBase* MeshRawData = _OwnerMeshAsset->GetMeshRawData();
	SS::SHasherW MeshName = _OwnerMeshAsset->GetAssetName();

	if (MeshRawData->GetMeshType() != EMeshType::SimpleLine)
	{
		SS_INTERRUPT();
		return;
	}
	const MeshRawDataSimpleLine* LineRawData = static_cast<const MeshRawDataSimpleLine*>(MeshRawData);

	// Create Vertex buffer and Update
	{
		uint64 VertexBufferSize = sizeof(SimpleLineVertex) * LineRawData->_vertexCnt;

		ID3D12Resource* NewVertexBuffer = nullptr;

		CD3DX12_HEAP_PROPERTIES DefaultHeapProp(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
		hr = D3DDevice->CreateCommittedResource(
			&DefaultHeapProp,
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

		hr = DX12ResourceUpdater->UpdateBuffer(CurCommandList, NewVertexBuffer, LineRawData->_vertexData, VertexBufferSize,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		if (FAILED(hr))
		{
			SS_INTERRUPT();
			return;
		}

		_VertexBuffer = NewVertexBuffer;
		_VertexBufferView.BufferLocation = NewVertexBuffer->GetGPUVirtualAddress();
		_VertexBufferView.StrideInBytes = sizeof(SimpleLineVertex);
		_VertexBufferView.SizeInBytes = (UINT)VertexBufferSize;
	}

	// Create Index buffer and Update
	{
		uint64 IndexBufferSize = sizeof(uint32) * LineRawData->_IdxCnt;

		ID3D12Resource* NewIndexBuffer = nullptr;

		CD3DX12_HEAP_PROPERTIES DefaultHeapProp(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(IndexBufferSize);
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

		hr = DX12ResourceUpdater->UpdateBuffer(CurCommandList, NewIndexBuffer, LineRawData->_indexData, IndexBufferSize,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		if (FAILED(hr))
		{
			SS_INTERRUPT();
			return;
		}

		_IndexBuffer = NewIndexBuffer;
		_IndexBufferView.BufferLocation = NewIndexBuffer->GetGPUVirtualAddress();
		_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		_IndexBufferView.SizeInBytes = (UINT)IndexBufferSize;
		_IndexCnt = LineRawData->_IdxCnt;
	}
}

void DX12GALSimpleLineMeshAssetWrapper::Release()
{
	if (_VertexBuffer != nullptr)
	{
		_VertexBuffer->Release();
		_VertexBuffer = nullptr;
	}

	if (_IndexBuffer != nullptr)
	{
		_IndexBuffer->Release();
		_IndexBuffer = nullptr;
	}

	delete this;
}