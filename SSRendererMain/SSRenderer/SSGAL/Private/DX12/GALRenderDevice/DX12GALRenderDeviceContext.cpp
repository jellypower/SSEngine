#include "DX12GALRenderDevice.h"
#include "DX12GALRenderDeviceContext.h"

#include "SSGAL/Private/DX12/GALRenderAsset/DX12GALMeshAssetWrapper.h"
#include "SSGAL/Private/DX12/GALRenderInstance/DX12GALRIMetadata_SM.h"

#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALRenderTargetBase.h"

#include "SSGAL/Private/DX12/GALWrapper/DX12PSOPool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOWrapper.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12RootSignaturePool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12RootSignatureWrapper.h"
#include "SSGAL/Private/DX12/GALResourceUpdater/DX12GALResourceUpdater.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBAModelBuffer.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBARenderEnvParam.h"
#include "SSRenderer/Private/RenderInstance/RIStaticMesh.h"

#include "SSRenderer/Public/RenderAsset/MaterialAssetManager.h"
#include "SSRenderer/Public/RenderAsset/MeshAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"


DX12GALRenderDeviceContext::DX12GALRenderDeviceContext(DX12GALRenderDevice* InRenderDevice, int32 InitialCommandListCnt)
{
	_OwnerRenderDevice = InRenderDevice;

	ID3D12Device5* D3DDevice = InRenderDevice->GetD3DDevice();

	_CommandAllocators.Reserve(InitialCommandListCnt * 2);
	_CommandLists.Reserve(InitialCommandListCnt * 2);


	ID3D12CommandAllocator* NewCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* NewCommandList = nullptr;
	for (int32 i = 0; i < InitialCommandListCnt; i++)
	{
		NewCommandAllocator = nullptr;
		NewCommandList = nullptr;
		if (FAILED(D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&NewCommandAllocator))))
		{
			DEBUG_BREAK();
			goto lb_cleanup;
		}

		if (FAILED(D3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, NewCommandAllocator, nullptr, IID_PPV_ARGS(&NewCommandList))))
		{
			DEBUG_BREAK();
			goto lb_cleanup;
		}


		NewCommandList->Close();

		_CommandAllocators.PushBack(NewCommandAllocator);
		_CommandLists.PushBack(NewCommandList);
	}

	_ResourceUpdater = DBG_NEW DX12GALResourceUpdater(InRenderDevice, this);

	return;

lb_cleanup:
	if (_ResourceUpdater != nullptr)
	{
		delete _ResourceUpdater;
		_ResourceUpdater = nullptr;
	}

	if (NewCommandList)
	{
		NewCommandList->Release();
	}

	if (NewCommandAllocator)
	{
		NewCommandAllocator->Release();
	}

	for (ID3D12GraphicsCommandList* CommandListItem : _CommandLists)
	{
		CommandListItem->Release();
	}

	for (ID3D12CommandAllocator* CommandAllocator : _CommandAllocators)
	{
		CommandAllocator->Release();
	}

	_CommandLists.Resize(0);
	_CommandAllocators.Resize(0);
}

DX12GALRenderDeviceContext::~DX12GALRenderDeviceContext()
{
	for (ID3D12CommandList* CommandListItem : _CommandLists)
	{
		CommandListItem->Release();
	}

	for (ID3D12CommandAllocator* AllocatorItem : _CommandAllocators)
	{
		AllocatorItem->Release();	
	}

	_CommandLists.Resize(0);
	_CommandAllocators.Resize(0);

	delete _ResourceUpdater;
}

bool DX12GALRenderDeviceContext::IsValid() const
{
	return _CommandLists.GetSize() != 0;
}

