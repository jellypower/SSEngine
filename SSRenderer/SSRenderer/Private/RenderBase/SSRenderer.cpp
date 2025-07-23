#include "SSRenderer.h"

#include "RenderWorld.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSGAL/Public/GALRenderAsset/GALMaterialAssetWrapperBase.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"
#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"
#include "SSGAL/Public/GALRenderTarget/GALCPUReadableTexture.h"

#include "SSRenderer/Private/RenderAsset/AssetManagerBase.h"
#include "SSRenderer/Private/RenderAsset/CommonRenderAssetSet.h"
#include "SSRenderer/Private/RenderInstance/RenderCamera.h"
#include "SSRenderer/Private/RenderInstance/RenderLightDirectional.h"
#include "SSRenderer/Private/RenderInstance/RIStaticMesh.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"


#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"


SSRenderer::SSRenderer(GALRenderDevice* InRenderDevice) :
	_RenderInstancesToDraw(1000),
	_RenderLightsToDraw(10)
{
	_GALRenderDevice = InRenderDevice;
	_MainDeviceContext = _GALRenderDevice->CreateRenderDeviceContext();
	_AssetManager = DBG_NEW AssetManagerBase(1000, 10);
	_CommonRenderAssetSet = DBG_NEW CommonRenderAssetSet();
}

SSRenderer::~SSRenderer()
{
	delete _CommonRenderAssetSet;
}

