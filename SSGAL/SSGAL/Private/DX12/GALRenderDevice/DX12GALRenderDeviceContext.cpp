#include "pch.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"
#include "SSEngineDefault/Public/SSContainer/ContainerUtil/ContainerUtil.h"

#include "DX12GALRenderDevice.h"
#include "DX12GALRenderDeviceContext.h"

#include <SSRenderer/Public/RenderInstance/IRICubeMap.h>


#include "Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "Private/DX12/DX12CommonUtils/DX12TransientConstantBufferAllocator.h"
#include "Private/DX12/GALPostProcessContext/DX12GALPPCDeferredShading.h"
#include "Private/DX12/GALRenderAsset/DX12GALSkinnedMeshAssetWrapper.h"
#include "Private/DX12/GALRenderInstance/DX12GALRICubeMap.h"
#include "Private/DX12/GALRenderInstance/DX12GALRIDirectionalLightShadowMapMetadata.h"
#include "Private/DX12/GALRenderInstance/DX12GALRIMetadata_SKM.h"
#include "Private/DX12/GALRenderTarget/DX12GALUAVRenderTarget.h"
#include "Private/PCommon/TestCodes/GALTestCodes.h"
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
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"
#include "SSRenderer/Public/RenderInstance/Light/IRenderLightDirectional.h"
#include "SSRenderer/Public/RenderInstance/Descriptors/LightDesc.h"
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"
#include "SSRenderer/Public/RenderInstance/IRISkinnedMesh.h"



DX12GALRenderDeviceContext::DX12GALRenderDeviceContext(DX12GALRenderDevice* InRenderDevice, int32 SwapChainFrameCnt):
	_BoundRenderTargets(RT_NUM_MAX),
	_RenderLightsToDraw(32)
{
	_OwnerRenderDevice = InRenderDevice;

	ID3D12Device5* D3DDevice = InRenderDevice->GetD3DDevice();

	_DrawWorkerCommandAllocators.Reserve(SwapChainFrameCnt * 2);
	_DrawWorkerCommandLists.Reserve(SwapChainFrameCnt * 2);


	
	
	for (int32 i = 0; i < SwapChainFrameCnt; i++)
	{
		ID3D12CommandAllocator* NewCommandAllocator = nullptr;
		ID3D12GraphicsCommandList* NewCommandList = nullptr;
		if (FAILED(D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&NewCommandAllocator))))
		{
			DEBUG_BREAK();
			goto lb_cleanup;
		}

		if (FAILED(D3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, NewCommandAllocator, nullptr, IID_PPV_ARGS(&NewCommandList))))
		{
			NewCommandAllocator->Release();
			DEBUG_BREAK();
			goto lb_cleanup;
		}


		NewCommandList->Close();

		_DrawWorkerCommandAllocators.PushBack(NewCommandAllocator);
		_DrawWorkerCommandLists.PushBack(NewCommandList);
	}

	_TransientCBAllocator = DBG_NEW DX12TransientConstantBufferAllocator(
		this,
		GAL_RESOURCE_DEFAULT_ALIGNMENT_SIZE,
		GAL_CONSTANTBUFFER_MIN_SIZE,
		256,
		L"DX12GALRenderDevice::_TransientConstantBufferAllocator");

	_ResourceUpdater = DBG_NEW DX12GALResourceUpdater(InRenderDevice, this);

	if (_OwnerRenderDevice->IsDebugEnabled())
	{
		TestTransientAllocator(this);
	}

	return;

lb_cleanup:
	if (_ResourceUpdater != nullptr)
	{
		delete _ResourceUpdater;
		_ResourceUpdater = nullptr;
	}

	for (ID3D12GraphicsCommandList* CommandListItem : _DrawWorkerCommandLists)
	{
		CommandListItem->Release();
	}

	for (ID3D12CommandAllocator* CommandAllocator : _DrawWorkerCommandAllocators)
	{
		CommandAllocator->Release();
	}

	_DrawWorkerCommandLists.Resize(0);
	_DrawWorkerCommandAllocators.Resize(0);
}

DX12GALRenderDeviceContext::~DX12GALRenderDeviceContext()
{
	for (ID3D12CommandList* CommandListItem : _DrawWorkerCommandLists)
	{
		CommandListItem->Release();
	}

	for (ID3D12CommandAllocator* AllocatorItem : _DrawWorkerCommandAllocators)
	{
		AllocatorItem->Release();	
	}

	_DrawWorkerCommandLists.Resize(0);
	_DrawWorkerCommandAllocators.Resize(0);

	delete _ResourceUpdater;

	_TransientCBAllocator->ReleaseDefaultPages();
	delete _TransientCBAllocator;
}

