#include "SSRenderer.h"

#include "SSContentsBase/SGameObject.h"
#include "SSContentsBase/SWorld.h"

#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALDefaultRenderTarget.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"

#include "SSRenderer/Public/SSRendererModuleEntry/SSRendererFactory.h"
#include "SSRenderer/Public/RenderAsset/MaterialAssetManager.h"
#include "SSRenderer/Public/RenderAsset/MeshAssetManager.h"
#include "SSRenderer/Public/RenderAsset/ModelAssetManager.h"
#include "SSRenderer/Public/RenderAsset/ModelCombinationAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/SSAssetBase.h"
#include "SSRenderer/Public/RenderInstance/RenderWorld.h"
#include "SSRenderer/Public/SObjectBase/SCameraComponent.h"


SSRenderer::SSRenderer()
{

}

SSRenderer::~SSRenderer()
{
}

void SSRenderer::AddModelInstanceReference(ModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer)
{
	NewModelAsset->AddAssetReference(Referencer);

	if (NewModelAsset->GetAssetInstanceReferenceCnt() > 1)
	{
		return; // 이미 에셋 로딩이 돼있을 것이기 때문에 패스
	}

	{
		SS::SHasherW MeshAssetName = NewModelAsset->GetMeshAssetName();
		if (MeshAssetName.IsEmpty())
		{
			SS_ASSERT(false);
			return;
		}

		MeshAsset* MeshAssetToInstantiate = (MeshAsset*)_meshAssetManager->FindAssetByName(MeshAssetName);
		if (MeshAssetToInstantiate == nullptr)
		{
			SS_ASSERT(false);
			return;
		}

		AssetInstanceReferencer ModelReferencer;
		ModelReferencer.Type = EAssetInstanceReferenceType::AssetName;
		ModelReferencer.AssetName = NewModelAsset->GetAssetName();
		AddMeshInstanceReference(MeshAssetToInstantiate, ModelReferencer);
	}
}

void SSRenderer::AddMeshInstanceReference(MeshAsset* NewMeshAsset, const AssetInstanceReferencer& Referencer)
{
	NewMeshAsset->AddAssetReference(Referencer);

	if (NewMeshAsset->GetAssetInstanceReferenceCnt() > 1)
	{
		return; // 이미 에셋 로딩이 돼있을 것이기 때문에 패스
	}

	_InstanceStateChangedMesh.PushBack(NewMeshAsset);
}

