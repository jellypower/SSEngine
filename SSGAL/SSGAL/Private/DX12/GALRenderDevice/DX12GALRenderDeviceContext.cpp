#include "pch.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "DX12GALRenderDevice.h"
#include "DX12GALRenderDeviceContext.h"


#include "Private/DX12/GALRenderInstance/DX12GALRIDirectionalLightShadowMapMetadata.h"
#include "SSGAL/Private/DX12/GALRenderInstance/DX12GALRWMetaData.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALDSVRenderTarget.h"
#include "SSGAL/Private/PCommon/GALPrivateGlobals.h"
#include "SSGAL/Private/DX12/DX12CommonUtils/DDSTextureLoader12/DDSTextureLoader12.h"
#include "SSGAL/Private/DX12/GALRenderAsset/DX12GALTextureAssetWrapper.h"
#include "SSGAL/Private/DX12/GALRenderAsset/GALMaterialAssets/DX12GALDefaultPBRMaterialAsset.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALCPUReadableTexture.h"
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

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataBase.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"
#include "SSRenderer/Public/RenderInstance/Light/IRenderLightDirectional.h"
#include "SSRenderer/Public/RenderInstance/Descriptors/LightDesc.h"
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"



DX12GALRenderDeviceContext::DX12GALRenderDeviceContext(DX12GALRenderDevice* InRenderDevice, int32 InitialCommandListCnt):
	_BoundRenderTargets(RT_NUM_MAX),
	_RenderLightsToDraw(32)
{
	_OwnerRenderDevice = InRenderDevice;

	ID3D12Device5* D3DDevice = InRenderDevice->GetD3DDevice();

	_CommandAllocators.Reserve(InitialCommandListCnt * 2);
	_DrawWorkerCommandLists.Reserve(InitialCommandListCnt * 2);


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
		_DrawWorkerCommandLists.PushBack(NewCommandList);
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

	for (ID3D12GraphicsCommandList* CommandListItem : _DrawWorkerCommandLists)
	{
		CommandListItem->Release();
	}

	for (ID3D12CommandAllocator* CommandAllocator : _CommandAllocators)
	{
		CommandAllocator->Release();
	}

	_DrawWorkerCommandLists.Resize(0);
	_CommandAllocators.Resize(0);
}

DX12GALRenderDeviceContext::~DX12GALRenderDeviceContext()
{
	for (ID3D12CommandList* CommandListItem : _DrawWorkerCommandLists)
	{
		CommandListItem->Release();
	}

	for (ID3D12CommandAllocator* AllocatorItem : _CommandAllocators)
	{
		AllocatorItem->Release();	
	}

	_DrawWorkerCommandLists.Resize(0);
	_CommandAllocators.Resize(0);

	delete _ResourceUpdater;
}