bool DX12GALRenderDeviceContext::IsValid() const
{
	return _DrawWorkerCommandLists.GetSize() != 0;
}

ERenderDeviceTaskPhase DX12GALRenderDeviceContext::GetTaskPhase()
{
	return _TaskPhase;
}

GALRWMetaData* DX12GALRenderDeviceContext::GetCurRenderWorldGALMetaData() const
{
	if (_CurRenderWorldGALData == nullptr)
	{
		SS_INTERRUPT();
		return nullptr;
	}

	return _CurRenderWorldGALData;
}

bool DX12GALRenderDeviceContext::GenerateMeshGALAsset(IMeshAssetMutable* InMeshAsset)
{
	if (InMeshAsset->GetGALMeshAsset() != nullptr)
	{
		return false;
	}

	HRESULT hr = S_OK;


	if (InMeshAsset->GetMeshType() == EMeshType::Rigid)
	{
		DX12GALMeshAssetWrapper* NewGALMeshAsset = DBG_NEW DX12GALMeshAssetWrapper(InMeshAsset, this);
		InMeshAsset->InjectGALMeshAsset(NewGALMeshAsset);
		return true;
	}

	if (InMeshAsset->GetMeshType() == EMeshType::Skinned)
	{
		DX12GALSkinnedMeshAssetWrapper* NewGALMeshAsset = DBG_NEW DX12GALSkinnedMeshAssetWrapper(InMeshAsset, this);
		InMeshAsset->InjectGALMeshAsset(NewGALMeshAsset);
		return true;
	}


	SS_INTERRUPT();
	return false;
}

bool DX12GALRenderDeviceContext::GenerateTextureGALAsset(ITextureAssetMutable* InTextureAsset)
{
	if (InTextureAsset->GetGALTextureAsset() != nullptr)
	{
		return false;
	}

	DX12GALTextureAssetWrapper* NewTextureAsset = DBG_NEW DX12GALTextureAssetWrapper(InTextureAsset, this);
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
		IRIMesh* InIRIMesh = static_cast<IRIMesh*>(InRenderInstance);
		DX12GALRIMetadata_SM* NewGALRI = DBG_NEW DX12GALRIMetadata_SM(_OwnerRenderDevice, InIRIMesh);
		InIRIMesh->InjectGALMetadataXXX(NewGALRI);
	}
	else if (RIType == ERenderInstanceType::SkinnedMesh)
	{
		IRISkinnedMesh* InIRIMesh = static_cast<IRISkinnedMesh*>(InRenderInstance);
		DX12GALRIMetadata_SKM* NewGALRI = DBG_NEW DX12GALRIMetadata_SKM(_OwnerRenderDevice, InIRIMesh);
		NewGALRI->SyncBonePose();
		InIRIMesh->InjectGALMetadataXXX(NewGALRI);
	}
	else if (RIType == ERenderInstanceType::Light)
	{
		IRenderLight* InRenderLight = static_cast<IRenderLight*>(InRenderInstance);

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
	else if (RIType == ERenderInstanceType::CubeMap)
	{
		IRICubeMap* InCubeMap = static_cast<IRICubeMap*>(InRenderInstance);
		DX12GALRICubeMap* NewGALRICubeMap =
			DBG_NEW DX12GALRICubeMap(static_cast<DX12GALRenderDevice*>(_OwnerRenderDevice), InCubeMap);

		InCubeMap->InjectGALMetadataXXX(NewGALRICubeMap);
	}
	else
	{
		DEBUG_BREAK();
		return;
	}
}

void DX12GALRenderDeviceContext::BeginDrawShadowMap(IRenderLight* InLightToDrawShadowMap)
{
	if (_TaskPhase != ERenderDeviceTaskPhase::TaskWaiting)
	{
		SS_INTERRUPT();
	}
	_TaskPhase = ERenderDeviceTaskPhase::DrawShadow;

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
		ClearRenderTarget(ShadowMap, Vector4f::Zero);
		SetRenderTarget(0, nullptr, ShadowMap);
	}
	else
	{
		SS_ASSERT(false);
	}
}