void SSRenderer::RemoveModelInstanceReference(ModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer)
{
	NewModelAsset->RemoveAssetReference(Referencer);

	if (NewModelAsset->GetAssetInstanceReferenceCnt() > 0)
	{
		return; // 언로드할게 없기 때문에 패스
	}

	SS::SHasherW MeshAssetName = NewModelAsset->GetMeshAssetName();
	if (MeshAssetName.IsEmpty())
	{
		SS_ASSERT(false);
		return;
	}

	MeshAsset* MeshAssetToInstantiate = (MeshAsset*)_meshAssetManager->FindAssetByName(MeshAssetName);
	if (MeshAssetToInstantiate == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	AssetInstanceReferencer ModelReferencer;
	ModelReferencer.Type = EAssetInstanceReferenceType::AssetName;
	ModelReferencer.AssetName = NewModelAsset->GetAssetName();
	RemoveMeshInstanceReference(MeshAssetToInstantiate, ModelReferencer);
}

void SSRenderer::RemoveMeshInstanceReference(MeshAsset* MeshAssetToRemove, const AssetInstanceReferencer& Referencer)
{
	MeshAssetToRemove->RemoveAssetReference(Referencer);
	if (MeshAssetToRemove->GetAssetInstanceReferenceCnt() > 0)
	{
		return; // 언로드할게 없기 때문에 패스
	}

	_InstanceStateChangedMesh.PushBack(MeshAssetToRemove);
}

RenderWorld* SSRenderer::CreateRenderWorld()
{
	RenderWorld* NewRenderWorld = DBG_NEW RenderWorld();
	NewRenderWorld->InitializeRenderWorld(this);
	return NewRenderWorld;
}

void SSRenderer::SetRenderCamera(SCameraComponent* InCamera)
{
	if (InCamera->_RenderTarget != nullptr)
	{
		SS_INTERRUPT();
		return;
	}

	InCamera->_RenderTarget = _GALRenderDevice->GetDefaultViewportRenderTarget();
	_CurRenderCamera = InCamera;
}

void SSRenderer::StartUp()
{
	InitAssetManagers();


	constexpr int32 BUFFER_WIDTH = 1024;
	constexpr int32 BUFFER_HEIGHT = 1024;

	GALRenderTargetDesc RTDesc;
	RTDesc.ResourceWidth = BUFFER_WIDTH;
	RTDesc.ResourceHeight = BUFFER_HEIGHT;
	RTDesc.ScissorRectSize.Min = Vector2f(0, 0);
	RTDesc.ScissorRectSize.Max = Vector2f(BUFFER_WIDTH, BUFFER_HEIGHT);
	RTDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
	RTDesc.DrawBoxSize.WidthHeight = Vector2f(BUFFER_WIDTH, BUFFER_HEIGHT);
	RTDesc.DrawBoxSize.MinDepth = 0.f;
	RTDesc.DrawBoxSize.MaxDepth = 1.f;
	RTDesc.Format = ERTColorFormat::R32_SINT;
	_PixelPickerRenderTarget = _GALRenderDevice->CreateRenderTarget(RTDesc, L"PixelPickerRenderTarget");
}

void SSRenderer::PerFrame()
{
	_GALRenderDevice->BeginRender();
	{
		_MainDeviceContext->BeginRender();
		{
			_MainDeviceContext->ResourceBarrier(_GALRenderDevice->GetDefaultViewportRenderTarget(), EResourceStateType::Present, EResourceStateType::RenderTarget);
			_MainDeviceContext->ClearRenderTarget(_GALRenderDevice->GetDefaultViewportRenderTarget());

			XMMATRIX VPMatrix = _CurRenderCamera->GetVPMatrix();
			VPMatrix = XMMatrixTranspose(VPMatrix);
			SGameObject* CameraObj = _CurRenderCamera->GetParent();
			_MainDeviceContext->SetCameraVPTransform(VPMatrix);
			_MainDeviceContext->SetCameraPosition(CameraObj->GetTransform().Position.SimdVec);
			_MainDeviceContext->SetRenderTarget(_CurRenderCamera->_RenderTarget);
	

			InstantiatePendingAssets(_MainDeviceContext);
			DrawRenderWorld(_MainDeviceContext, _CurRenderCamera);

			_MainDeviceContext->ResourceBarrier(_GALRenderDevice->GetDefaultViewportRenderTarget(), EResourceStateType::RenderTarget, EResourceStateType::Present);
		}
		_MainDeviceContext->EndRender();
		_GALRenderDevice->ExecuteRenderContext(_MainDeviceContext);
	}
	_GALRenderDevice->EndRender();
}

void SSRenderer::CleanUp()
{
	delete _PixelPickerRenderTarget;

	CleanupAssetMnagers();
	CleanupRenderer();
}

void SSRenderer::InstantiatePendingAssets(GALRenderDeviceContext* Executor)
{
	for (MeshAsset* MeshAssetItem : _InstanceStateChangedMesh)
	{
		if (MeshAssetItem->GetAssetInstanceReferenceCnt() > 0 && MeshAssetItem->_GALMeshAsset == nullptr)
		{
			Executor->GenerateMeshGALAsset((MeshAsset*)MeshAssetItem);
		}
		else if (MeshAssetItem->GetAssetInstanceReferenceCnt() <= 0 && MeshAssetItem->_GALMeshAsset != nullptr)
		{
			((MeshAsset*)MeshAssetItem)->ReleaseGALData();
		}
	}

	_InstanceStateChangedMesh.Clear();
}

void SSRenderer::DrawRenderWorld(GALRenderDeviceContext* Executor, SCameraComponent* InCamera)
{
	SGameObject* Parent = InCamera->GetParent();
	if (Parent == nullptr)
	{
		SS_INTERRUPT();
	}

	SWorld* World = Parent->GetIncludedWorldRef();
	if (World == nullptr)
	{
		DEBUG_BREAK();
		return;
	}

	RenderWorld* WorldToRender = World->GetRenderWorld();
	if (WorldToRender == nullptr)
	{
		DEBUG_BREAK();
		return;
	}

	const SS::HashMap<SObjHashCode, IRenderInstance*>& RenderInstanceMap = WorldToRender->GetRenderInstanceMap();
	for (const SS::pair<SObjHashCode, IRenderInstance*>& InstancePairItem : RenderInstanceMap)
	{
		IRenderInstance* InstanceItem = InstancePairItem.second;
		Executor->Draw(InstanceItem);
	}
}

void SSRenderer::InitAssetManagers()
{
	_meshAssetManager = DBG_NEW MeshAssetManager(1000, 100);

	_materialAssetManager = DBG_NEW MaterialAssetManager(1000, 1000);
	_materialAssetManager->InstantiateAllMaterialAssets();


	_ModelAssetManager = DBG_NEW ModelAssetManager(1000, 100);

	_ModelCombAssetManager = DBG_NEW ModelCombinationAssetManager();
}

void SSRenderer::CleanupRenderer()
{
	delete _MainDeviceContext;
	_MainDeviceContext = nullptr;

	delete _GALRenderDevice;
	_GALRenderDevice = nullptr;
}

void SSRenderer::CleanupAssetMnagers()
{
	_ModelCombAssetManager->ReleaseAllAssets();
	delete _ModelCombAssetManager;
	_ModelCombAssetManager = nullptr;

	_ModelAssetManager->ReleaseAllAssets();
	delete _ModelAssetManager;
	_ModelAssetManager = nullptr;

	_materialAssetManager->ReleaseAllMaterials();
	delete _materialAssetManager;
	_materialAssetManager = nullptr;

	_meshAssetManager->ReleaseAllAssets();
	delete _meshAssetManager;
	_meshAssetManager = nullptr;

}
