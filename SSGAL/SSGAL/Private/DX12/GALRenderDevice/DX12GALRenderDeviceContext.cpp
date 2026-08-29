#include "pch.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"
#include "SSEngineDefault/Public/SSContainer/ContainerUtil/ContainerUtil.h"
#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"

#include "DX12GALRenderDeviceContext.h"

#include <SSRenderer/Public/RenderCommon/SSRenderUtilFuncs.h>

#include "DX12GALRenderDevice.h"

#include "SSRenderer/Public/RenderInstance/IRICubeMap.h"


#include "Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "Private/DX12/DX12CommonUtils/DX12TransientConstantBufferAllocator.h"
#include "Private/DX12/GALPostProcessContext/DX12GALPPCDeferredShading.h"
#include "Private/DX12/GALRenderAsset/DX12GALSkinnedMeshAssetWrapper.h"
#include "Private/DX12/GALRenderInstance/DX12GALRICubeMap.h"
#include "Private/DX12/GALRenderInstance/DX12GALRIDirectionalLightShadowMapMetadata.h"
#include "Private/DX12/GALRenderInstance/DX12GALRIMetadata_SKM.h"
#include "Private/DX12/GALRenderTarget/DX12GALSwapChainRenderTarget.h"
#include "Private/PCommon/TestCodes/GALTestCodes.h"
#include "SSGAL/Private/DX12/DX12CommonUtils/DDSTextureLoader12/DDSTextureLoader12.h"
#include "SSGAL/Private/DX12/GALRenderAsset/DX12GALMeshAssetWrapper.h"
#include "SSGAL/Private/DX12/GALRenderAsset/DX12GALSimpleLineMeshAssetWrapper.h"
#include "SSGAL/Private/DX12/GALRenderAsset/DX12GALTextureAssetWrapper.h"
#include "SSGAL/Private/DX12/GALRenderAsset/GALMaterialAssets/DX12GALDefaultPBRMaterialAsset.h"
#include "SSGAL/Private/DX12/GALRenderInstance/DX12GALRIMetadata_SM.h"
#include "SSGAL/Private/DX12/GALRenderInstance/DX12GALRWMetaData.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALCPUReadableTexture.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALDSVRenderTarget.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALRenderTargetBase.h"
#include "SSGAL/Private/DX12/GALResourceUpdater/DX12GALResourceUpdater.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOPool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOWrapper.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12RootSignaturePool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12RootSignatureWrapper.h"
#include "SSGAL/Private/PCommon/GALPrivateGlobals.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBAModelBuffer.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBARenderEnvParam.h"

#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"
#include "SSRenderer/Public/RenderCommon/SSVertexType.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataBase.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"
#include "SSRenderer/Public/RenderInstance/IRISkinnedMesh.h"
#include "SSRenderer/Public/RenderInstance/Descriptors/LightDesc.h"
#include "SSRenderer/Public/RenderInstance/Light/IRenderLightDirectional.h"



DX12GALRenderDeviceContext::DX12GALRenderDeviceContext(DX12GALRenderDevice* InRenderDevice):
	_BoundRenderTargets(RT_NUM_MAX),
	_RenderLightsToDraw(32)
{
	HRESULT hr = S_OK;

	ID3D12Device5* D3DDevice = InRenderDevice->GetD3DDevice();

	_OwnerRenderDevice = InRenderDevice;


	// Create Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		hr = D3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_D3DCommandQueue));
		if (FAILED(hr))
		{
			DEBUG_BREAK();
			return;
		}
		_D3DCommandQueue->SetName(L"D3DCommandQueue");


		hr = D3DDevice->CreateFence(_FenceCnt, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_Fence));
		if (FAILED(hr))
		{
			SS_INTERRUPT();
		}
		_Fence->SetName(L"RenderDeviceFence");

		_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (FAILED(_FenceEvent))
		{
			SS_INTERRUPT();
		}
	}

	
	for (int32 i = 0; i < GAL_NESTED_FRAME_CNT; i++)
	{
		ID3D12CommandAllocator* NewCommandAllocator = nullptr;
		ID3D12GraphicsCommandList* NewCommandList = nullptr;
		if (FAILED(D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&NewCommandAllocator))))
		{
			SS_INTERRUPT();
		}

		if (FAILED(D3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, NewCommandAllocator, nullptr, IID_PPV_ARGS(&NewCommandList))))
		{
			SS_INTERRUPT();
		}


		NewCommandList->Close();

		_DrawWorkerCommandAllocators[i] = NewCommandAllocator;
		_DrawWorkerCommandLists[i] = NewCommandList;
	}

	for (int32 i=0;i<GAL_NESTED_FRAME_CNT;i++)
	{
		_TransientCBAllocator[i] = DBG_NEW DX12TransientConstantBufferAllocator(
			this,
			GAL_RESOURCE_DEFAULT_ALIGNMENT_SIZE,
			GAL_CONSTANTBUFFER_MIN_SIZE,
			256,
			L"DX12GALRenderDevice::_TransientConstantBufferAllocator");

		_ResourceUpdater[i] = DBG_NEW DX12GALResourceUpdater(InRenderDevice, this);
	}


	if (_OwnerRenderDevice->IsDebugEnabled())
	{
		TestTransientAllocator(this);
	}
}

void DX12GALRenderDeviceContext::Release()
{
	CloseHandle(_FenceEvent);
	_Fence->Release();
	_D3DCommandQueue->Release();


	for (int32 i = 0; i < GAL_NESTED_FRAME_CNT; i++)
	{
		_DrawWorkerCommandLists[i]->Release();
		_DrawWorkerCommandAllocators[i]->Release();

		_ResourceUpdater[i]->Release();

		_TransientCBAllocator[i]->ReleaseDefaultPages();
		_TransientCBAllocator[i]->Release();
	}

	delete this;
}