bool DX12GALRenderDeviceContext::IsValid() const
{
	return _DrawWorkerCommandLists.GetSize() != 0;
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
	ID3D12GraphicsCommandList* CurCommandList = _DrawWorkerCommandLists[_CurCommandListIdx];

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
	SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = OwnerDX12RenderDevice->GetDescriptorTableAllocatorForTex();
	ID3D12Device5* D3DDevice = OwnerDX12RenderDevice->GetD3DDevice();
	ID3D12GraphicsCommandList* CurCommandList = _DrawWorkerCommandLists[_CurCommandListIdx];

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

	AllocatedChunkHeader SRVDescriptorChunk = DescriptorTableAllocatorForTex->AllocChunk(1, InTextureAsset->GetAssetName());
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

bool DX12GALRenderDeviceContext::GenerateMaterialGALAsset(IMaterialAssetMutable* InMaterialAsset)
{
	DX12GALRenderDevice* OwnerDevice = (DX12GALRenderDevice*)GetOwnerRenderDevice();

	if (InMaterialAsset == nullptr)
	{
		return false;
	}

	const MtlDataBase* MtlData = InMaterialAsset->GetMtlData();

	if (MtlData->_Type == EMaterialType::DefaultPBR)
	{
		GALMaterialAssetWrapperBase* GALMaterial = DBG_NEW DX12GALDefaultPBRMaterialAsset(InMaterialAsset, OwnerDevice);
		InMaterialAsset->InjectGALMaterialAsset(GALMaterial);
		return true;
	}
	else
	{
		SS_ASSERT(false);
		return false;
	}

	
	return false;
}

void DX12GALRenderDeviceContext::GenerateRenderInstanceMetadata(IRenderInstance* InRenderInstance)
{
	ERenderInstanceType RIType = InRenderInstance->GetRIType();

	if (RIType == ERenderInstanceType::StaticMesh)
	{
		IRIMesh* InIRIMesh = (IRIMesh*)InRenderInstance;
		DX12GALRIMetadata_SM* NewGALRI = DBG_NEW DX12GALRIMetadata_SM(_OwnerRenderDevice, InIRIMesh);
		InIRIMesh->InjectGALMetadataXXX(NewGALRI);
	}
	else if (RIType == ERenderInstanceType::Light)
	{
		IRenderLight* InRenderLight = (IRenderLight*)InRenderInstance;

		ELightType LightType = InRenderLight->GetLightType();
		if (LightType == ELightType::Directional)
		{
			IRenderLightDirectional* DirectionalLight = static_cast<IRenderLightDirectional*>(InRenderLight);
			const RenderLightDirectionalDesc& Desc = DirectionalLight->GetDirectionalLightDesc();

			if (Desc.bEnableShadowMap)
			{
				DX12GALRIDirectionalLightShadowMapMetadata* NewShadowMapMetadata = 
					DBG_NEW DX12GALRIDirectionalLightShadowMapMetadata((DX12GALRenderDevice*)_OwnerRenderDevice, DirectionalLight);

				DirectionalLight->InjectGALMetadataXXX(NewShadowMapMetadata);
			}
		}
		else
		{
			SS_ASSERT(false);
		}
	}
	else
	{
		DEBUG_BREAK();
		return;
	}
}

void DX12GALRenderDeviceContext::BeginDrawShadowMap(IRenderLight* InLightToDrawShadowMap)
{
	if (_DrawingShadowMapMetadata != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	bool bIsAddedLight = false;
	for (IRenderLight* AddedRenderLightItem : _RenderLightsToDraw)
	{
		if (AddedRenderLightItem == InLightToDrawShadowMap)
		{
			bIsAddedLight = true;
			break;
		}
	}

	if (bIsAddedLight == false)
	{
		SS_ASSERT(false);
		return;
	}


	ELightType LightType = InLightToDrawShadowMap->GetLightType();

	if (LightType == ELightType::Directional)
	{
		IRenderLightDirectional* DirectionalLight = 
			static_cast<IRenderLightDirectional*>(InLightToDrawShadowMap);

		DX12GALRIDirectionalLightShadowMapMetadata* DirectionalLightShadowMapMetadata =
			static_cast<DX12GALRIDirectionalLightShadowMapMetadata*>(DirectionalLight->GetGALMetadata());

		_DrawingShadowMapMetadata = DirectionalLightShadowMapMetadata;

		DirectionalLightShadowMapMetadata->_ShadowMapCBSysMemAddr->VPMatrix =
			XMMatrixTranspose(DirectionalLight->CalcShadowMapVPMatrix(_CurRenderCamera));

		

		GALRenderTarget* ShadowMap = DirectionalLightShadowMapMetadata->GetShadowMap();

		ResourceBarrier(ShadowMap, EResourceStateType::Common, EResourceStateType::DepthWrite);
		ClearRenderTarget(ShadowMap);
		SetRenderTarget(0, nullptr, ShadowMap);
	}
	else
	{
		SS_ASSERT(false);
	}
}

void DX12GALRenderDeviceContext::EndDrawShadowMap()
{
	ELightType LightType = _DrawingShadowMapMetadata->GetLightType();

	if (LightType == ELightType::Directional)
	{
		DX12GALRIDirectionalLightShadowMapMetadata* DirectionalLightShadowMapMetadata =
			static_cast<DX12GALRIDirectionalLightShadowMapMetadata*>(_DrawingShadowMapMetadata);

		GALRenderTarget* ShadowMap = DirectionalLightShadowMapMetadata->GetShadowMap();
		ResourceBarrier(ShadowMap, EResourceStateType::DepthWrite, EResourceStateType::Common);
	}


	_DrawingShadowMapMetadata = nullptr;
}

void DX12GALRenderDeviceContext::SetRenderCamera(IRenderCamera* InCamera)
{
	_CurRenderCamera = InCamera;

	DX12GALRenderDevice* OwnerDX12RenderDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice);

	IRenderWorld* CurRenderWorld = InCamera->GetIcludedRenderWorld();
	_CurRenderWorldGALData = (DX12GALRWMetaData*)CurRenderWorld->GetGALMetadata();
	if (_CurRenderWorldGALData == nullptr)
	{
		_CurRenderWorldGALData = DBG_NEW DX12GALRWMetaData(OwnerDX12RenderDevice, CurRenderWorld);
		CurRenderWorld->InjectGALMetadataXXX(_CurRenderWorldGALData);
	}


	_CurRenderWorldGALData->_RenderEnvCBSysMemAddr->VPMatrix = XMMatrixTranspose(InCamera->CalcVPMatrix());
	_CurRenderWorldGALData->_RenderEnvCBSysMemAddr->ViewerPos = InCamera->GetCameraTransform().Position.SimdVec;
}

void DX12GALRenderDeviceContext::AddRenderLightToDraw(IRenderLight* InLight)
{
	_RenderLightsToDraw.PushBack(InLight);
	SS_ASSERT(_RenderLightsToDraw.GetSize() == 1);

	for (IRenderLight* LightItem : _RenderLightsToDraw)
	{
		ELightType LightType = LightItem->GetLightType();
		if (LightType == ELightType::Directional)
		{
			IRenderLightDirectional* DirectionalLight = (IRenderLightDirectional*)LightItem;
			const RenderLightDirectionalDesc& Desc = DirectionalLight->GetDirectionalLightDesc();

			if (DirectionalLight->GetGALMetadata() == nullptr && Desc.bEnableShadowMap)
			{
				GenerateRenderInstanceMetadata(DirectionalLight);
			}
			else if (DirectionalLight->GetGALMetadata() != nullptr && Desc.bEnableShadowMap == false)
			{
				DirectionalLight->ReleaseGALMetaData();
			}
		}
		else
		{
			SS_ASSERT(false);
		}
	}
}

void DX12GALRenderDeviceContext::CommitAddedRenderLights()
{
	if (_CurRenderCamera == nullptr)
	{
		SS_INTERRUPT();
	}

	_CurRenderWorldGALData->SyncLights(_RenderLightsToDraw, _CurRenderCamera);
}



void DX12GALRenderDeviceContext::ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From,
                                                 EResourceStateType To)
{
	InRenderTarget->ResourceBarrier(this, From, To);
}