bool DX12GALRenderDeviceContext::GenerateMeshGALAsset(MeshAsset* InMeshAsset)
{
	if (InMeshAsset->_GALMeshAsset != nullptr)
	{
		return false;
	}

	HRESULT hr = S_OK;

	DX12GALResourceUpdater* DX12ResourceUpdater = (DX12GALResourceUpdater*)_ResourceUpdater;
	DX12GALRenderDevice* OwnerDX12RenderDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice);
	ID3D12Device5* D3DDevice = OwnerDX12RenderDevice->GetD3DDevice();
	ID3D12GraphicsCommandList* CurCommandList = _CommandLists[_CurCommandListIdx];

	DX12GALMeshAssetWrapper* NewGALMeshAsset = DBG_NEW DX12GALMeshAssetWrapper(InMeshAsset, OwnerDX12RenderDevice);


	const IMeshRawData* MeshRawData = InMeshAsset->GetMeshRawData();
	const MeshRawDataDefault* DefaultMeshRawData = nullptr;
	switch (MeshRawData->_MeshType)
	{
	case EMeshType::Rigid:
	case EMeshType::Skinned:
		DefaultMeshRawData = (MeshRawDataDefault*)MeshRawData;
		break;

	default:
		SS_ASSERT(false);
		break;
	}
	
	{
		int32 EachVertexSize = DefaultMeshRawData->_eachVertexSize;
		int32 VertexCnt = DefaultMeshRawData->_vertexCnt;
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
			DEBUG_BREAK();
			goto lb_fail;
		}
		NewVertexBuffer->SetName(InMeshAsset->GetAssetName().C_Str());

		hr = DX12ResourceUpdater->UpdateBuffer(CurCommandList, NewVertexBuffer, VertexData, VertexBufferSize,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		if (FAILED(hr))
		{
			DEBUG_BREAK();
			goto lb_fail;
		}

		D3D12_VERTEX_BUFFER_VIEW NewVertexBufferView;
		NewVertexBufferView.BufferLocation = NewVertexBuffer->GetGPUVirtualAddress();
		NewVertexBufferView.StrideInBytes = EachVertexSize;
		NewVertexBufferView.SizeInBytes = VertexBufferSize;

		NewGALMeshAsset->_VertexBuffer = NewVertexBuffer;
		NewGALMeshAsset->_VertexBufferView = NewVertexBufferView;
	}

	{
		
		int32 SubMeshCnt = DefaultMeshRawData->_subMeshCnt;;
		NewGALMeshAsset->_SubMeshCnt = SubMeshCnt;
		int32 WholeIdxDataCnt = DefaultMeshRawData->_wholeIndexDataCnt;
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
			DEBUG_BREAK();
			goto lb_fail;
		}
		NewIndexBuffer->SetName(InMeshAsset->GetAssetName().C_Str());

		hr = DX12ResourceUpdater->UpdateBuffer(CurCommandList, NewIndexBuffer, IndexData, WholeIndexBufferSize,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		if (FAILED(hr))
		{
			DEBUG_BREAK();
			goto lb_fail;
		}

		NewGALMeshAsset->_IndexBuffer = NewIndexBuffer;

		int32 CurIdxDataCnt = 0;
		int32 Offset = 0;
		for (int32 i = 0; i < SubMeshCnt; i++)
		{
			CurIdxDataCnt = DefaultMeshRawData->_indexDataCnt[i];

			D3D12_INDEX_BUFFER_VIEW NewIndexBufferView;
			NewIndexBufferView.BufferLocation = NewIndexBuffer->GetGPUVirtualAddress() + (sizeof(uint32) * Offset);
			NewIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			NewIndexBufferView.SizeInBytes = CurIdxDataCnt * sizeof(uint32);

			NewGALMeshAsset->_IndexBufferView[i] = NewIndexBufferView;

			Offset += CurIdxDataCnt;
		}
	}

	InMeshAsset->_GALMeshAsset = NewGALMeshAsset;

	return true;

lb_fail:
	if (NewGALMeshAsset != nullptr)
	{
		if (NewGALMeshAsset->_VertexBuffer != nullptr)
		{
			NewGALMeshAsset->_VertexBuffer->Release();
		}

		if (NewGALMeshAsset->_IndexBuffer != nullptr)
		{
			NewGALMeshAsset->_IndexBuffer->Release();
		}

		delete NewGALMeshAsset;
	}

	return false;
}

bool DX12GALRenderDeviceContext::GenerateMaterialGALAsset(MaterialAsset* InMaterialAsset)
{


	SS_INTERRUPT();
	return false;
}