bool DX12GALRenderDeviceContext::IsValid() const
{
	return _DrawWorkerCommandLists[0] != nullptr;
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

void DX12GALRenderDeviceContext::FinalizeDeviceContext()
{
	uint64 CompletedValue = _Fence->GetCompletedValue();
	// NestedFrame이 아니라 바로 직전 프레임의 작업이 끝나기를 기다립니다.
	if (CompletedValue < _FenceCnt)
	{
		_Fence->SetEventOnCompletion(_FenceCnt, _FenceEvent);
		WaitForSingleObject(_FenceEvent, INFINITE);
	}
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


	if (InMeshAsset->GetMeshType() == EMeshType::SimpleLine)
	{
		DX12GALSimpleLineMeshAssetWrapper* NewGALMeshAsset = DBG_NEW DX12GALSimpleLineMeshAssetWrapper(InMeshAsset, this);
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

void DX12GALRenderDeviceContext::GenerateGALRI(IRenderInstance* InRenderInstance) const
{
	//	SCOPE_PROFILE(GenerateGALRI);
	const ERenderInstanceType RIType = InRenderInstance->GetRIType();
	const int32 FrameMod = RenderFrameInfo::GetFrameMod();


	if (InRenderInstance->GetGALMetadata(FrameMod) != nullptr)
	{
		return;
	}

	if (RIType == ERenderInstanceType::StaticMesh)
	{
		IRIMesh* InIRIMesh = static_cast<IRIMesh*>(InRenderInstance);
		DX12GALRIMetadata_SM* NewGALRI = DBG_NEW DX12GALRIMetadata_SM(_OwnerRenderDevice, InIRIMesh);
		InIRIMesh->InjectGALMetadataXXX(NewGALRI, FrameMod);
	}
	else if (RIType == ERenderInstanceType::SkinnedMesh)
	{
		IRISkinnedMesh* InIRIMesh = static_cast<IRISkinnedMesh*>(InRenderInstance);
		DX12GALRIMetadata_SKM* NewGALRI = DBG_NEW DX12GALRIMetadata_SKM(_OwnerRenderDevice, InIRIMesh);
		NewGALRI->SyncBonePose();
		InIRIMesh->InjectGALMetadataXXX(NewGALRI, FrameMod);
	}
	else if (RIType == ERenderInstanceType::Light)
	{
		IRenderLight* InRenderLight = static_cast<IRenderLight*>(InRenderInstance);

		ELightType LightType = InRenderLight->GetLightType();
		if (LightType == ELightType::Directional)
		{
			IRenderLightDirectional* DirectionalLight = static_cast<IRenderLightDirectional*>(InRenderLight);

			if (DirectionalLight->IsShadowMapEnabled())
			{
				DX12GALRIDirectionalLightShadowMapMetadata* NewShadowMapMetadata = 
					DBG_NEW DX12GALRIDirectionalLightShadowMapMetadata(
						static_cast<DX12GALRenderDevice*>(_OwnerRenderDevice), DirectionalLight);

				DirectionalLight->InjectGALMetadataXXX(NewShadowMapMetadata, FrameMod);
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

		InCubeMap->InjectGALMetadataXXX(NewGALRICubeMap, FrameMod);
	}
	else
	{
		DEBUG_BREAK();
		return;
	}
}

void DX12GALRenderDeviceContext::SyncGALRI(IRenderInstance* RIToSync, const IRenderCamera* CameraToSync) const
{
	const int32 FrameMod = RenderFrameInfo::GetFrameMod();
	GALRIMetadata* GALRIMetaData = RIToSync->GetGALMetadata(FrameMod);
	if (GALRIMetaData == nullptr)
	{
		SS_INTERRUPT();
		return;
	}

	const ERenderInstanceType GALRIType = GALRIMetaData->GetMetadataRenderInstanceType();
	const ERenderInstanceType RIType = RIToSync->GetRIType();
	if (RIType != GALRIType)
	{
		SS_INTERRUPT();
		return;
	}


	if (RIType == ERenderInstanceType::StaticMesh)
	{
		const IRIMesh* lRIMesh = static_cast<IRIMesh*>(RIToSync);
		DX12GALRIMetadata_SM* GALRIStatic = static_cast<DX12GALRIMetadata_SM*>(GALRIMetaData);

		GALRIStatic->_ModelCBSysMemAddr->WMatrix = XMMatrixTranspose(lRIMesh->GetWorldTransformMatrix());
		GALRIStatic->_ModelCBSysMemAddr->RotMatrix = XMMatrixTranspose(lRIMesh->GetWorldRotationMatrix());
		GALRIStatic->_ModelCBSysMemAddr->ObjectID = lRIMesh->GetGameObjectID().GetNativeValue();
	}
	else if (RIType == ERenderInstanceType::SkinnedMesh)
	{
		const IRISkinnedMesh* lRISkinnedMesh = static_cast<IRISkinnedMesh*>(RIToSync);
		DX12GALRIMetadata_SKM* GALRISkinned = static_cast<DX12GALRIMetadata_SKM*>(GALRIMetaData);

		GALRISkinned->_ModelCBSysMemAddr->WMatrix = XMMatrixTranspose(lRISkinnedMesh->GetWorldTransformMatrix());
		GALRISkinned->_ModelCBSysMemAddr->RotMatrix = XMMatrixTranspose(lRISkinnedMesh->GetWorldRotationMatrix());
		GALRISkinned->_ModelCBSysMemAddr->ObjectID = lRISkinnedMesh->GetGameObjectID().GetNativeValue();
		GALRISkinned->SyncBonePose();
	}
	else if (RIType == ERenderInstanceType::Light)
	{
		const IRenderLight* RILight = static_cast<IRenderLight*>(RIToSync);
		if (RILight->IsShadowMapEnabled() == false)
		{
			return;
		}

		GALRIShadowMapMetadata* GALRIShadowMap = static_cast<GALRIShadowMapMetadata*>(RILight->GetGALMetadata(FrameMod));

		const ELightType LightType = RILight->GetLightType();
		const ELightType GALLightType = GALRIShadowMap->GetLightType();
		if (LightType != GALLightType)
		{
			SS_INTERRUPT();
			return;
		}

		if (LightType == ELightType::Directional)
		{
			const IRenderLightDirectional* DirectionalLight = static_cast<const IRenderLightDirectional*>(RILight);
			DX12GALRIDirectionalLightShadowMapMetadata* DirectionalLightShadowMapMetadata = static_cast<DX12GALRIDirectionalLightShadowMapMetadata*>(GALRIShadowMap);

			DirectionalLightShadowMapMetadata->_ShadowMapCBSysMemAddr->VPMatrix =
				XMMatrixTranspose(DirectionalLight->CalcShadowMapVPMatrix(CameraToSync));

			// CBARenderEnvParam::ViewrPos는 PS에서 쓰이지 여기 VS에선 쓰이지 않음

		}
		else
		{
			SS_ASSERT(false);
		}
	}
	else if (RIType == ERenderInstanceType::CubeMap)
	{
		DX12GALRenderDevice* OwnerDX12RenderDevice = static_cast<DX12GALRenderDevice*>(_OwnerRenderDevice);
		ID3D12Device5* D3DDevice = OwnerDX12RenderDevice->GetD3DDevice();


		IRICubeMap* CubeMapToDraw = (IRICubeMap*)RIToSync;
		DX12GALRICubeMap* DX12GALCubeMap = static_cast<DX12GALRICubeMap*>(CubeMapToDraw->GetGALMetadata(FrameMod));

		Transform CubemapModelTransform;
		const Transform& CamTransform = CameraToSync->GetCameraTransform();

		const Vector4f& CamPos = CamTransform.Position;
		const float CubeMapSize = CubeMapToDraw->GetCubeMapSize();
		CubemapModelTransform.Position = Vector4f(CamPos.X, CamPos.Y, CamPos.Z, 1);
		CubemapModelTransform.Scale = Vector4f(CubeMapSize, CubeMapSize, CubeMapSize, 0);

		DX12GALCubeMap->GetCubemapCBModelSysmem()->ObjectID = 0; // 일단 사용 안함
		DX12GALCubeMap->GetCubemapCBModelSysmem()->RotMatrix = XMMatrixIdentity();
		DX12GALCubeMap->GetCubemapCBModelSysmem()->WMatrix = XMMatrixTranspose(CubemapModelTransform.AsMatrix());



		const float CamAspectRatio = CameraToSync->GetAspectRatio();
		const float CamFOV = CameraToSync->GetFOVWithRadians();
		XMMATRIX ProjMat = XMMatrixPerspectiveFovLH(CamFOV, CamAspectRatio, 0.001, CubeMapSize * 2);


		Vector4f Direction = CamTransform.GetForward();
		Vector4f Up = CamTransform.GetUp();
		XMMATRIX ViewMat = XMMatrixLookToLH(CamPos.SimdVec, Direction.SimdVec, Up.SimdVec);


		const XMMATRIX VPMatrix = ViewMat * ProjMat;
		DX12GALCubeMap->GetCubemapCBRenderEnvParamSysmem()->ViewerPos = CamPos.SimdVec;
		DX12GALCubeMap->GetCubemapCBRenderEnvParamSysmem()->VPMatrix = XMMatrixTranspose(VPMatrix);


		ITextureAsset* TextureAsset = CubeMapToDraw->GetCubemapTexture();
		const DX12GALTextureAssetWrapper* DX12GALTexAsset = static_cast<const DX12GALTextureAssetWrapper*>(TextureAsset->GetGALTextureAsset());

		CD3DX12_CPU_DESCRIPTOR_HANDLE CubemapDescTableCPU = DX12GALCubeMap->GetCubemapDescTableCPU();
		D3DDevice->CopyDescriptorsSimple(1,
			CubemapDescTableCPU, DX12GALTexAsset->_SRVHandle,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	else
	{
		SS_ASSERT(false);
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


	SyncGALRI(InLightToDrawShadowMap, _CurRenderCamera);

	const int32 FrameMod = RenderFrameInfo::GetFrameMod();
	GALRIShadowMapMetadata* GALRIShadowMapMetaData = 
		static_cast<GALRIShadowMapMetadata*>(InLightToDrawShadowMap->GetGALMetadata(FrameMod));

	_DrawingShadowMapMetadata = GALRIShadowMapMetaData;

	GALRenderTarget* ShadowMap = GALRIShadowMapMetaData->GetShadowMap();

	ResourceBarrier(ShadowMap, EResourceStateType::Common, EResourceStateType::DepthWrite);
	ClearRenderTarget(ShadowMap, Vector4f::Zero);
	SetRenderTarget(0, nullptr, ShadowMap);
}

void DX12GALRenderDeviceContext::EndDrawShadowMap()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawShadow)
	{
		SS_INTERRUPT();
	}
	_TaskPhase = ERenderDeviceTaskPhase::TaskWaiting;


	GALRenderTarget* ShadowMap = _DrawingShadowMapMetadata->GetShadowMap();
	ResourceBarrier(ShadowMap, EResourceStateType::DepthWrite, EResourceStateType::Common);

	_DrawingShadowMapMetadata = nullptr;
}

void DX12GALRenderDeviceContext::SetRenderCamera(IRenderCamera* InCamera)
{
	const int32 FrameMod = RenderFrameInfo::GetFrameMod();

	_CurRenderCamera = InCamera;

	DX12GALRenderDevice* OwnerDX12RenderDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice);

	IRenderWorld* RenderWorldToStartDraw = InCamera->GetIcludedRenderWorld();
	if (RenderWorldToStartDraw == nullptr)
	{
		SS_INTERRUPT();
		return;
	}

	_CurRenderWorld = RenderWorldToStartDraw;
	_CurRenderWorldGALData = (DX12GALRWMetaData*)RenderWorldToStartDraw->GetGALMetadata(FrameMod);
	if (_CurRenderWorldGALData == nullptr)
	{
		_CurRenderWorldGALData = DBG_NEW DX12GALRWMetaData(OwnerDX12RenderDevice, RenderWorldToStartDraw);
		RenderWorldToStartDraw->InjectGALMetadataXXX(_CurRenderWorldGALData, FrameMod);
	}


	_CurRenderWorldGALData->_RenderEnvCBSysMemAddr->VPMatrix = XMMatrixTranspose(InCamera->CalcVPMatrix());
	_CurRenderWorldGALData->_RenderEnvCBSysMemAddr->ViewerPos = InCamera->GetCameraTransform().Position.SimdVec;
}

void DX12GALRenderDeviceContext::AddRenderLightToDraw(IRenderLight* InLight)
{
	_RenderLightsToDraw.PushBack(InLight);
	SS_ASSERT(_RenderLightsToDraw.GetSize() == 1);
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
//	SCOPE_PROFILE(Set_PSO_RS);
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

	{
//		SCOPE_PROFILE(DX12_SetPSO_RS);
		CurCommandList->SetGraphicsRootSignature(lDX12RootSignatureWrapper->GetRootSignatureInstantce());
		CurCommandList->SetPipelineState(lDX12PSOWrapper->GetPipelineState());
	}

}


void DX12GALRenderDeviceContext::SetRenderTarget(int32 NumRenderTargets, GALRenderTarget** InRenderTargets,
                                                 GALRenderTarget* InDepthStencilView)
{
//	SCOPE_PROFILE(SetRT);
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

void DX12GALRenderDeviceContext::DrawSkyMap(IRICubeMap* CubeMapToDraw)
{
	if (GetTaskPhase() != ERenderDeviceTaskPhase::PostProcess)
	{
		SS_INTERRUPT();
		return;
	}

	SyncGALRI(CubeMapToDraw, _CurRenderCamera);


	const int32 FrameMod = RenderFrameInfo::GetFrameMod();

	IMeshAsset* CubeMeshAsset = g_CommonRenderAssetSet->GetCube1mMesh();
	const DX12GALMeshAssetWrapper* DX12CubeMeshAsset = static_cast<const DX12GALMeshAssetWrapper*>(CubeMeshAsset->GetGALMeshAsset());
	const MeshRawDataDefault* DefaultMeshRawData = static_cast<const MeshRawDataDefault*>(CubeMeshAsset->GetMeshRawData());
	int32 IndexCnt = DefaultMeshRawData->_VertexHeader.indexDataCnt[0];

	DX12GALRICubeMap* DX12GALCubeMap = static_cast<DX12GALRICubeMap*>(CubeMapToDraw->GetGALMetadata(FrameMod));
	ID3D12DescriptorHeap* CubemapDescHeap = DX12GALCubeMap->GetCubeMapDescHeap();
	CD3DX12_GPU_DESCRIPTOR_HANDLE CubemapDescTableGPU = DX12GALCubeMap->GetCubemapDescTableGPU();


	PipelineDesc SkyMapPSODesc = ConstructPSOToDrawSkyMap();
	SetPSOAndRootSignature(SkyMapPSODesc);


	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &DX12CubeMeshAsset->_VertexBufferView);
	CurCommandList->IASetIndexBuffer(DX12CubeMeshAsset->_IndexBufferView);

	CurCommandList->SetGraphicsRootConstantBufferView(0, DX12GALCubeMap->GetCubemapCBModelGPUMem());
	CurCommandList->SetGraphicsRootConstantBufferView(1, DX12GALCubeMap->GetCubemapCBRenderEnvParamGPUMem());

	CurCommandList->SetDescriptorHeaps(1, &CubemapDescHeap);
	CurCommandList->SetGraphicsRootDescriptorTable(2, CubemapDescTableGPU);

	CurCommandList->DrawIndexedInstanced(IndexCnt, 1, 0, 0, 0);
}

void DX12GALRenderDeviceContext::DrawMesh(IRenderInstance* InRenderInstance)
{
//	SCOPE_PROFILE_INDEXED(DrawMeshItem, InRenderInstance->GetGameObjectID().GetNativeValue());
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawMesh)
	{
		SS_INTERRUPT();
		return;
	}

	if (InRenderInstance->GetRIType() == ERenderInstanceType::StaticMesh)
	{
		DrawStaticMesh(static_cast<IRIMesh*>(InRenderInstance));
	}
	else if (InRenderInstance->GetRIType() == ERenderInstanceType::SkinnedMesh)
	{
		DrawSkinnedMesh(static_cast<IRISkinnedMesh*>(InRenderInstance));
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

	if (InMesh->GetMeshType() == EMeshType::SimpleLine)
	{
		DrawDebugLineList(
			InMesh,
			TransformMatrix,
			RotMatrix,
			InColor,
			bUseDepth
		);
	}
	else
	{
		DrawDebugMeshWire(
			InMesh,
			TransformMatrix,
			RotMatrix,
			InColor,
			bUseDepth
		);
	}

}

void DX12GALRenderDeviceContext::DrawDebugLineList(
	const IMeshAsset* InMesh,
	const XMMATRIX& TransformMatrix,
	const XMMATRIX& RotMatrix,
	const Vector4f& InColor,
	bool bUseDepth)
{
	const int32 FrameMod = RenderFrameInfo::GetFrameMod();

	if (_TaskPhase != ERenderDeviceTaskPhase::DrawDebug)
	{
		SS_INTERRUPT();
	}

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	SSTransientMemAllocator* TransientMemAllocator = GetTransientCBAllocator(FrameMod);

	GALRenderTarget* RTDepth = nullptr;
	if (bUseDepth)
	{
		RTDepth = GetThisFrameBoundDSV();
	}
	PipelineDesc Desc = ConstructPSOToDrawDebugLineList(RTDepth);
	SetPSOAndRootSignature(Desc);

	TransientChunkHeader MeshTransformCBChunk = TransientMemAllocator->AllocChunk(sizeof(CBAModelBuffer));
	DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)MeshTransformCBChunk.PageContent;
	CBAModelBuffer* ModelCBSystemAddr = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + MeshTransformCBChunk.ChunkOffset);
	D3D12_GPU_VIRTUAL_ADDRESS ModelCBGPUAdddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + MeshTransformCBChunk.ChunkOffset;
	ModelCBSystemAddr->WMatrix = XMMatrixTranspose(TransformMatrix);
	ModelCBSystemAddr->RotMatrix = XMMatrixTranspose(RotMatrix);
	CurCommandList->SetGraphicsRootConstantBufferView(0, ModelCBGPUAdddr);
	CurCommandList->SetGraphicsRootConstantBufferView(1, _CurRenderWorldGALData->_RenderEnvCBGPUMemAddr);

	TransientChunkHeader DrawColorCBChunk = TransientMemAllocator->AllocChunk(sizeof(Vector4f));
	DX12ConstantBufferResourcePage* ColorCBPage = (DX12ConstantBufferResourcePage*)DrawColorCBChunk.PageContent;
	Vector4f* ColorCBSystemAddr = reinterpret_cast<Vector4f*>(ColorCBPage->ResourceSysMem + DrawColorCBChunk.ChunkOffset);
	D3D12_GPU_VIRTUAL_ADDRESS ColorCBGPUAdddr = ColorCBPage->D3D12Resource->GetGPUVirtualAddress() + DrawColorCBChunk.ChunkOffset;
	(*ColorCBSystemAddr) = InColor;
	CurCommandList->SetGraphicsRootConstantBufferView(2, ColorCBGPUAdddr);

	const DX12GALSimpleLineMeshAssetWrapper* GALMeshAsset = static_cast<const DX12GALSimpleLineMeshAssetWrapper*>(InMesh->GetGALMeshAsset());
	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAsset->_VertexBufferView);
	CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView);
	CurCommandList->DrawIndexedInstanced(GALMeshAsset->_IndexCnt, 1, 0, 0, 0);
}

void DX12GALRenderDeviceContext::DrawDebugLines(
	const SimpleLineColorVertex* InVertexData,
	int32 VertexCount,
	bool bUseDepth)
{
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawDebug)
	{
		SS_INTERRUPT();
		return;
	}
	if (VertexCount <= 0)
	{
		return;
	}

	const int32 FrameMod = RenderFrameInfo::GetFrameMod();
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	SSTransientMemAllocator* TransientMemAllocator = GetTransientCBAllocator(FrameMod);

	GALRenderTarget* RTDepth = bUseDepth ? GetThisFrameBoundDSV() : nullptr;
	PipelineDesc Desc = ConstructPSOToDrawDebugLines(RTDepth);
	SetPSOAndRootSignature(Desc);

	TransientChunkHeader ModelCBChunk = TransientMemAllocator->AllocChunk(sizeof(CBAModelBuffer));
	DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)ModelCBChunk.PageContent;
	CBAModelBuffer* ModelCBSysMem = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + ModelCBChunk.ChunkOffset);
	D3D12_GPU_VIRTUAL_ADDRESS ModelCBGPUAddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + ModelCBChunk.ChunkOffset;
	ModelCBSysMem->WMatrix = XMMatrixIdentity();
	ModelCBSysMem->RotMatrix = XMMatrixIdentity();
	CurCommandList->SetGraphicsRootConstantBufferView(0, ModelCBGPUAddr);
	CurCommandList->SetGraphicsRootConstantBufferView(1, _CurRenderWorldGALData->_RenderEnvCBGPUMemAddr);

	const int32 VertexStride = sizeof(SimpleLineColorVertex);
	const int32 VertexDataSize = VertexStride * VertexCount;
	TransientChunkHeader VBChunk = TransientMemAllocator->AllocChunk(VertexDataSize);
	DX12ConstantBufferResourcePage* VBPage = (DX12ConstantBufferResourcePage*)VBChunk.PageContent;
	void* VBSysMem = VBPage->ResourceSysMem + VBChunk.ChunkOffset;
	D3D12_GPU_VIRTUAL_ADDRESS VBGPUAddr = VBPage->D3D12Resource->GetGPUVirtualAddress() + VBChunk.ChunkOffset;
	memcpy(VBSysMem, InVertexData, VertexDataSize);

	D3D12_VERTEX_BUFFER_VIEW TransientVBView = {};
	TransientVBView.BufferLocation = VBGPUAddr;
	TransientVBView.SizeInBytes = (UINT)VertexDataSize;
	TransientVBView.StrideInBytes = (UINT)VertexStride;

	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &TransientVBView);
	CurCommandList->DrawInstanced((UINT)VertexCount, 1, 0, 0);
}