void DX12GALRenderDeviceContext::EndDrawShadowMap()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawShadow)
	{
		SS_INTERRUPT();
	}
	_TaskPhase = ERenderDeviceTaskPhase::TaskWaiting;

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

	IRenderWorld* RenderWorldToStartDraw = InCamera->GetIcludedRenderWorld();
	if (RenderWorldToStartDraw == nullptr)
	{
		SS_INTERRUPT();
		return;
	}

	_CurRenderWorld = RenderWorldToStartDraw;
	_CurRenderWorldGALData = (DX12GALRWMetaData*)RenderWorldToStartDraw->GetGALMetadata();
	if (_CurRenderWorldGALData == nullptr)
	{
		_CurRenderWorldGALData = DBG_NEW DX12GALRWMetaData(OwnerDX12RenderDevice, RenderWorldToStartDraw);
		RenderWorldToStartDraw->InjectGALMetadataXXX(_CurRenderWorldGALData);
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
		RTVDescHandles[i] = RTItem->GetCurrentRTV();

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
		D3D12_CPU_DESCRIPTOR_HANDLE DSVDescHandle = DX12DSV->GetCurrentRTV();
		CurCommandList->OMSetRenderTargets(NumRenderTargets, RTVDescHandles, FALSE, &DSVDescHandle);
	}
	else if (NumRenderTargets > 0 && InDepthStencilView == nullptr) // DepStencil만 없는 경우
	{
		CurCommandList->OMSetRenderTargets(NumRenderTargets, RTVDescHandles, FALSE, nullptr);
	}
	else // DepthStencil만 있는 경우
	{
		DX12GALDSVRenderTarget* DX12DSV = (DX12GALDSVRenderTarget*)InDepthStencilView;
		D3D12_CPU_DESCRIPTOR_HANDLE DSVDescHandle = DX12DSV->GetCurrentRTV();
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

void DX12GALRenderDeviceContext::ClearRenderTarget(GALRenderTarget* InRenderTarget, const Vector4f& ClearColor)
{
	DX12GALRenderTargetBase* DX12RenderTarget = (DX12GALRenderTargetBase*)InRenderTarget;
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();

	DX12RenderTarget->ClearRenderTarget(CurCommandList, ClearColor);
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

void DX12GALRenderDeviceContext::CopyRenderTarget(GALRenderTarget* CopyDest, GALRenderTarget* CopySrc)
{
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	DX12GALRenderDevice* OwnerDeviceDX12 = (DX12GALRenderDevice*)GetOwnerRenderDevice();
	ID3D12Device5* D3DDevice = OwnerDeviceDX12->GetD3DDevice();


	DX12GALRenderTargetBase* DestRTDX12 = (DX12GALRenderTargetBase*)CopyDest;
	ID3D12Resource* DestRes = DestRTDX12->GetCurrentResource();


	DX12GALRenderTargetBase* SrcRTDX12 = (DX12GALRenderTargetBase*)CopySrc;
	ID3D12Resource* SrcRes = SrcRTDX12->GetCurrentResource();


	UINT __Rows = 0;
	UINT64 __RowSize = 0;
	UINT64 __TotalBytes = 0;
	D3D12_RESOURCE_DESC SrcDesc = SrcRes->GetDesc();
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT SrcFootprint;
	D3DDevice->GetCopyableFootprints(
		&SrcDesc,
		0,
		1,
		0,
		&SrcFootprint,
		&__Rows,
		&__RowSize,
		&__TotalBytes
	);


	D3D12_RESOURCE_DESC DestDesc = DestRes->GetDesc();
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT DestFootprint;
	D3DDevice->GetCopyableFootprints(
		&DestDesc,
		0,
		1,
		0,
		&DestFootprint,
		&__Rows,
		&__RowSize,
		&__TotalBytes
	);




	D3D12_BOX box;
	box.front = 0;
	box.back = 1;
	box.left = 0;
	box.right = DestRTDX12->GetViewportBoxSize().WidthHeight.X;
	box.top = 0;
	box.bottom = DestRTDX12->GetViewportBoxSize().WidthHeight.Y;

	D3D12_TEXTURE_COPY_LOCATION	destLocation;
	destLocation.PlacedFootprint = DestFootprint;
	destLocation.pResource = DestRes;
	destLocation.SubresourceIndex = 0;
	destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	D3D12_TEXTURE_COPY_LOCATION	srcLocation;
	srcLocation.PlacedFootprint = SrcFootprint;
	srcLocation.pResource = SrcRes;
	srcLocation.SubresourceIndex = 0;
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	CurCommandList->CopyTextureRegion(&destLocation, box.left, box.top, 0, &srcLocation, &box);
}

void DX12GALRenderDeviceContext::BeginDrawMesh()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::TaskWaiting)
	{
		SS_INTERRUPT();
	}

	_TaskPhase = ERenderDeviceTaskPhase::DrawMesh;
}

