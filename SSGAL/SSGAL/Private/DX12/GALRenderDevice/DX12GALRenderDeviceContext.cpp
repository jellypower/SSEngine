#include "pch.h"


#include "DX12GALRenderDevice.h"
#include "DX12GALRenderDeviceContext.h"

#include <SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h>

#include "Private/DX12/DX12CommonUtils/DDSTextureLoader12/DDSTextureLoader12.h"
#include "Private/DX12/GALRenderAsset/DX12GALTextureAssetWrapper.h"
#include "Private/DX12/GALRenderTarget/DX12GALCPUReadableTexture.h"

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

#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
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

bool DX12GALRenderDeviceContext::GenerateMeshGALAsset(IMeshAssetMutable* InMeshAsset)
{
	if (InMeshAsset->GetGALMeshAsset() != nullptr)
	{
		return false;
	}

	HRESULT hr = S_OK;

	DX12GALResourceUpdater* DX12ResourceUpdater = (DX12GALResourceUpdater*)_ResourceUpdater;
	DX12GALRenderDevice* OwnerDX12RenderDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice);
	ID3D12Device5* D3DDevice = OwnerDX12RenderDevice->GetD3DDevice();
	ID3D12GraphicsCommandList* CurCommandList = _CommandLists[_CurCommandListIdx];

	DX12GALMeshAssetWrapper* NewGALMeshAsset = DBG_NEW DX12GALMeshAssetWrapper(InMeshAsset, OwnerDX12RenderDevice);


	const MeshRawDataBase* MeshRawData = InMeshAsset->GetMeshRawData();
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
		
		int32 SubMeshCnt = DefaultMeshRawData->_subMeshCnt;
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

	InMeshAsset->InjectGALMeshAsset(NewGALMeshAsset);

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

bool DX12GALRenderDeviceContext::GenerateTextureGALAsset(ITextureAssetMutable* InTextureAsset)
{
	if (InTextureAsset->GetGALTextureAsset() != nullptr)
	{
		return false;
	}

	DX12GALTextureAssetWrapper* NewTextureAsset = nullptr;
	HRESULT hr = S_OK;

	DX12GALResourceUpdater* DX12ResourceUpdater = (DX12GALResourceUpdater*)_ResourceUpdater;
	DX12GALRenderDevice* OwnerDX12RenderDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice);
	SSCustomMemChunkAllocator* DescriptorTableAllocator = OwnerDX12RenderDevice->GetDescriptorTableAllocator();
	ID3D12Device5* D3DDevice = OwnerDX12RenderDevice->GetD3DDevice();
	ID3D12GraphicsCommandList* CurCommandList = _CommandLists[_CurCommandListIdx];

	const utf16* TexturePath = InTextureAsset->GetAssetPath().C_Str();
	const utf16* TextureName = InTextureAsset->GetAssetName().C_Str();


	ID3D12Resource* pTexResource = nullptr;
	D3D12_RESOURCE_DESC textureDesc = {};
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> subresouceData;
	if (FAILED(LoadDDSTextureFromFile(D3DDevice, TexturePath, &pTexResource, ddsData, subresouceData)))
	{
		DEBUG_BREAK();
		if (pTexResource != nullptr)
		{
			pTexResource->Release();
		}
		return false;
	}
	textureDesc = pTexResource->GetDesc();

	const D3D12_SUBRESOURCE_DATA* SrcData = subresouceData.data();
	UINT NumSubResources = (UINT)subresouceData.size();
	UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTexResource, 0, NumSubResources);
	
	
	hr = DX12ResourceUpdater->UpdateTexture(
		CurCommandList,
		pTexResource,
		SrcData,
		NumSubResources,
		uploadBufferSize,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		if (pTexResource != nullptr)
		{
			pTexResource->Release();
		}
		return false;
	}
	pTexResource->SetName(TextureName);

	AllocatedChunkHeader SRVDescriptorChunk = DescriptorTableAllocator->AllocChunk(1, InTextureAsset->GetAssetName());
	ID3D12DescriptorHeap* AllocatedDescHeap = (ID3D12DescriptorHeap*)SRVDescriptorChunk.PageContent;
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRVHandle(
		AllocatedDescHeap->GetCPUDescriptorHandleForHeapStart(),
		SRVDescriptorChunk.ChunkOffset,
		D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));


	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = textureDesc.Format;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	D3DDevice->CreateShaderResourceView(pTexResource, &SRVDesc, SRVHandle);


	NewTextureAsset = DBG_NEW DX12GALTextureAssetWrapper(InTextureAsset, OwnerDX12RenderDevice);
	NewTextureAsset->_TexResource = pTexResource;
	NewTextureAsset->_DescriptorTableChunk = SRVDescriptorChunk;
	NewTextureAsset->_SRVHandle = SRVHandle;

	InTextureAsset->InjectGALTextureAsset(NewTextureAsset);

	return true;
}