void DX12GALRenderDeviceContext::EndDrawDebug()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::DrawDebug)
	{
		SS_INTERRUPT();
	}

	_TaskPhase = ERenderDeviceTaskPhase::TaskWaiting;
}

void DX12GALRenderDeviceContext::Present(GALRenderTarget* SwapChainToPresent)
{
	if (SwapChainToPresent->GetRenderTargetType() != ERenderTargetType::SwapChain)
	{
		SS_INTERRUPT();
	}

	DX12GALSwapChainRenderTarget* DX12SwapChain = static_cast<DX12GALSwapChainRenderTarget*>(SwapChainToPresent);
	HRESULT hr = DX12SwapChain->Present();
	if (FAILED(hr))
	{
		SS_INTERRUPT();
	}
}

void DX12GALRenderDeviceContext::DrawShadow(IRenderInstance* InRenderInstance)
{
//	SCOPE_PROFILE_INDEXED(DrawShadowItem, InRenderInstance->GetGameObjectID().GetNativeValue());

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


void DX12GALRenderDeviceContext::DrawStaticMesh(IRIMesh* RIToDraw)
{
//	SCOPE_PROFILE(Draw_SM);
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();

	const int32 FrameMod = RenderFrameInfo::GetFrameMod();

	// GAL Info
	DX12GALRIMetadata_SM* DX12RenderInstanceMetaData = (DX12GALRIMetadata_SM*)RIToDraw->GetGALMetadata(FrameMod);

	// Scrap Mesh Asset
	IMeshAsset* lMeshAsset = RIToDraw->GetMeshAsset();
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
//		SCOPE_PROFILE(MeshBind);

		PipelineDesc NewPipelineDesc = ConstructPSODescToDrawMesh(
			EMeshType::Rigid,
			EMaterialType::DefaultPBR,
			_BoundRenderTargets.GetSize(),
			_BoundRenderTargets.GetData(),
			GetThisFrameBoundDSV());
		SetPSOAndRootSignature(NewPipelineDesc);


		{
//			SCOPE_PROFILE(DX12);
			CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);
			CurCommandList->SetGraphicsRootConstantBufferView(0, DX12RenderInstanceMetaData->_ModelCBGPUMemAddr);
			CurCommandList->SetGraphicsRootConstantBufferView(1, _CurRenderWorldGALData->_RenderEnvCBGPUMemAddr);
		}
	}


	{
//		SCOPE_PROFILE(SubMeshBindAndDraw);

		for (int32 i = 0; i < SubMeshCnt; i++)
		{
			IMaterialAsset* MtlAsset = RIToDraw->GetMaterialAsset(i);
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

			int32 CurIdxDataCnt = DefaultMeshRawData->_VertexHeader.indexDataCnt[i];


			_UniqueDescHeapWorkTable.Clear();
			ListPushBackUnique(_UniqueDescHeapWorkTable, GALMaterial->_CachedMtlTexSRVDescHeap); // 메테리얼용
			ListPushBackUnique(_UniqueDescHeapWorkTable, _CurRenderWorldGALData->GetLightSettingDescHeap()); // 섀도우맵용

			// DescHeaps 
			CurCommandList->SetDescriptorHeaps(_UniqueDescHeapWorkTable.GetSize(), _UniqueDescHeapWorkTable.GetData());
			// ~DescHeaps 

			// Material
			CurCommandList->SetGraphicsRootConstantBufferView(2, GALMaterial->_MtlCBGPUMemAddr); // b2 -> Mtl용 CB
			CurCommandList->SetGraphicsRootDescriptorTable(3, GALMaterial->_MtlTexSRVDescTableGPU); // 메테리얼 디스크립터 테이블 바인딩
			// ~Material

			// RenderEnv
			CurCommandList->SetGraphicsRootConstantBufferView(4, _CurRenderWorldGALData->GetRenderLightParamCB()); // b4 -> RenderEnv용 CB
			CurCommandList->SetGraphicsRootDescriptorTable(5, _CurRenderWorldGALData->GetLightSeetingDescTable()); // ShadowMapBinding
			// ~RenderEnv

			// Draw
			CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);
			CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
			// ~Draw
		}
	}
}