void DX12GALRenderDeviceContext::SetPSOAndRootSignature(const PipelineDesc& InPSODesc)
{
	if (_LastSetPSO == InPSODesc)
	{
		return;
	}

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	PCommonGALRenderDevice* OwnerDevice = (PCommonGALRenderDevice*)GetOwnerRenderDevice();
	RootSignaturePool* lRootSignaturePool = OwnerDevice->GetRootSignaturePool();
	DX12PSOPool* PSOPool = (DX12PSOPool*)OwnerDevice->GetPSOPool();

	const DX12PSOWrapper* lDX12PSOWrapper = (const DX12PSOWrapper*)PSOPool->FindOrAddPSO(InPSODesc);

	const RootSignatureWrapper* RootSignatureWrapper = lRootSignaturePool->GetRootSignature(InPSODesc.RootSignatureType);
	const DX12RootSignatureWrapper* lDX12RootSignatureWrapper = (const DX12RootSignatureWrapper*)RootSignatureWrapper;

	CurCommandList->SetGraphicsRootSignature(lDX12RootSignatureWrapper->GetRootSignatureInstantce());
	CurCommandList->SetPipelineState(lDX12PSOWrapper->GetPipelineState());
}


void DX12GALRenderDeviceContext::SetRenderTarget(int32 NumRenderTargets, GALRenderTarget** InRenderTargets,
                                                 GALRenderTarget* InDepthStencilView)
{
	if (NumRenderTargets > RT_NUM_MAX)
	{
		SS_ASSERT(false);
		return;
	}

	if (InDepthStencilView != nullptr && InDepthStencilView->GetRenderTargetType() != ERenderTargetType::DepthStencil)
	{
		SS_ASSERT(false);
		return;
	}

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();

	_BoundDSV = InDepthStencilView;

	D3D12_CPU_DESCRIPTOR_HANDLE RTVDescHandles[RT_NUM_MAX];

	_BoundRenderTargets.Clear();
	for (int i = 0; i < NumRenderTargets; i++)
	{
		DX12GALRenderTargetBase* RTItem = (DX12GALRenderTargetBase*)InRenderTargets[i];
		RTVDescHandles[i] = RTItem->GetCurrentDSV();

		_BoundRenderTargets.PushBack(RTItem);
	}


	if (NumRenderTargets > 0)
	{
		DX12GALRenderTargetBase* MajorRT = (DX12GALRenderTargetBase*)InRenderTargets[0];
		const ViewportBox& VB = MajorRT->GetViewportBoxSize();
		const BoundBox2f& SR = MajorRT->GetScissorRectSize();

		D3D12_RECT ScissorRectSize;
		ScissorRectSize.left = SR.Min.X;
		ScissorRectSize.top = SR.Min.Y;
		ScissorRectSize.right = SR.Max.X;
		ScissorRectSize.bottom = SR.Max.Y;

		D3D12_VIEWPORT ViewportSize;
		ViewportSize.TopLeftX = VB.LeftTop.X;
		ViewportSize.TopLeftY = VB.LeftTop.Y;
		ViewportSize.Width = VB.WidthHeight.X;
		ViewportSize.Height = VB.WidthHeight.Y;
		ViewportSize.MinDepth = VB.MinDepth;
		ViewportSize.MaxDepth = VB.MaxDepth;

		CurCommandList->RSSetScissorRects(1, &ScissorRectSize);
		CurCommandList->RSSetViewports(1, &ViewportSize);
	}

	if (NumRenderTargets > 0 && InDepthStencilView != nullptr) // DepthStencil이랑 NumRenderTarget이 모두 있는 경우
	{
		DX12GALDSVRenderTarget* DX12DSV = (DX12GALDSVRenderTarget*)InDepthStencilView;
		D3D12_CPU_DESCRIPTOR_HANDLE DSVDescHandle = DX12DSV->GetCurrentDSV();
		CurCommandList->OMSetRenderTargets(NumRenderTargets, RTVDescHandles, FALSE, &DSVDescHandle);
	}
	else if (NumRenderTargets > 0 && InDepthStencilView == nullptr) // DepStencil만 없는 경우
	{
		CurCommandList->OMSetRenderTargets(NumRenderTargets, RTVDescHandles, FALSE, nullptr);
	}
	else // DepthStencil만 있는 경우
	{
		DX12GALDSVRenderTarget* DX12DSV = (DX12GALDSVRenderTarget*)InDepthStencilView;
		D3D12_CPU_DESCRIPTOR_HANDLE DSVDescHandle = DX12DSV->GetCurrentDSV();
		const ViewportBox& VB = DX12DSV->GetViewportBoxSize();
		const BoundBox2f& SR = DX12DSV->GetScissorRectSize();

		D3D12_RECT ScissorRectSize;
		ScissorRectSize.left = SR.Min.X;
		ScissorRectSize.top = SR.Min.Y;
		ScissorRectSize.right = SR.Max.X;
		ScissorRectSize.bottom = SR.Max.Y;

		D3D12_VIEWPORT ViewportSize;
		ViewportSize.TopLeftX = VB.LeftTop.X;
		ViewportSize.TopLeftY = VB.LeftTop.Y;
		ViewportSize.Width = VB.WidthHeight.X;
		ViewportSize.Height = VB.WidthHeight.Y;
		ViewportSize.MinDepth = VB.MinDepth;
		ViewportSize.MaxDepth = VB.MaxDepth;

		CurCommandList->RSSetScissorRects(1, &ScissorRectSize);
		CurCommandList->RSSetViewports(1, &ViewportSize);
		CurCommandList->OMSetRenderTargets(0, nullptr, FALSE, &DSVDescHandle);
	}
}