void DX12GALRenderDeviceContext::GenerateRenderInstanceMetadata(IRenderInstance* InRenderInstance)
{
	if (InRenderInstance->GetRIType() == ERenderInstanceType::StaticMesh)
	{
		IRIMesh* InIRIMesh = (IRIMesh*)InRenderInstance;
		DX12GALRIMetadata_SM* NewGALRI = DBG_NEW DX12GALRIMetadata_SM(_OwnerRenderDevice, InIRIMesh);
		InIRIMesh->InjectGALMetadataXXX(NewGALRI);
	}
	else
	{
		DEBUG_BREAK();
		return;
	}
}

void DX12GALRenderDeviceContext::ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From,
	EResourceStateType To)
{
	InRenderTarget->ResourceBarrier(this, From, To);
}

void DX12GALRenderDeviceContext::SetRenderTarget(GALRenderTarget* InRenderTarget)
{
	DX12GALRenderTargetBase* DX12RenderTarget = (DX12GALRenderTargetBase*)InRenderTarget;
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentCmdList();

	DX12RenderTarget->SetRenderTarget(CurCommandList);
}

void DX12GALRenderDeviceContext::ClearRenderTarget(GALRenderTarget* InRenderTarget)
{
	DX12GALRenderTargetBase* DX12RenderTarget = (DX12GALRenderTargetBase*)InRenderTarget;
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentCmdList();

	DX12RenderTarget->ClearRenderTarget(CurCommandList);
}

void DX12GALRenderDeviceContext::Draw(IRenderInstance* InRenderInstance)
{
	if (InRenderInstance->GetGALMetadata() == nullptr)
	{
		GenerateRenderInstanceMetadata(InRenderInstance);
	}

	XMMATRIX ObjTransformMat = InRenderInstance->GetWorldTransformMatrix();
	XMMATRIX ObjRotMat = InRenderInstance->GetWorldRotationMatrix();


	switch (InRenderInstance->GetRIType())
	{
	case ERenderInstanceType::StaticMesh:
	{
		IRIMesh* RIMesh = (IRIMesh*)InRenderInstance;
		TEMP_DrawStaticMesh(RIMesh->GetModelAsset(), (DX12GALRIMetadata_SM*)RIMesh->GetGALMetadata(), ObjTransformMat, ObjRotMat);
	}
	break;

	default:
		SS_INTERRUPT();
		return;
	}
}