void DX12GALRenderDeviceContext::DrawMesh(IRenderInstance* InRenderInstance)
{
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawMesh)
	{
		SS_INTERRUPT();
		return;
	}


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
	else if (InRenderInstance->GetRIType() == ERenderInstanceType::SkinnedMesh)
	{
		IRISkinnedMesh* RIMesh = (IRISkinnedMesh*)InRenderInstance;
		DrawSkinnedMesh(RIMesh, ObjTransformMat, ObjRotMat);
	}
	else
	{
		SS_INTERRUPT();
	}
}

void DX12GALRenderDeviceContext::EndDrawMesh()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawMesh)
	{
		SS_INTERRUPT();
	}
	_TaskPhase = ERenderDeviceTaskPhase::TaskWaiting;
}

void DX12GALRenderDeviceContext::DrawSkyMap(IRICubeMap* CubeMapToDraw)
{
	if (GetTaskPhase() != ERenderDeviceTaskPhase::PostProcess)
	{
		SS_INTERRUPT();
		return;
	}

	if (CubeMapToDraw->GetGALMetadata() == nullptr)
	{
		GenerateRenderInstanceMetadata(CubeMapToDraw);
	}


	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	DX12GALRenderDevice* OwnerDX12RenderDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice);
	ID3D12Device5* D3DDevice = OwnerDX12RenderDevice->GetD3DDevice();

	ITextureAsset* TextureAsset = CubeMapToDraw->GetCubemapTexture();
	const DX12GALTextureAssetWrapper* DX12GALTexAsset = static_cast<const DX12GALTextureAssetWrapper*>(TextureAsset->GetGALTextureAsset());

	IMeshAsset* CubeMeshAsset = g_CommonRenderAssetSet->GetCube1mMesh();
	const DX12GALMeshAssetWrapper* DX12CubeMeshAsset = static_cast<const DX12GALMeshAssetWrapper*>(CubeMeshAsset->GetGALMeshAsset());
	const MeshRawDataDefault* DefaultMeshRawData = static_cast<const MeshRawDataDefault*>(CubeMeshAsset->GetMeshRawData());
	int32 IndexCnt = DefaultMeshRawData->_VertexHeader.indexDataCnt[0];

	DX12GALRICubeMap* DX12GALCubeMap = static_cast<DX12GALRICubeMap*>(CubeMapToDraw->GetGALMetadata());
	ID3D12DescriptorHeap* CubemapDescHeap = DX12GALCubeMap->GetCubeMapDescHeap();
	CD3DX12_CPU_DESCRIPTOR_HANDLE CubemapDescTableCPU = DX12GALCubeMap->GetCubemapDescTableCPU();
	CD3DX12_GPU_DESCRIPTOR_HANDLE CubemapDescTableGPU = DX12GALCubeMap->GetCubemapDescTableGPU();


	D3DDevice->CopyDescriptorsSimple(1, 
		CubemapDescTableCPU, DX12GALTexAsset->_SRVHandle,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);



	PipelineDesc SkyMapPSODesc = ConstructPSOToDrawSkyMap();
	SetPSOAndRootSignature(SkyMapPSODesc);


	{
		const Transform& CamTransform = _CurRenderCamera->GetCameraTransform();
		const Vector4f& CamPos = CamTransform.Position;
		const float CubeMapSize = CubeMapToDraw->GetCubeMapSize();


		DX12GALCubeMap->GetCubemapCBModelSysmem()->ObjectID = 0; // 일단 사용 안함
		DX12GALCubeMap->GetCubemapCBModelSysmem()->RotMatrix = XMMatrixIdentity();
		Transform CubemapModelTransform;
		CubemapModelTransform.Position = Vector4f(CamPos.X, CamPos.Y - CubeMapSize * 0.5f, CamPos.Z, 1);
		CubemapModelTransform.Scale = Vector4f(CubeMapSize, CubeMapSize, CubeMapSize, 0);
		DX12GALCubeMap->GetCubemapCBModelSysmem()->WMatrix = XMMatrixTranspose(CubemapModelTransform.AsMatrix());


		const float CamAspectRatio = _CurRenderCamera->GetAspectRatio();
		const float CamFOV = _CurRenderCamera->GetFOVWithRadians();
		XMMATRIX ProjMat = XMMatrixPerspectiveFovLH(CamFOV, CamAspectRatio, 0.001, CubeMapSize * 2);


		XMVECTOR EyePos = CamTransform.Position.SimdVec;
		XMVECTOR Direction = CamTransform.GetForward().SimdVec;
		XMVECTOR Up = CamTransform.GetUp().SimdVec;
		XMMATRIX ViewMat = XMMatrixLookToLH(EyePos, Direction, Up);

		DX12GALCubeMap->GetCubemapCBRenderEnvParamSysmem()->ViewerPos = CamTransform.Position.SimdVec;
		const XMMATRIX VPMatrix = ViewMat * ProjMat;
		DX12GALCubeMap->GetCubemapCBRenderEnvParamSysmem()->VPMatrix = XMMatrixTranspose(VPMatrix);
	}


	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &DX12CubeMeshAsset->_VertexBufferView);
	CurCommandList->IASetIndexBuffer(DX12CubeMeshAsset->_IndexBufferView);

	CurCommandList->SetGraphicsRootConstantBufferView(0, DX12GALCubeMap->GetCubemapCBModelGPUMem());
	CurCommandList->SetGraphicsRootConstantBufferView(1, DX12GALCubeMap->GetCubemapCBRenderEnvParamGPUMem());

	CurCommandList->SetDescriptorHeaps(1, &CubemapDescHeap);
	CurCommandList->SetGraphicsRootDescriptorTable(2, CubemapDescTableGPU);

	CurCommandList->DrawIndexedInstanced(IndexCnt, 1, 0, 0, 0);
}