void DX12GALRenderDeviceContext::ClearRenderTarget(GALRenderTarget* InRenderTarget)
{
	DX12GALRenderTargetBase* DX12RenderTarget = (DX12GALRenderTargetBase*)InRenderTarget;
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();

	DX12RenderTarget->ClearRenderTarget(CurCommandList);
}

void DX12GALRenderDeviceContext::CopyRenderTarget(GALCPUReadableTexture* CopyDest, GALRenderTarget* CopySrc)
{
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
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


	if (InRenderInstance->GetRIType() == ERenderInstanceType::StaticMesh)
	{
		IRIMesh* RIMesh = (IRIMesh*)InRenderInstance;
		DrawStaticMesh(RIMesh, ObjTransformMat, ObjRotMat);
	}
	else
	{
		SS_INTERRUPT();
	}
}

void DX12GALRenderDeviceContext::DrawShadow(IRenderInstance* InRenderInstance)
{
	if (InRenderInstance->GetGALMetadata() == nullptr)
	{
		GenerateRenderInstanceMetadata(InRenderInstance);
	}

	XMMATRIX ObjTransformMat = InRenderInstance->GetWorldTransformMatrix();
	XMMATRIX ObjRotMat = InRenderInstance->GetWorldRotationMatrix();


	if (InRenderInstance->GetRIType() == ERenderInstanceType::StaticMesh)
	{
		IRIMesh* RIMesh = (IRIMesh*)InRenderInstance;
		DrawShadowStaticMesh(RIMesh, ObjTransformMat, ObjRotMat);
	}
	else
	{
		SS_INTERRUPT();
	}
}