bool DX12GALRenderDeviceContext::GenerateMaterialGALAsset(IMaterialAsset* InMaterialAsset)
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

void DX12GALRenderDeviceContext::CopyRenderTarget(GALCPUReadableTexture* CopyDest, GALRenderTarget* CopySrc)
{
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentCmdList();
	DX12GALRenderDevice* OwnerDeviceDX12 = (DX12GALRenderDevice*)GetOwnerRenderDevice();
	ID3D12Device5* D3DDevice = OwnerDeviceDX12->GetD3DDevice();


	DX12GALCPUReadableTexture* DestReadableDX12 = (DX12GALCPUReadableTexture*)CopyDest;
	ID3D12Resource* DestRes = DestReadableDX12->GetCurrentResource();


	DX12GALRenderTargetBase* SrcRTDX12 = (DX12GALRenderTargetBase*)CopySrc;
	ID3D12Resource* SrcRes = SrcRTDX12->GetCurrentResource();


	UINT Rows = 0;
	UINT64 RowSize = 0;
	UINT64 TotalBytes = 0;
	D3D12_RESOURCE_DESC SrcDesc = SrcRes->GetDesc();
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT SrcFootprint;
	D3DDevice->GetCopyableFootprints(
		&SrcDesc,
		0,
		1,
		0,
		&SrcFootprint,
		&Rows,
		&RowSize,
		&TotalBytes
	);


	D3D12_BOX box;
	box.front = 0;
	box.back = 1;
	box.left = 0;
	box.right = DestReadableDX12->GetResourceWidthHeight().X;
	box.top = 0;
	box.bottom = DestReadableDX12->GetResourceWidthHeight().Y;

	D3D12_TEXTURE_COPY_LOCATION	destLocation;
	destLocation.PlacedFootprint = SrcFootprint;
	destLocation.pResource = DestRes;
	destLocation.SubresourceIndex = 0;
	destLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

	D3D12_TEXTURE_COPY_LOCATION	srcLocation;
	srcLocation.pResource = SrcRes;
	srcLocation.SubresourceIndex = 0;
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	CurCommandList->CopyTextureRegion(&destLocation, box.left, box.top, 0, &srcLocation, &box);
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
		DrawStaticMesh((IRIMesh*)InRenderInstance, ObjTransformMat, ObjRotMat);
	}
	break;

	default:
		SS_INTERRUPT();
		return;
	}
}

void DX12GALRenderDeviceContext::DrawID(IRenderInstance* InRenderInstance)
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
		DrawStaticMeshID((IRIMesh*)InRenderInstance, ObjTransformMat, ObjRotMat);
	}
	break;

	default:
		SS_INTERRUPT();
		return;
	}
}

