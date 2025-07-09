#include "SSRenderer.h"


#include "RenderWorld.h"

#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"
#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"

#include "SSRenderer/Private/RenderAsset/AssetManagerBase.h"
#include "SSRenderer/Private/RenderInstance/RIStaticMesh.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"


#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"


SSRenderer::SSRenderer(GALRenderDevice* InRenderDevice) :
	_RenderInstancesToDraw(1000)
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

IRIMesh* SSRenderer::CreateRIStaticMesh()
{
	return DBG_NEW RIStaticMesh();
}


void SSRenderer::AddGALStateChangedAsset(IAssetBase* AssetToChange)
{
	switch (AssetToChange->GetAssetType())
	{
	case EAssetType::Mesh:
		_GALStateChangedMeshAsset.PushBack((IMeshAssetMutable*)AssetToChange);
		break;
	case EAssetType::Texture:
		SS_ASSERT_MSG(false, L"TODO: 구현하기");
		break;
	default:
		SS_ASSERT(false);
		break;
	}
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
	RTDesc.Format = ERTColorFormat::R32G32_SINT;
	RTDesc.InitialResourceState = EResourceStateType::CopySrc;
	_PixelPickerRenderTarget = _GALRenderDevice->CreateRenderTarget(RTDesc, L"PixelPickerRenderTarget");
}

void SSRenderer::PerFrame()
{
	// RenderTime
	{
		_RenderInstancesToDraw.Clear();
		ScrapRenderInstsances(_RenderInstancesToDraw, _CurRenderCamera);
	}



	// GALTime
	_GALRenderDevice->BeginRender();
	{
		_MainDeviceContext->BeginRender();
		{
			InstantiatePendingGALAssets(_MainDeviceContext);


			// Set Camera Setting
			{
				XMMATRIX VPMatrix = _CurRenderCamera->GetVPMatrix();
				VPMatrix = XMMatrixTranspose(VPMatrix);
				_MainDeviceContext->SetCameraVPTransform(VPMatrix);
				_MainDeviceContext->SetCameraPosition(_CurRenderCamera->GetCameraTransform().Position.SimdVec);
			}


			// Default Render Target
			{
				_MainDeviceContext->ResourceBarrier(_GALRenderDevice->GetDefaultViewportRenderTarget(), EResourceStateType::Present, EResourceStateType::RenderTarget);
				_MainDeviceContext->ClearRenderTarget(_GALRenderDevice->GetDefaultViewportRenderTarget());

				if (_CurRenderCamera->GetSpecificRenderTarget() == nullptr)
				{
					_MainDeviceContext->SetRenderTarget(_GALRenderDevice->GetDefaultViewportRenderTarget());
				}
				else
				{
					_MainDeviceContext->SetRenderTarget(_CurRenderCamera->GetSpecificRenderTarget());
				}


				for (IRenderInstance* Item : _RenderInstancesToDraw)
				{
					_MainDeviceContext->Draw(Item);
				}

				_MainDeviceContext->ResourceBarrier(_GALRenderDevice->GetDefaultViewportRenderTarget(), EResourceStateType::RenderTarget, EResourceStateType::Present);
			}

			
			// Pixel Picker RenderTarget
			{
				_MainDeviceContext->ResourceBarrier(_PixelPickerRenderTarget, EResourceStateType::CopySrc, EResourceStateType::RenderTarget);
				_MainDeviceContext->ClearRenderTarget(_PixelPickerRenderTarget);
				_MainDeviceContext->SetRenderTarget(_PixelPickerRenderTarget);

				for (IRenderInstance* Item : _RenderInstancesToDraw)
				{
					_MainDeviceContext->DrawID(Item);
				}

				_MainDeviceContext->ResourceBarrier(_PixelPickerRenderTarget, EResourceStateType::RenderTarget, EResourceStateType::CopySrc);
			}
			

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

void SSRenderer::InstantiatePendingGALAssets(GALRenderDeviceContext* Executor)
{
	for (IMeshAssetMutable* MeshAssetItem : _GALStateChangedMeshAsset)
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

	_GALStateChangedMeshAsset.Clear();
}



void SSRenderer::ScrapRenderInstsances(SS::PooledList<IRenderInstance*>& OutRenderInstancesToDraw, IRenderCamera* InCamera)
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
		OutRenderInstancesToDraw.PushBack(InstanceItem);
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