void DX12GALRenderDeviceContext::DrawStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat)
{
	DX12GALRIMetadata_SM* DX12RenderInstanceMetaData = (DX12GALRIMetadata_SM*)RIToDraw->GetGALMetadata();
	IModelAsset* InModelAsset = RIToDraw->GetModelAsset();

	PCommonGALRenderDevice* OwnerDevice = (PCommonGALRenderDevice*)GetOwnerRenderDevice();
	ICommonRenderAssetSet* CommonRenderAssets = OwnerDevice->GetCommonRenderAssetSet();
	RootSignaturePool* lRootSignaturePool = OwnerDevice->GetRootSignaturePool();
	DX12PSOPool* PSOPool = (DX12PSOPool*)OwnerDevice->GetPSOPool();
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();


	// Scrap Mesh Asset
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
		return;
	}


	{
		PipelineDesc NewPipelineDesc = ConstructPSODescToDrawMesh(
			EMeshType::Rigid,
			EMaterialType::DefaultPBR,
			_BoundRenderTargets.GetSize(),
			_BoundRenderTargets.GetData(),
			GetThisFrameBoundDSV());
		SetPSOAndRootSignature(NewPipelineDesc);
	}

	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);


	{
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->WMatrix = XMMatrixTranspose(DrawMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->RotMatrix = XMMatrixTranspose(DrawRotMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->ObjectID = RIToDraw->GetGameObjectID().GetNativeValue();

	}

	CurCommandList->SetGraphicsRootConstantBufferView(0, DX12RenderInstanceMetaData->_ModelCBGPUMemAddr);
	CurCommandList->SetGraphicsRootConstantBufferView(1, _CurRenderWorldGALData->_RenderEnvCBGPUMemAddr);


	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		IMaterialAsset* MtlAsset = InModelAsset->GetMaterialAsset(i);
		DX12GALDefaultPBRMaterialAsset* GALMaterial = nullptr;
		if (MtlAsset != nullptr)
		{
			const MtlDataBase* MtlData = MtlAsset->GetMtlData();
			if (MtlData->_Type == EMaterialType::DefaultPBR)
			{
				GALMaterial = (DX12GALDefaultPBRMaterialAsset*)MtlAsset->GetGALMaterialAsset();
			}
		}

		if (MtlAsset == nullptr || GALMaterial == nullptr)
		{
			const IMaterialAsset* EmptyMtl = g_CommonRenderAssetSet->GetEmptyPBRMaterial();
			GALMaterial = (DX12GALDefaultPBRMaterialAsset*)EmptyMtl->GetGALMaterialAsset();

		}

		CurCommandList->SetGraphicsRootConstantBufferView(2, GALMaterial->_MtlCBGPUMemAddr); // b2



		// TODO: BeginDrawMesh랑 EndDrawMesh구현하면서 SetDescriptorHeaps, SetPipelineState, SetGraphicsRootSignature 하는거 몰아서 하기
		if (GALMaterial->_MtlTexSRVDescHeap == _CurRenderWorldGALData->GetLightSettingDescHeap())
		{
			CurCommandList->SetDescriptorHeaps(1, &GALMaterial->_MtlTexSRVDescHeap); // 메테리얼, GALWorld 디스크립터 힙 바인딩
		}
		else
		{
			ID3D12DescriptorHeap* Heaps[] =
			{
				GALMaterial->_MtlTexSRVDescHeap,
				_CurRenderWorldGALData->GetLightSettingDescHeap()
			};
			CurCommandList->SetDescriptorHeaps(_countof(Heaps), Heaps); // 메테리얼, GALWorld 디스크립터 힙 바인딩
		}

		CurCommandList->SetGraphicsRootDescriptorTable(3, GALMaterial->_MtlTexSRVDescTableGPU); // 메테리얼 디스크립터 테이블 바인딩

		{
			CurCommandList->SetGraphicsRootConstantBufferView(4, _CurRenderWorldGALData->GetRenderLightParamCB());
			CurCommandList->SetGraphicsRootDescriptorTable(5, _CurRenderWorldGALData->GetLightSeetingDescTable()); // GALWorld의 RenderEnv 바인딩
		} // RenderEnv

		CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);
		int32 CurIdxDataCnt = DefaultMeshRawData->_indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
		// CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, IdxDataOffset, 0, 0); => IdxDataOffset이 이미 GALMeshAsset->_IndexBufferView에 포함돼있어서 안넣어줘도 됨
	}
}