void DX12GALRenderDeviceContext::DrawStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat)
{
	DX12GALRIMetadata_SM* DX12RenderInstanceMetaData = (DX12GALRIMetadata_SM*)RIToDraw->GetGALMetadata();
	IModelAsset* InModelAsset = RIToDraw->GetModelAsset();

	PCommonGALRenderDevice* OwnerDevice = (PCommonGALRenderDevice*)GetOwnerRenderDevice();
	RootSignaturePool* lRootSignaturePool = OwnerDevice->GetRootSignaturePool();
	DX12PSOPool* PSOPool = (DX12PSOPool*)OwnerDevice->GetPSOPool();
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentCmdList();
	ID3D12DescriptorHeap* RenderInstanceDescHeap = (ID3D12DescriptorHeap*)DX12RenderInstanceMetaData->_DescriptorTableChunk.PageContent;


	IMeshAsset* lMeshAsset = InModelAsset->GetMeshAsset();
	const DX12GALMeshAssetWrapper* GALMeshAsset = (const DX12GALMeshAssetWrapper*)lMeshAsset->GetGALMeshAsset();
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;
	const MeshRawDataBase* MeshRawData = lMeshAsset->GetMeshRawData();
	const MeshRawDataDefault* DefaultMeshRawData = nullptr;
	int32 SubMeshCnt = 0;
	switch (MeshRawData->_MeshType)
	{
	case EMeshType::Rigid:
	case EMeshType::Skinned:
		DefaultMeshRawData = (MeshRawDataDefault*)MeshRawData;
		SubMeshCnt = DefaultMeshRawData->_subMeshCnt;
		break;

	default:
		SS_ASSERT(false);
		break;
	}


	{
		PipelineDesc NewPipelineDesc;
		NewPipelineDesc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT;
		NewPipelineDesc.VSName = L"VS_SMToDefaultPSInput";
		NewPipelineDesc.PSName = L"PS_TestDrawer";
		NewPipelineDesc.RootSignatureType = ERootSignatureType::SS_TEMP_ROOTSIGNATURE;
		NewPipelineDesc.RTColorFormat = ERTColorFormat::R8G8B8A8_UNORM;
		NewPipelineDesc.DSColorFormat = ERTColorFormat::D32_FLOAT;
		const DX12PSOWrapper* lDX12PSOWrapper = (const DX12PSOWrapper*)PSOPool->FindOrAddPSO(NewPipelineDesc);

		const RootSignatureWrapper* RootSignatureWrapper = lRootSignaturePool->GetRootSignature(NewPipelineDesc.RootSignatureType);
		const DX12RootSignatureWrapper* lDX12RootSignatureWrapper = (const DX12RootSignatureWrapper*)RootSignatureWrapper;


		CurCommandList->SetGraphicsRootSignature(lDX12RootSignatureWrapper->GetRootSignatureInstantce());
		CurCommandList->SetPipelineState(lDX12PSOWrapper->GetPipelineState());
	}

	CurCommandList->SetDescriptorHeaps(1, &RenderInstanceDescHeap);
	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);


	{
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->WMatrix = XMMatrixTranspose(DrawMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->RotMatrix = XMMatrixTranspose(DrawRotMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->ObjectID = RIToDraw->GetGameObjectID().GetNativeValue();

		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->VPMatrix = _CameraVPTransform;
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->SunDirection = { 1,1,1,0 };
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->SunIntensity = { 1,1,1,0 };
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->ViewerPos = _CameraPosition;
	}

	CurCommandList->SetGraphicsRootConstantBufferView(0, DX12RenderInstanceMetaData->_ModelCBGPUMemAddr);
	CurCommandList->SetGraphicsRootConstantBufferView(1, DX12RenderInstanceMetaData->_RenderEnvCBGPUMemAddr);


	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);

		int32 CurIdxDataCnt = DefaultMeshRawData->_indexDataCnt[i];
		// CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, IdxDataOffset, 0, 0); => IdxDataOffset이 이미 GALMeshAsset->_IndexBufferView에 포함돼있어서 안넣어줘도 됨
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
	}
}