void DX12GALRenderDeviceContext::DrawSkinnedMesh(IRISkinnedMesh* RIToDraw)
{
//	SCOPE_PROFILE(Draw_SKM);
	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();

	const int32 FrameMod = RenderFrameInfo::GetFrameMod();

	// Mesh Transform Update
	DX12GALRIMetadata_SKM* DX12SkinnedRIMetaData = static_cast<DX12GALRIMetadata_SKM*>(RIToDraw->GetGALMetadata(FrameMod));

	// Scrap Mesh Asset
	IMeshAsset* lMeshAsset = RIToDraw->GetMeshAsset();
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
//		SCOPE_PROFILE(MeshBind);
		PipelineDesc NewPipelineDesc = ConstructPSODescToDrawMesh(
			EMeshType::Skinned,
			EMaterialType::DefaultPBR,
			_BoundRenderTargets.GetSize(),
			_BoundRenderTargets.GetData(),
			GetThisFrameBoundDSV());
		SetPSOAndRootSignature(NewPipelineDesc);

		// Set Vertex Buffer and Mesh Transform, RenderEnv CB
		{
//			SCOPE_PROFILE(DX12);
			CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);
			CurCommandList->SetGraphicsRootConstantBufferView(0, DX12SkinnedRIMetaData->_ModelCBGPUMemAddr);
			CurCommandList->SetGraphicsRootConstantBufferView(1, _CurRenderWorldGALData->_RenderEnvCBGPUMemAddr);
		}
	}


	{
//		SCOPE_PROFILE(SubMeshBindAndDraw);
		for (int32 i = 0; i < SubMeshCnt; i++)
		{
			IMaterialAsset* MtlAsset = RIToDraw->GetMaterialAsset(i);
			DX12GALDefaultPBRMaterialAsset* GALMaterial = nullptr;
			if (MtlAsset != nullptr)
			{
				const MtlDataBase* MtlData = MtlAsset->GetMtlData();
				if (MtlData->_Type == EMaterialType::DefaultPBR)
				{
					GALMaterial = static_cast<DX12GALDefaultPBRMaterialAsset*>(MtlAsset->GetGALMaterialAsset());
				}
			}

			if (MtlAsset == nullptr || GALMaterial == nullptr)
			{
				const IMaterialAsset* EmptyMtl = g_CommonRenderAssetSet->GetEmptyPBRMaterial();
				GALMaterial = static_cast<DX12GALDefaultPBRMaterialAsset*>(EmptyMtl->GetGALMaterialAsset());

			}

			int32 CurIdxDataCnt = SkinnedMeshRawData->_VertexHeader.indexDataCnt[i];




			_UniqueDescHeapWorkTable.Clear();
			ListPushBackUnique(_UniqueDescHeapWorkTable, GALMaterial->_CachedMtlTexSRVDescHeap); // Mtl
			ListPushBackUnique(_UniqueDescHeapWorkTable, _CurRenderWorldGALData->GetLightSettingDescHeap()); // ShadowMap
			ListPushBackUnique(_UniqueDescHeapWorkTable, DX12SkinnedRIMetaData->_CachedJointSRVDescHeap); // Bone

			// DescHeaps
			CurCommandList->SetDescriptorHeaps(_UniqueDescHeapWorkTable.GetSize(), _UniqueDescHeapWorkTable.GetData());
			// ~DescHeaps

			// Mtl
			CurCommandList->SetGraphicsRootConstantBufferView(2, GALMaterial->_MtlCBGPUMemAddr); // b2 -> Mtl용 CB
			CurCommandList->SetGraphicsRootDescriptorTable(3, GALMaterial->_MtlTexSRVDescTableGPU); // 메테리얼 디스크립터 테이블 바인딩
			// ~Mtl


			// RenderEnv
			CurCommandList->SetGraphicsRootConstantBufferView(4, _CurRenderWorldGALData->GetRenderLightParamCB()); // GALWorld의 RenderEnv 바인딩
			CurCommandList->SetGraphicsRootDescriptorTable(5, _CurRenderWorldGALData->GetLightSeetingDescTable()); // ShadowMapBinding
			// ~RenderEnv

			// Skinning
			CurCommandList->SetGraphicsRootDescriptorTable(6, DX12SkinnedRIMetaData->_JointSRVDescTableGPU); // SkinningBinding
			// ~Skinning

			// Draw
			CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);
			CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
			// ~Draw
		}
	}
}