void DX12GALRenderDeviceContext::TEMP_DrawStaticMesh(
	ModelAsset* InModelAsset,
	DX12GALRIMetadata_SM* DX12RenderInstanceMetaData,
	const XMMATRIX& DrawMat,
	const XMMATRIX& DrawRotMat)
{
	PCommonGALRenderDevice* OwnerDevice = (PCommonGALRenderDevice*)GetOwnerRenderDevice();
	IRenderer* Renderer = OwnerDevice->GetOwnerRenderer();
	RootSignaturePool* lRootSignaturePool = OwnerDevice->GetRootSignaturePool();
	DX12PSOPool* PSOPool = (DX12PSOPool*)OwnerDevice->GetPSOPool();
	MeshAssetManager* MeshAssetManager = Renderer->GetMeshAssetManager();
	MaterialAssetManager* MaterialAssetManager = Renderer->GetMaterialAssetManager();
	SS::SHasherW MeshAssetName = InModelAsset->GetMeshAssetName();
	int32 SubMeshCnt = InModelAsset->GetSubMeshCnt();
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentCmdList();
	ID3D12DescriptorHeap* RenderInstanceDescHeap = (ID3D12DescriptorHeap*)DX12RenderInstanceMetaData->_DescriptorTableChunk.PageContent;


	MeshAsset* lMeshAsset = (MeshAsset*)MeshAssetManager->FindAssetByName(MeshAssetName);
	DX12GALMeshAssetWrapper* GALMeshAsset = (DX12GALMeshAssetWrapper*)lMeshAsset->_GALMeshAsset;
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;
	const IMeshRawData* MeshRawData = lMeshAsset->GetMeshRawData();
	const MeshRawDataDefault* DefaultMeshRawData = nullptr;
	switch (MeshRawData->_MeshType)
	{
	case EMeshType::Rigid:
	case EMeshType::Skinned:
		DefaultMeshRawData = (MeshRawDataDefault*)MeshRawData;
		break;

	default:
		SS_ASSERT(false);
		break;
	}


	const RootSignatureWrapper* RootSignatureWrapper = lRootSignaturePool->GetRootSignature(ERootSignatureType::SS_TEMP_ROOTSIGNATURE);
	const DX12RootSignatureWrapper* lDX12RootSignatureWrapper = (const DX12RootSignatureWrapper*)RootSignatureWrapper;


	PipelineDesc NewPipelineDesc;
	NewPipelineDesc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT;
	NewPipelineDesc.VSName = L"LambertShaderVS";
	NewPipelineDesc.PSName = L"LambertShaderPS";
	NewPipelineDesc.RootSignatureType = ERootSignatureType::SS_TEMP_ROOTSIGNATURE;
	const DX12PSOWrapper* lDX12PSOWrapper = (const DX12PSOWrapper * )PSOPool->FindOrAddPSO(NewPipelineDesc);


	CurCommandList->SetGraphicsRootSignature(lDX12RootSignatureWrapper->GetRootSignatureInstantce());
	CurCommandList->SetDescriptorHeaps(1, &RenderInstanceDescHeap);
	CurCommandList->SetPipelineState(lDX12PSOWrapper->GetPipelineState());
	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);


	{
//		Transform TEMPDrawTransform;
//		TEMPDrawTransform.Position = Vector4f(2, 0, 0, 0);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->WMatrix = XMMatrixTranspose(DrawMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->RotMatrix = XMMatrixTranspose(DrawRotMat);


		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->VPMatrix = _CameraVPTransform;
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->SunDirection = { 1,1,1,0 };
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->SunIntensity = { 1,1,1,0 };
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->ViewerPos = _CameraPosition;
	}

	CurCommandList->SetGraphicsRootConstantBufferView(0, DX12RenderInstanceMetaData->_ModelCBGPUMemAddr);
	CurCommandList->SetGraphicsRootConstantBufferView(1, DX12RenderInstanceMetaData->_RenderEnvCBGPUMemAddr);


	int32 IdxDataOffset = 0;
	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);

		int32 CurIdxDataCnt = DefaultMeshRawData->_indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, IdxDataOffset, 0, 0);
		IdxDataOffset += CurIdxDataCnt;
	}

	// TODO: 25/05/13
	// 셰이더를 에셋으로 간주하고 외부에서 넣어주는 형태 -> 괜찮음
	// 메테리얼은 하드코딩 하는게 좋아보인다.
	// EMaterialType을 만들고 이를 포함하는 기본 MaterialAsset타입을 만든다(베이스로)
	// 그리고 여기서 파생하는 하드코딩된 다양한 메테리얼들 ModelMaterial, ParticleMaterial ...을 만든다.
	// 메테리얼이 셰이더를 직접적으로 지정하지 않도록 바꾼다.
	// VSName, PSName이 아니라 메테리얼 타입으로 PipelineDesc를 지정할 수 있도록 바꾼다.
	// 계속 생각해보자. 근데 메테리얼 자체를 하드코딩 하는 방향성 자체는 맞는것 같음.

}

void DX12GALRenderDeviceContext::ResetRenderState()
{
	_ResourceUpdater->ResetUpdateBuffer();
	ResetCommandList();
}

void DX12GALRenderDeviceContext::ResetCommandList()
{
	HRESULT hr;

	for (int32 i = 0; i <= _CurCommandListIdx; i++)
	{
		ID3D12GraphicsCommandList* CurCommandList = _CommandLists[i];
		ID3D12CommandAllocator* CurCommandAllcator = _CommandAllocators[i];

		hr = CurCommandAllcator->Reset();
		if (FAILED(hr)) DEBUG_BREAK();
		hr = CurCommandList->Reset(CurCommandAllcator, nullptr);
		if (FAILED(hr)) DEBUG_BREAK();
	}

	_CurCommandListIdx = 0;
}

void DX12GALRenderDeviceContext::BeginRender()
{
	ResetRenderState();
}

void DX12GALRenderDeviceContext::EndRender()
{
	HRESULT hr;

	for (int32 i = 0; i <= _CurCommandListIdx; i++)
	{
		ID3D12GraphicsCommandList* CurCommandList = _CommandLists[i];

		hr = CurCommandList->Close();
		if (FAILED(hr)) SS_INTERRUPT();
	}
}