void DX12GALRenderDeviceContext::DrawStaticMeshID(IRIMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat)
{
	DX12GALRIMetadata_SM* DX12RenderInstanceMetaData = (DX12GALRIMetadata_SM*)RIToDraw->GetGALMetadata();
	IModelAsset* InModelAsset = RIToDraw->GetModelAsset();

	PCommonGALRenderDevice* OwnerDevice = (PCommonGALRenderDevice*)GetOwnerRenderDevice();
	RootSignaturePool* lRootSignaturePool = OwnerDevice->GetRootSignaturePool();
	DX12PSOPool* PSOPool = (DX12PSOPool*)OwnerDevice->GetPSOPool();
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentCmdList();
	ID3D12DescriptorHeap* RenderInstanceDescHeap = (ID3D12DescriptorHeap*)DX12RenderInstanceMetaData->_DescriptorTableChunk.PageContent;


	IMeshAsset* lMeshAsset = InModelAsset->GetMeshAsset();
	const DX12GALMeshAssetWrapper* GALMeshAsset = (const DX12GALMeshAssetWrapper*)lMeshAsset->GetGALMeshAsset();
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;
	const MeshRawDataBase* MeshRawData = lMeshAsset->GetMeshRawData();
	const MeshRawDataDefault* DefaultMeshRawData = nullptr;
	int32 SubMeshCnt = 0;
	switch (MeshRawData->_MeshType)
	{
	case EMeshType::Rigid:
	case EMeshType::Skinned:
		DefaultMeshRawData = (MeshRawDataDefault*)MeshRawData;
		SubMeshCnt = DefaultMeshRawData->_subMeshCnt;
		break;

	default:
		SS_ASSERT(false);
		break;
	}


	{
		PipelineDesc NewPipelineDesc;
		NewPipelineDesc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT;
		NewPipelineDesc.VSName = L"VS_SMToDefaultPSInput";
		NewPipelineDesc.PSName = L"PS_IDDrawer";
		NewPipelineDesc.RootSignatureType = ERootSignatureType::SS_TEMP_ROOTSIGNATURE;
		NewPipelineDesc.RTColorFormat = ERTColorFormat::R32G32_SINT;
		NewPipelineDesc.DSColorFormat = ERTColorFormat::D32_FLOAT;
		const DX12PSOWrapper* lDX12PSOWrapper = (const DX12PSOWrapper*)PSOPool->FindOrAddPSO(NewPipelineDesc);

		const RootSignatureWrapper* RootSignatureWrapper = lRootSignaturePool->GetRootSignature(NewPipelineDesc.RootSignatureType);
		const DX12RootSignatureWrapper* lDX12RootSignatureWrapper = (const DX12RootSignatureWrapper*)RootSignatureWrapper;


		CurCommandList->SetGraphicsRootSignature(lDX12RootSignatureWrapper->GetRootSignatureInstantce());
		CurCommandList->SetPipelineState(lDX12PSOWrapper->GetPipelineState());
	}

	CurCommandList->SetDescriptorHeaps(1, &RenderInstanceDescHeap);
	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);


	{
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->WMatrix = XMMatrixTranspose(DrawMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->RotMatrix = XMMatrixTranspose(DrawRotMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->ObjectID = RIToDraw->GetGameObjectID().GetNativeValue();

		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->VPMatrix = _CameraVPTransform;
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->SunDirection = { 1,1,1,0 };
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->SunIntensity = { 1,1,1,0 };
		DX12RenderInstanceMetaData->_RenderEnvCBSysMemAddr->ViewerPos = _CameraPosition;
	}

	CurCommandList->SetGraphicsRootConstantBufferView(0, DX12RenderInstanceMetaData->_ModelCBGPUMemAddr);
	CurCommandList->SetGraphicsRootConstantBufferView(1, DX12RenderInstanceMetaData->_RenderEnvCBGPUMemAddr);


	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);

		int32 CurIdxDataCnt = DefaultMeshRawData->_indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
	}
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