void DX12GALRenderDeviceContext::DrawShadowStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat,
                                                      const XMMATRIX& DrawRotMat)
{
//	SCOPE_PROFILE(DrawShadow_SM);

	const int32 FrameMod = RenderFrameInfo::GetFrameMod();

	DX12GALRIMetadata_SM* DX12RenderInstanceMetaData = static_cast<DX12GALRIMetadata_SM*>(RIToDraw->GetGALMetadata(FrameMod));

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();


	// Scrap Mesh Asset
	IMeshAsset* lMeshAsset = RIToDraw->GetMeshAsset();
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
//	SCOPE_PROFILE(DrawShadow_SKM);

	const int32 FrameMod = RenderFrameInfo::GetFrameMod();

	DX12GALRIMetadata_SKM* DX12RenderInstanceMetaData = static_cast<DX12GALRIMetadata_SKM*>(RIToDraw->GetGALMetadata(FrameMod));

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();


	// Scrap Mesh Asset
	IMeshAsset* lMeshAsset = RIToDraw->GetMeshAsset();
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

void DX12GALRenderDeviceContext::DrawDebugMeshWire(const IMeshAsset* InMesh, const XMMATRIX& TransformMatrix,
	const XMMATRIX& RotMatrix, const Vector4f& InColor, bool bUseDepth)
{
	const int32 FrameMod = RenderFrameInfo::GetFrameMod();


	if (_TaskPhase != ERenderDeviceTaskPhase::DrawDebug)
	{
		SS_INTERRUPT();
	}

	ID3D12GraphicsCommandList* CurCommandList = GetCurrentDrawWorkerCmdList();
	SSTransientMemAllocator* TransientMemAllocator = GetTransientCBAllocator(FrameMod);


	GALRenderTarget* RTDepth = nullptr;
	if (bUseDepth)
	{
		RTDepth = GetThisFrameBoundDSV();
	}
	PipelineDesc Desc = ConstructPSOToDrawDebugWire(RTDepth);
	SetPSOAndRootSignature(Desc);




	TransientChunkHeader MeshTransformCBChunk = TransientMemAllocator->AllocChunk(sizeof(CBAModelBuffer));
	DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)MeshTransformCBChunk.PageContent;
	CBAModelBuffer* ModelCBSystemAddr = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + MeshTransformCBChunk.ChunkOffset);
	D3D12_GPU_VIRTUAL_ADDRESS ModelCBGPUAdddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + MeshTransformCBChunk.ChunkOffset;
	ModelCBSystemAddr->WMatrix = XMMatrixTranspose(TransformMatrix);
	ModelCBSystemAddr->RotMatrix = XMMatrixTranspose(RotMatrix);
	CurCommandList->SetGraphicsRootConstantBufferView(0, ModelCBGPUAdddr);
	CurCommandList->SetGraphicsRootConstantBufferView(1, _CurRenderWorldGALData->_RenderEnvCBGPUMemAddr);



	TransientChunkHeader DrawColorCBChunk = TransientMemAllocator->AllocChunk(sizeof(Vector4f));
	DX12ConstantBufferResourcePage* ColorCBPage = (DX12ConstantBufferResourcePage*)DrawColorCBChunk.PageContent;
	Vector4f* ColorCBSystemAddr = reinterpret_cast<Vector4f*>(ColorCBPage->ResourceSysMem + DrawColorCBChunk.ChunkOffset);
	D3D12_GPU_VIRTUAL_ADDRESS ColorCBGPUAdddr = ColorCBPage->D3D12Resource->GetGPUVirtualAddress() + DrawColorCBChunk.ChunkOffset;
	(*ColorCBSystemAddr) = InColor;
	CurCommandList->SetGraphicsRootConstantBufferView(2, ColorCBGPUAdddr);



	int32 SubMeshCnt = InMesh->GetSubMeshCnt();
	const DX12GALMeshAssetWrapper* GALMeshAsset = static_cast<const DX12GALMeshAssetWrapper*>(InMesh->GetGALMeshAsset());
	const D3D12_VERTEX_BUFFER_VIEW& GALMeshAssetVertexBuffer = GALMeshAsset->_VertexBufferView;
	const MeshRawDataDefault* RawData = static_cast<const MeshRawDataDefault*>(InMesh->GetMeshRawData());

	// DrawDebugLineList나 DrawDebugLines에서 LineList로 그리는 기능이 있기 때문에 여기서 한 번 더 세팅 해줘야 한다.
	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->IASetVertexBuffers(0, 1, &GALMeshAssetVertexBuffer);

	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		CurCommandList->IASetIndexBuffer(&GALMeshAsset->_IndexBufferView[i]);
		int32 CurIdxDataCnt = RawData->_VertexHeader.indexDataCnt[i];
		CurCommandList->DrawIndexedInstanced(CurIdxDataCnt, 1, 0, 0, 0);
	}
}