void DX12GALRenderDeviceContext::BeginPostProcessing()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::TaskWaiting)
	{
		SS_INTERRUPT();
	}

	_TaskPhase = ERenderDeviceTaskPhase::PostProcess;
}

void DX12GALRenderDeviceContext::ExecutePostProcessing(
	GALPostProcessContextBase* PostProcessContext)
{
	if (_TaskPhase != ERenderDeviceTaskPhase::PostProcess)
	{
		SS_INTERRUPT();
		return;
	}

	PostProcessContext->ExecutePostProcess(this);
}

void DX12GALRenderDeviceContext::EndPostProcessing()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::PostProcess)
	{
		SS_INTERRUPT();
	}

	_TaskPhase = ERenderDeviceTaskPhase::TaskWaiting;
}


void DX12GALRenderDeviceContext::BeginDrawDebug()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::TaskWaiting)
	{
		SS_INTERRUPT();
	}

	_TaskPhase = ERenderDeviceTaskPhase::DrawDebug;
}

void DX12GALRenderDeviceContext::DrawDebugWire(
	const IMeshAsset* InMesh,
	const XMMATRIX& TransformMatrix,
	const XMMATRIX& RotMatrix, 
	const Vector4f& InColor,
	bool bUseDepth)
{
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawDebug)
	{
		SS_INTERRUPT();
	}

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	SSTransientMemAllocator* TransientMemAllocator = GetTransientCBAllocator();


	GALRenderTarget* RTDepth = nullptr;
	if (bUseDepth)
	{
		RTDepth = GetThisFrameBoundDSV();
	}
	PipelineDesc Desc = ConstructPSOToDrawDebugWire(RTDepth);
	SetPSOAndRootSignature(Desc);



	TransientChunkHeader MeshTransformCBChunk = TransientMemAllocator->AllocChunk(sizeof(sizeof(CBAModelBuffer)));
	DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)MeshTransformCBChunk.PageContent;
	CBAModelBuffer* ModelCBSystemAddr = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + MeshTransformCBChunk.ChunkOffset);
	D3D12_GPU_VIRTUAL_ADDRESS ModelCBGPUAdddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + MeshTransformCBChunk.ChunkOffset;
	ModelCBSystemAddr->WMatrix = XMMatrixTranspose(TransformMatrix);
	ModelCBSystemAddr->RotMatrix = XMMatrixTranspose(RotMatrix);
	CurCommandList->SetGraphicsRootConstantBufferView(0, ModelCBGPUAdddr);
	CurCommandList->SetGraphicsRootConstantBufferView(1, _CurRenderWorldGALData->_RenderEnvCBGPUMemAddr);



	TransientChunkHeader DrawColorCBChunk = TransientMemAllocator->AllocChunk(sizeof(sizeof(Vector4f)));
	DX12ConstantBufferResourcePage* ColorCBPage = (DX12ConstantBufferResourcePage*)DrawColorCBChunk.PageContent;
	Vector4f* ColorCBSystemAddr = reinterpret_cast<Vector4f*>(ColorCBPage->ResourceSysMem + DrawColorCBChunk.ChunkOffset);
	D3D12_GPU_VIRTUAL_ADDRESS ColorCBGPUAdddr = ColorCBPage->D3D12Resource->GetGPUVirtualAddress() + DrawColorCBChunk.ChunkOffset;
	(*ColorCBSystemAddr) = InColor;
	CurCommandList->SetGraphicsRootConstantBufferView(2, ColorCBGPUAdddr);



	int32 SubMeshCnt = InMesh->GetSubMeshCnt();
	const DX12GALMeshAssetWrapper* GALMeshAsset = static_cast<const DX12GALMeshAssetWrapper*>(InMesh->GetGALMeshAsset());
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;
	const MeshRawDataDefault* RawData = static_cast<const MeshRawDataDefault*>(InMesh->GetMeshRawData());

	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);

	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);
		int32 CurIdxDataCnt = RawData->_VertexHeader.indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
	}
}