void DX12GALRenderDeviceContext::DrawShadowStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat,
	const XMMATRIX& DrawRotMat)
{
	DX12GALRIMetadata_SM* DX12RenderInstanceMetaData = (DX12GALRIMetadata_SM*)RIToDraw->GetGALMetadata();
	IModelAsset* InModelAsset = RIToDraw->GetModelAsset();

	PCommonGALRenderDevice* OwnerDevice = (PCommonGALRenderDevice*)GetOwnerRenderDevice();
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();


	// Scrap Mesh Asset
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
		return;
	}


	{
		PipelineDesc NewPipelineDesc = ConstructPSODescToDrawShadow(
			EMeshType::Rigid);
		SetPSOAndRootSignature(NewPipelineDesc);
	}

	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);

	{
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->WMatrix = XMMatrixTranspose(DrawMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->RotMatrix = XMMatrixTranspose(DrawRotMat);
		DX12RenderInstanceMetaData->_ModelCBSysMemAddr->ObjectID = RIToDraw->GetGameObjectID().GetNativeValue();
	}

	CurCommandList->SetGraphicsRootConstantBufferView(0, DX12RenderInstanceMetaData->_ModelCBGPUMemAddr);

	if (_DrawingShadowMapMetadata->GetLightType() == ELightType::Directional)
	{
		DX12GALRIDirectionalLightShadowMapMetadata* DX12GalriDirectionalLightShadowMapMetaData =
			static_cast<DX12GALRIDirectionalLightShadowMapMetadata*>(_DrawingShadowMapMetadata);
		CurCommandList->SetGraphicsRootConstantBufferView(1, DX12GalriDirectionalLightShadowMapMetaData->_ShadowMapCBGPUMemAddr);
	}
	else
	{
		SS_ASSERT(false);
	}

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

	_RenderLightsToDraw.Clear();
	_BoundRenderTargets.Clear();
	_BoundDSV = nullptr;

	_CurRenderCamera = nullptr;
	_CurRenderWorldGALData = nullptr;
	_DrawingShadowMapMetadata = nullptr;
	_LastSetPSO = PipelineDesc(); // 초기화
}

void DX12GALRenderDeviceContext::ResetCommandList()
{
	HRESULT hr;

	for (int32 i = 0; i <= _CurCommandListIdx; i++)
	{
		ID3D12GraphicsCommandList* CurCommandList = _DrawWorkerCommandLists[i];
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
		ID3D12GraphicsCommandList* CurCommandList = _DrawWorkerCommandLists[i];

		hr = CurCommandList->Close();
		if (FAILED(hr)) SS_INTERRUPT();
	}
}