ICommonRenderAssetSet* SSRenderer::GetCommonRenderAssetSet() const
{
	return _CommonRenderAssetSet;
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

IRenderCamera* SSRenderer::CreateRenderCamera()
{
	return DBG_NEW RenderCamera();
}

IRenderLightDirectional* SSRenderer::CreateDirectionalLight(const RenderLightDirectionalDesc& InDesc)
{
	return DBG_NEW RenderLightDirectional(InDesc);
}

SObjHashCode SSRenderer::GetPixelPickedObjectID() const
{
	return _PickedObjectHash;
}

Vector2f SSRenderer::GetViewportSize() const
{
	GALRenderTarget* ViewportRT = _GALRenderDevice->GetDefaultViewportRenderTarget();
	const ViewportBox& VB = ViewportRT->GetViewportBoxSize();
	return VB.WidthHeight;
}


void SSRenderer::AddGALStateChangedAsset(IAssetBase* AssetToChange)
{
	switch (AssetToChange->GetAssetType())
	{
	case EAssetType::Mesh:
		_GALStateChangedMeshAsset.PushBack((IMeshAssetMutable*)AssetToChange);
		break;
	case EAssetType::Texture:
		_GALStateChangedTextureAsset.PushBack((ITextureAssetMutable*)AssetToChange);
		break;
	case EAssetType::Material:
		_GALStateChangedMaterialAsset.PushBack((IMaterialAssetMutable*)AssetToChange);
		break;
	default:
		SS_ASSERT(false);
		break;
	}
}

IRenderWorld* SSRenderer::CreateRenderWorld(const utf16* InWorldName)
{
	if (InWorldName == nullptr) InWorldName = L"EMPTY_WorldName";
	
	RenderWorld* NewRenderWorld = DBG_NEW RenderWorld(InWorldName);
	NewRenderWorld->InitializeRenderWorld(this);
	return NewRenderWorld;
}

void SSRenderer::SetRenderCamera(IRenderCamera* InCamera)
{
	_MainRenderCamera = InCamera;
}

void SSRenderer::RequestPixelPicking(int32 X, int32 Y)
{
	_bPixelPickingReserved = true;
	_PixelPickingCoord = Vector2i32(X, Y);
}


void SSRenderer::StartUp()
{

	GALRenderTarget* SwapChainBuffer= _GALRenderDevice->GetDefaultViewportRenderTarget();
	Vector2i32 SwapChainBufferSize = SwapChainBuffer->GetResourceSize();

	{
		GALRenderTargetDesc IDDrawerRTDesc;
		IDDrawerRTDesc.ResourceWidth = SwapChainBufferSize.X;
		IDDrawerRTDesc.ResourceHeight = SwapChainBufferSize.Y;
		IDDrawerRTDesc.ScissorRectSize.Min = Vector2f(0, 0);
		IDDrawerRTDesc.ScissorRectSize.Max = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		IDDrawerRTDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
		IDDrawerRTDesc.DrawBoxSize.WidthHeight = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		IDDrawerRTDesc.DrawBoxSize.MinDepth = 0.f;
		IDDrawerRTDesc.DrawBoxSize.MaxDepth = 1.f;
		IDDrawerRTDesc.Format = ERTColorFormat::R32G32_SINT;
		IDDrawerRTDesc.InitialResourceState = EResourceStateType::CopySrc;
		_PixelPickerRenderTarget = _GALRenderDevice->CreateRenderTarget(IDDrawerRTDesc, L"PixelPickerRenderTarget");

		int32 Pitch = _PixelPickerRenderTarget->GetResourceRowPitch();
		_PixelPickerCPUReadableTex = _GALRenderDevice->CreateCPUReadableTexture(
			ERTColorFormat::R32G32_SINT,
			Vector2i32(SwapChainBufferSize.X, SwapChainBufferSize.Y),
			Pitch,
			L"PixelPickerCPUReadableTex");
	}

	{
		GALRenderTargetDesc DSVDesc;
		DSVDesc.ResourceWidth = SwapChainBufferSize.X;
		DSVDesc.ResourceHeight = SwapChainBufferSize.Y;
		DSVDesc.ScissorRectSize = SwapChainBuffer->GetScissorRectSize();
		DSVDesc.DrawBoxSize = SwapChainBuffer->GetViewportBoxSize();
		DSVDesc.Format = ERTColorFormat::D32_FLOAT;
		DSVDesc.InitialResourceState = EResourceStateType::DepthWrite;
		_DSVRenderTarget = _GALRenderDevice->CreateDepthStencilView(DSVDesc, L"Main_DSV");
	}
}

void SSRenderer::PerFrame()
{
	// RenderTime
	{
		_RenderInstancesToDraw.Clear();
		ScrapRenderInstsances(_RenderInstancesToDraw, _RenderLightsToDraw ,_MainRenderCamera);
	}



	_GALRenderDevice->BeginRender();
	{
		// TEMP Read PixelPicker
		if (_bPixelPickingReserved)
		{
			_PixelPickerCPUReadableTex->BeginRead();

			Vector2ui32 WindowSize = SSFrameInfo::GetWindowSize();
			
			int64 ObjectNativeID = 0; 
			int64* pObjectNativeID = (int64*)_PixelPickerCPUReadableTex->GetDataAtRatio(
				(float)_PixelPickingCoord.X / WindowSize.X,
				(float)_PixelPickingCoord.Y / WindowSize.Y);
			if (pObjectNativeID != nullptr)
			{
				ObjectNativeID = *pObjectNativeID;
			}
			_PickedObjectHash = SObjHashCode(ObjectNativeID);


			_PixelPickerCPUReadableTex->EndRead();

			_bPixelPickingReserved = false;
		}

		_MainDeviceContext->BeginRender();
		{
			InstantiatePendingGALAssets(_MainDeviceContext);


			// Set Camera Setting
			{
				_MainDeviceContext->SetRenderCamera(_MainRenderCamera);
			}


			// Default Render Target
			{
				_MainDeviceContext->ResourceBarrier(_GALRenderDevice->GetDefaultViewportRenderTarget(), EResourceStateType::Present, EResourceStateType::RenderTarget);
				_MainDeviceContext->ResourceBarrier(_PixelPickerRenderTarget, EResourceStateType::CopySrc, EResourceStateType::RenderTarget);

				_MainDeviceContext->ClearRenderTarget(_PixelPickerRenderTarget);
				_MainDeviceContext->ClearRenderTarget(_DSVRenderTarget);
				_MainDeviceContext->ClearRenderTarget(_GALRenderDevice->GetDefaultViewportRenderTarget());


				GALRenderTarget* RenderTargets[RT_NUM_MAX] = { nullptr, };
				RenderTargets[0] = _GALRenderDevice->GetDefaultViewportRenderTarget();
				RenderTargets[1] = _PixelPickerRenderTarget;
				_MainDeviceContext->SetRenderTarget(2, RenderTargets, _DSVRenderTarget);


				for (IRenderInstance* Item : _RenderInstancesToDraw)
				{
					_MainDeviceContext->Draw(Item);
				}

				_MainDeviceContext->ResourceBarrier(_PixelPickerRenderTarget, EResourceStateType::RenderTarget, EResourceStateType::CopySrc);
				_MainDeviceContext->ResourceBarrier(_GALRenderDevice->GetDefaultViewportRenderTarget(), EResourceStateType::RenderTarget, EResourceStateType::Present);
			}

			
			// Pixel Picker RenderTarget
			{
				_MainDeviceContext->CopyRenderTarget(_PixelPickerCPUReadableTex, _PixelPickerRenderTarget);
			}
		}
		_MainDeviceContext->EndRender();
		_GALRenderDevice->ExecuteRenderContext(_MainDeviceContext);
	}

	Before_GALRenderDevice_EndRender();
	_GALRenderDevice->EndRender();
}

void SSRenderer::CleanUp()
{
	delete _DSVRenderTarget;
	_DSVRenderTarget = nullptr;

	delete _PixelPickerCPUReadableTex;
	_PixelPickerCPUReadableTex = nullptr;

	delete _PixelPickerRenderTarget;
	_PixelPickerRenderTarget = nullptr;


	_GALRenderDevice->BeginRender(); // WaitForFence
	{
		_MainDeviceContext->BeginRender();
		InstantiatePendingGALAssets(_MainDeviceContext); // 잔여물이 남아있을 수도 있음
		_MainDeviceContext->EndRender();
	}
	_GALRenderDevice->EndRender();


	_AssetManager->ReleaseAllAssets();
	delete _AssetManager;
	_AssetManager = nullptr;

	delete _MainDeviceContext;
	_MainDeviceContext = nullptr;

	delete _GALRenderDevice;
	_GALRenderDevice = nullptr;
}

void SSRenderer::ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(void(* InCallback)())
{
	_OneTimeCallback_BeforeGALRenderDeviceEndRender.PushBack(InCallback);
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

	for (ITextureAssetMutable* TextureAssetItem : _GALStateChangedTextureAsset)
	{
		if (TextureAssetItem->GetAssetInstanceReferenceCnt() > 0 && TextureAssetItem->GetGALTextureAsset() == nullptr)
		{
			Executor->GenerateTextureGALAsset(TextureAssetItem);
		}
		else if (TextureAssetItem->GetAssetInstanceReferenceCnt() <= 0 && TextureAssetItem->GetGALTextureAsset() != nullptr)
		{
			TextureAssetItem->ReleaseGALData();
		}
	}

	for (IMaterialAssetMutable* MaterialAssetItem : _GALStateChangedMaterialAsset)
	{
		if (MaterialAssetItem->GetAssetInstanceReferenceCnt() > 0 && MaterialAssetItem->GetGALMaterialAsset() == nullptr)
		{
			Executor->GenerateMaterialGALAsset(MaterialAssetItem); // 레프 카운트가 0에서 올랐으면 생성
		}
		else if (MaterialAssetItem->GetAssetInstanceReferenceCnt() <= 0 && MaterialAssetItem->GetGALMaterialAsset() != nullptr)
		{
			MaterialAssetItem->ReleaseGALData(); // 레프 카운트가 0으로 떨어졌으면 파괴
		}
		else if (MaterialAssetItem->GetAssetInstanceReferenceCnt() > 0 && MaterialAssetItem->GetGALMaterialAsset() != nullptr)
		{
			MaterialAssetItem->GetGALMaterialAsset()->SyncMtlParam(); // 레프 카운트가 그대로면 변경
		}
	}

	_GALStateChangedMeshAsset.Clear();
	_GALStateChangedTextureAsset.Clear();
	_GALStateChangedMaterialAsset.Clear();
}


void SSRenderer::ScrapRenderInstsances(SS::PooledList<IRenderInstance*>& OutRenderInstancesToDraw,
	SS::PooledList<IRenderLight*>& OutRenderLightsToDraw, IRenderCamera* InCamera)
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
		ERenderInstanceType RIType = InstanceItem->GetRIType();
		if (RIType == ERenderInstanceType::Light)
		{
			OutRenderLightsToDraw.PushBack((IRenderLight*)InstanceItem);
		}
		else if (RIType == ERenderInstanceType::StaticMesh)
		{
			OutRenderInstancesToDraw.PushBack(InstanceItem);
		}
		else
		{
			SS_ASSERT_MSG(false, L"TODO: Implementation");
		}

	}
}

void SSRenderer::Before_GALRenderDevice_EndRender()
{
	for (void (*CallbackItem)() : _OneTimeCallback_BeforeGALRenderDeviceEndRender)
	{
		CallbackItem();
	}

	_OneTimeCallback_BeforeGALRenderDeviceEndRender.Clear();
}