void DX12GALRenderDeviceContext::EndDrawDebug()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawDebug)
	{
		SS_INTERRUPT();
	}

	_TaskPhase = ERenderDeviceTaskPhase::TaskWaiting;
}

void DX12GALRenderDeviceContext::DrawShadow(IRenderInstance* InRenderInstance)
{
	if (InRenderInstance->GetGALMetadata() == nullptr)
	{
		GenerateRenderInstanceMetadata(InRenderInstance);
	}

	XMMATRIX ObjTransformMat = InRenderInstance->GetWorldTransformMatrix();
	XMMATRIX ObjRotMat = InRenderInstance->GetWorldRotationMatrix();


	ERenderInstanceType RIType = InRenderInstance->GetRIType();
	if (RIType == ERenderInstanceType::StaticMesh)
	{
		IRIMesh* RIMesh = (IRIMesh*)InRenderInstance;
		DrawShadowStaticMesh(RIMesh, ObjTransformMat, ObjRotMat);
	}
	else if (RIType == ERenderInstanceType::SkinnedMesh)
	{
		IRISkinnedMesh* RIMesh = (IRISkinnedMesh*)InRenderInstance;
		DrawShadowSkinnedMesh(RIMesh, ObjTransformMat, ObjRotMat);
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

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();


	// Scrap Mesh Asset
	IMeshAsset* lMeshAsset = InModelAsset->GetMeshAsset();
	const DX12GALMeshAssetWrapper* GALMeshAsset = (const DX12GALMeshAssetWrapper*)lMeshAsset->GetGALMeshAsset();
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;
	const MeshRawDataBase* MeshRawData = lMeshAsset->GetMeshRawData();


	if (MeshRawData->GetMeshType() != EMeshType::Rigid)
	{
		SS_INTERRUPT(false);
		return;
	}
	const MeshRawDataDefault* DefaultMeshRawData = static_cast<const MeshRawDataDefault*>(MeshRawData);
	int32 SubMeshCnt = DefaultMeshRawData->_VertexHeader.subMeshCnt;


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
		_UniqueDescHeapWorkTable.Clear();
		ListPushBackUnique(_UniqueDescHeapWorkTable, GALMaterial->_CachedMtlTexSRVDescHeap);
		ListPushBackUnique(_UniqueDescHeapWorkTable, _CurRenderWorldGALData->GetLightSettingDescHeap());
		CurCommandList->SetDescriptorHeaps(_UniqueDescHeapWorkTable.GetSize(), _UniqueDescHeapWorkTable.GetData());



		CurCommandList->SetGraphicsRootDescriptorTable(3, GALMaterial->_MtlTexSRVDescTableGPU); // 메테리얼 디스크립터 테이블 바인딩

		{
			CurCommandList->SetGraphicsRootConstantBufferView(4, _CurRenderWorldGALData->GetRenderLightParamCB()); // GALWorld의 RenderEnv 바인딩
			CurCommandList->SetGraphicsRootDescriptorTable(5, _CurRenderWorldGALData->GetLightSeetingDescTable()); // ShadowMapBinding
		} // RenderEnv

		CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);
		int32 CurIdxDataCnt = DefaultMeshRawData->_VertexHeader.indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
		// CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, IdxDataOffset, 0, 0); => IdxDataOffset이 이미 GALMeshAsset->_IndexBufferView에 포함돼있어서 안넣어줘도 됨
	}
}

