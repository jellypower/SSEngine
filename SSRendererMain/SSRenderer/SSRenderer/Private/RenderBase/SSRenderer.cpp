#include "SSRenderer.h"


#include "RenderWorld.h"
#include "SSContentsBase/SWorld.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALDefaultRenderTarget.h" // TODO: Private 헤더파일 종속성 없애기

#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"
#include "SSRenderer/Private/RenderAsset/AssetManagerBase.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"


#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"


SSRenderer::SSRenderer(GALRenderDevice* InRenderDevice)
{
	_GALRenderDevice = InRenderDevice;
	_MainDeviceContext = _GALRenderDevice->CreateRenderDeviceContext();
}

SSRenderer::~SSRenderer()
{
}

IAssetManager* SSRenderer::GetAssetManager() const
{
	return _AssetManager;
}

IAssetManagerMutable* SSRenderer::GetMutableAssetManager()
{
	return _AssetManager;
}

void SSRenderer::AddModelInstanceReference(IModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer)
{
	NewModelAsset->AddAssetReference(Referencer);

	if (NewModelAsset->GetAssetInstanceReferenceCnt() > 1)
	{
		return; // 이미 에셋 로딩이 돼있을 것이기 때문에 패스
	}

	{
		IMeshAsset* MeshAssetToInstantiate = NewModelAsset->GetMeshAsset();
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

void SSRenderer::AddMeshInstanceReference(IMeshAsset* NewMeshAsset, const AssetInstanceReferencer& Referencer)
{
	NewMeshAsset->AddAssetReference(Referencer);

	if (NewMeshAsset->GetAssetInstanceReferenceCnt() > 1)
	{
		return; // 이미 에셋 로딩이 돼있을 것이기 때문에 패스
	}

	_InstanceStateChangedMesh.PushBack((IMeshAssetMutable*)NewMeshAsset);
}

void SSRenderer::RemoveModelInstanceReference(IModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer)
{
	NewModelAsset->RemoveAssetReference(Referencer);

	if (NewModelAsset->GetAssetInstanceReferenceCnt() > 0)
	{
		return; // 언로드할게 없기 때문에 패스
	}


	IMeshAsset* MeshAssetToInstantiate = NewModelAsset->GetMeshAsset();
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

void SSRenderer::RemoveMeshInstanceReference(IMeshAsset* MeshAssetToRemove, const AssetInstanceReferencer& Referencer)
{
	MeshAssetToRemove->RemoveAssetReference(Referencer);
	if (MeshAssetToRemove->GetAssetInstanceReferenceCnt() > 0)
	{
		return; // 언로드할게 없기 때문에 패스
	}

	_InstanceStateChangedMesh.PushBack((IMeshAssetMutable*)MeshAssetToRemove);
}

IRenderWorld* SSRenderer::CreateRenderWorld()
{
	RenderWorld* NewRenderWorld = DBG_NEW RenderWorld();
	NewRenderWorld->InitializeRenderWorld(this);
	return NewRenderWorld;
}

void SSRenderer::SetRenderCamera(IRenderCamera* InCamera)
{
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
			_MainDeviceContext->SetCameraVPTransform(VPMatrix);
			_MainDeviceContext->SetCameraPosition(_CurRenderCamera->GetCameraTransform().Position.SimdVec);

			if (_CurRenderCamera->GetSpecificRenderTarget() == nullptr)
			{
				_MainDeviceContext->SetRenderTarget(_GALRenderDevice->GetDefaultViewportRenderTarget());
			}
			else
			{
				_MainDeviceContext->SetRenderTarget(_CurRenderCamera->GetSpecificRenderTarget());
			}

	

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
	for (IMeshAssetMutable* MeshAssetItem : _InstanceStateChangedMesh)
	{
		if (MeshAssetItem->GetAssetInstanceReferenceCnt() > 0 && MeshAssetItem->GetGALMeshAsset() == nullptr)
		{
			Executor->GenerateMeshGALAsset(MeshAssetItem);
		}
		else if (MeshAssetItem->GetAssetInstanceReferenceCnt() <= 0 && MeshAssetItem->GetGALMeshAsset() != nullptr)
		{
			MeshAssetItem->ReleaseGALData();
		}
	}

	_InstanceStateChangedMesh.Clear();
}

void SSRenderer::DrawRenderWorld(GALRenderDeviceContext* Executor, IRenderCamera* InCamera)
{
	RenderWorld* WorldToRender = (RenderWorld*)InCamera->GetIcludedRenderWorld();
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
	_AssetManager = DBG_NEW AssetManagerBase(1000, 10);
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
	_AssetManager->ReleaseAllAssets();
	delete _AssetManager;
	_AssetManager = nullptr;
}