ID3D12GraphicsCommandList* DX12GALRenderDeviceContext::GetCurrentDrawWorkerCmdList() const
{
	const int32 FrameMod = RenderFrameInfo::GetFrameMod();
	return _DrawWorkerCommandLists[FrameMod];
}

void DX12GALRenderDeviceContext::ResetRenderState()
{
	SCOPE_PROFILE(ResetRenderState);

	const int32 FrameMod = RenderFrameInfo::GetFrameMod();

	_TransientCBAllocator[FrameMod]->ResetAllChunksXXX();

	_ResourceUpdater[FrameMod]->ResetUpdateBuffer();
	ResetCurFrameCommandList();

	_RenderLightsToDraw.Clear();
	_BoundRenderTargets.Clear();
	_BoundDSV = nullptr;

	_CurRenderCamera = nullptr;
	_CurRenderWorld = nullptr;
	_CurRenderWorldGALData = nullptr;

	_DrawingShadowMapMetadata = nullptr;
	_LastSetPSO = PipelineDesc(); // 초기화
}

void DX12GALRenderDeviceContext::FenceFrame()
{
	_FenceCnt++;
	_D3DCommandQueue->Signal(_Fence, _FenceCnt);
	// 1. 프레임 숫자를 늘리고 늘어난 숫자를 시그널함
}