void DX12GALRenderDeviceContext::DrawSkinnedMesh(IRISkinnedMesh* RIToDraw, const XMMATRIX& DrawMat,
	const XMMATRIX& DrawRotMat)
{
	DX12GALRIMetadata_SKM* DX12SkinnedRIMetaData = static_cast<DX12GALRIMetadata_SKM*>(RIToDraw->GetGALMetadata());
	IModelAsset* InModelAsset = RIToDraw->GetModelAsset();

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();


	// Scrap Mesh Asset
	IMeshAsset* lMeshAsset = InModelAsset->GetMeshAsset();
	const MeshRawDataBase* MeshRawData = lMeshAsset->GetMeshRawData();
	if (MeshRawData->GetMeshType() != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}
	const MeshRawDataSkinned* SkinnedMeshRawData = static_cast<const MeshRawDataSkinned*>(MeshRawData);
	int32 SubMeshCnt = SkinnedMeshRawData->_VertexHeader.subMeshCnt;

	const DX12GALMeshAssetWrapper* GALMeshAsset = static_cast<const DX12GALMeshAssetWrapper*>(lMeshAsset->GetGALMeshAsset());
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;


	{
		PipelineDesc NewPipelineDesc = ConstructPSODescToDrawMesh(
			EMeshType::Skinned,
			EMaterialType::DefaultPBR,
			_BoundRenderTargets.GetSize(),
			_BoundRenderTargets.GetData(),
			GetThisFrameBoundDSV());

		SetPSOAndRootSignature(NewPipelineDesc);
	}

	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);


	{
		DX12SkinnedRIMetaData->_ModelCBSysMemAddr->WMatrix = XMMatrixTranspose(DrawMat);
		DX12SkinnedRIMetaData->_ModelCBSysMemAddr->RotMatrix = XMMatrixTranspose(DrawRotMat);
		DX12SkinnedRIMetaData->_ModelCBSysMemAddr->ObjectID = RIToDraw->GetGameObjectID().GetNativeValue();

	}

	CurCommandList->SetGraphicsRootConstantBufferView(0, DX12SkinnedRIMetaData->_ModelCBGPUMemAddr);
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
		_UniqueDescHeapWorkTable.Clear();
		ListPushBackUnique(_UniqueDescHeapWorkTable, GALMaterial->_CachedMtlTexSRVDescHeap);
		ListPushBackUnique(_UniqueDescHeapWorkTable, _CurRenderWorldGALData->GetLightSettingDescHeap());
		ListPushBackUnique(_UniqueDescHeapWorkTable, DX12SkinnedRIMetaData->_CachedJointSRVDescHeap);
		CurCommandList->SetDescriptorHeaps(_UniqueDescHeapWorkTable.GetSize(), _UniqueDescHeapWorkTable.GetData());



		CurCommandList->SetGraphicsRootDescriptorTable(3, GALMaterial->_MtlTexSRVDescTableGPU); // 메테리얼 디스크립터 테이블 바인딩

		{
			CurCommandList->SetGraphicsRootConstantBufferView(4, _CurRenderWorldGALData->GetRenderLightParamCB()); // GALWorld의 RenderEnv 바인딩
			CurCommandList->SetGraphicsRootDescriptorTable(5, _CurRenderWorldGALData->GetLightSeetingDescTable()); // ShadowMapBinding
		} // RenderEnv

		{
			CurCommandList->SetGraphicsRootDescriptorTable(6, DX12SkinnedRIMetaData->_JointSRVDescTableGPU); // SkinningBinding

		} // Skinning

		CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);
		int32 CurIdxDataCnt = SkinnedMeshRawData->_VertexHeader.indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
		// CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, IdxDataOffset, 0, 0); => IdxDataOffset이 이미 GALMeshAsset->_IndexBufferView에 포함돼있어서 안넣어줘도 됨
	}
}

void DX12GALRenderDeviceContext::DrawShadowStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat,
                                                      const XMMATRIX& DrawRotMat)
{
	DX12GALRIMetadata_SM* DX12RenderInstanceMetaData = static_cast<DX12GALRIMetadata_SM*>(RIToDraw->GetGALMetadata());
	IModelAsset* InModelAsset = RIToDraw->GetModelAsset();

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();


	// Scrap Mesh Asset
	IMeshAsset* lMeshAsset = InModelAsset->GetMeshAsset();
	const DX12GALMeshAssetWrapper* GALMeshAsset = static_cast<const DX12GALMeshAssetWrapper*>(lMeshAsset->GetGALMeshAsset());
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;
	const MeshRawDataBase* MeshRawData = lMeshAsset->GetMeshRawData();

	if(MeshRawData->GetMeshType() != EMeshType::Rigid)
	{
		SS_INTERRUPT();
		return;
	}

	const MeshRawDataDefault* DefaultMeshRawData = static_cast<const MeshRawDataDefault*>(MeshRawData);
	int32 SubMeshCnt = DefaultMeshRawData->_VertexHeader.subMeshCnt;


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
		int32 CurIdxDataCnt = DefaultMeshRawData->_VertexHeader.indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
	}
}

void DX12GALRenderDeviceContext::DrawShadowSkinnedMesh(IRISkinnedMesh* RIToDraw, const XMMATRIX& DrawMat,
	const XMMATRIX& DrawRotMat)
{
	DX12GALRIMetadata_SKM* DX12RenderInstanceMetaData = static_cast<DX12GALRIMetadata_SKM*>(RIToDraw->GetGALMetadata());
	IModelAsset* InModelAsset = RIToDraw->GetModelAsset();

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();


	// Scrap Mesh Asset
	IMeshAsset* lMeshAsset = InModelAsset->GetMeshAsset();
	const DX12GALMeshAssetWrapper* GALMeshAsset = static_cast<const DX12GALMeshAssetWrapper*>(lMeshAsset->GetGALMeshAsset());
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;
	const MeshRawDataBase* MeshRawData = lMeshAsset->GetMeshRawData();

	if (MeshRawData->GetMeshType() != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}

	const MeshRawDataSkinned* DefaultMeshRawData = static_cast<const MeshRawDataSkinned*>(MeshRawData);
	int32 SubMeshCnt = DefaultMeshRawData->_VertexHeader.subMeshCnt;


	{
		PipelineDesc NewPipelineDesc = ConstructPSODescToDrawShadow(
			EMeshType::Skinned);
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


	{
		CurCommandList->SetDescriptorHeaps(1, &DX12RenderInstanceMetaData->_CachedJointSRVDescHeap);
		CurCommandList->SetGraphicsRootDescriptorTable(2, DX12RenderInstanceMetaData->_JointSRVDescTableGPU);
	}

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
		int32 CurIdxDataCnt = DefaultMeshRawData->_VertexHeader.indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
	}
}

void DX12GALRenderDeviceContext::ResetRenderState()
{
	_TransientCBAllocator->ResetAllChunksXXX();

	_ResourceUpdater->ResetUpdateBuffer();
	ResetCommandList();

	_RenderLightsToDraw.Clear();
	_BoundRenderTargets.Clear();
	_BoundDSV = nullptr;

	_CurRenderCamera = nullptr;
	_CurRenderWorld = nullptr;
	_CurRenderWorldGALData = nullptr;

	_DrawingShadowMapMetadata = nullptr;
	_LastSetPSO = PipelineDesc(); // 초기화
}

void DX12GALRenderDeviceContext::ResetCommandList()
{
	HRESULT hr;

	{
		ID3D12CommandAllocator* CurDrawWorkerCommandAllcator = _DrawWorkerCommandAllocators[_CurCommandListIdx];
		hr = CurDrawWorkerCommandAllcator->Reset();
		if (FAILED(hr)) SS_INTERRUPT();

		ID3D12GraphicsCommandList* CurDrawWorkerCmdList = _DrawWorkerCommandLists[_CurCommandListIdx];
		hr = CurDrawWorkerCmdList->Reset(CurDrawWorkerCommandAllcator, nullptr);
		if (FAILED(hr)) SS_INTERRUPT();
	}


	_CurCommandListIdx = 0;
}

void DX12GALRenderDeviceContext::BeginRender()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::TaskDenial)
	{
		SS_INTERRUPT();
	}
	_TaskPhase = ERenderDeviceTaskPhase::TaskWaiting;

	ResetRenderState();
}

void DX12GALRenderDeviceContext::EndRender()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::TaskWaiting)
	{
		SS_INTERRUPT();
	}
	_TaskPhase = ERenderDeviceTaskPhase::TaskDenial;

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	HRESULT hr;
	hr = CurCommandList->Close();
	if (FAILED(hr)) SS_INTERRUPT();
}