void DX12GALRenderDeviceContext::WaitForNestedGPUJob()
{
	SCOPE_PROFILE(WaitForNestedGPUJob);

	// GAL_NESTED_FRAME_CNT 이전 프레임 기다려야 함
	int64 AwaitValue = _FenceCnt - GAL_NESTED_FRAME_CNT + 1;
	if (AwaitValue <= 0)
	{
		return; // _D3DCommandQueue->Signal를 하는 첫 프레임은 1번임
	}

	
//	AwaitValue = _FenceCnt; // 테스트용

	const uint64 CompletedValue = _Fence->GetCompletedValue();
	if (CompletedValue < AwaitValue)
	{
		_Fence->SetEventOnCompletion(AwaitValue, _FenceEvent);
		WaitForSingleObject(_FenceEvent, INFINITE);
	}
}

void DX12GALRenderDeviceContext::ResetCurFrameCommandList()
{
	HRESULT hr;

	int32 FrameMOD = RenderFrameInfo::GetFrameMod();

	ID3D12CommandAllocator* CurDrawWorkerCommandAllcator = _DrawWorkerCommandAllocators[FrameMOD];
	hr = CurDrawWorkerCommandAllcator->Reset();
	if (FAILED(hr)) SS_INTERRUPT();

	ID3D12GraphicsCommandList* CurDrawWorkerCmdList = _DrawWorkerCommandLists[FrameMOD];
	hr = CurDrawWorkerCmdList->Reset(CurDrawWorkerCommandAllcator, nullptr);
	if (FAILED(hr)) SS_INTERRUPT();

}

void DX12GALRenderDeviceContext::BeginRender()
{
	WaitForNestedGPUJob();

	if (_TaskPhase != ERenderDeviceTaskPhase::TaskDenial)
	{
		SS_INTERRUPT();
	}
	_TaskPhase = ERenderDeviceTaskPhase::TaskWaiting;

	ResetRenderState();
}

void DX12GALRenderDeviceContext::WaitForCommandExecuteFinish()
{
	if (_TaskPhase != ERenderDeviceTaskPhase::TaskWaiting)
	{
		SS_INTERRUPT();
	}
	_TaskPhase = ERenderDeviceTaskPhase::TaskDenial;

	ID3D12GraphicsCommandList* CurGraphicsCommandList = GetCurrentDrawWorkerCmdList();
	HRESULT hr = CurGraphicsCommandList->Close();
	if (FAILED(hr)) SS_INTERRUPT();


	{
		SCOPE_PROFILE(ExecuteCommandList);

		ID3D12CommandList* CurCommandList = CurGraphicsCommandList;
		_D3DCommandQueue->ExecuteCommandLists(1, &CurCommandList);
	}
}

void DX12GALRenderDeviceContext::EndRender()
{
	FenceFrame();
}
