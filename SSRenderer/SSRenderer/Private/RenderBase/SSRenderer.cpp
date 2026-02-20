#include "SSRenderer.h"

#include "RenderWorld.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSGAL/Public/GALRenderAsset/GALMaterialAssetWrapperBase.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"
#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"
#include "SSGAL/Public/GALRenderTarget/GALCPUReadableTexture.h"
#include "SSGAL/Public/GALPostProcessContext/GALPPCDeferredShading.h"

#include "SSRenderer/Private/RenderAsset/AssetManagerBase.h"
#include "SSRenderer/Private/RenderAsset/CommonRenderAsset/CommonRenderAssetSet.h"
#include "SSRenderer/Private/RenderInstance/RenderCamera.h"
#include "SSRenderer/Private/RenderInstance/RenderLightDirectional.h"
#include "SSRenderer/Private/RenderInstance/RICubeMap.h"
#include "SSRenderer/Private/RenderInstance/RISkinnedMesh.h"
#include "SSRenderer/Private/RenderInstance/RIStaticMesh.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"


#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"
#include "SSRenderer/Private/RenderInstance/RICubeMap.h"


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

IRISkinnedMesh* SSRenderer::CreateRISkinnedMesh()
{
	return DBG_NEW RISkinnedMesh();
}

IRenderCamera* SSRenderer::CreateRenderCamera()
{
	return DBG_NEW RenderCamera();
}

IRICubeMap* SSRenderer::CreateRICubeMap()
{
	return DBG_NEW RICubeMap();
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

const IRenderCamera* SSRenderer::GetMainRenderCamera() const
{
	return _MainRenderCamera;
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
	return NewRenderWorld;
}

void SSRenderer::SetMainRenderCamera(IRenderCamera* InCamera)
{
	GALRenderTarget* SwapChainBuffer = _GALRenderDevice->GetDefaultViewportRenderTarget();
	Vector2f ViewportWidthHeight = SwapChainBuffer->GetViewportBoxSize().WidthHeight;
	InCamera->SetAspectRatio(ViewportWidthHeight.X / ViewportWidthHeight.Y);

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
		DSVDesc.bUseSRV = true;
		_DSVRenderTarget = _GALRenderDevice->CreateDepthStencilView(DSVDesc, L"Main_DSV");
	}


	// G-Buffer Normal
	{
		GALRenderTargetDesc RTDesc;
		RTDesc.ResourceWidth = SwapChainBufferSize.X;
		RTDesc.ResourceHeight = SwapChainBufferSize.Y;
		RTDesc.ScissorRectSize.Min = Vector2f(0, 0);
		RTDesc.ScissorRectSize.Max = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
		RTDesc.DrawBoxSize.WidthHeight = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.MinDepth = 0.f;
		RTDesc.DrawBoxSize.MaxDepth = 1.f;
		RTDesc.Format = ERTColorFormat::R32G32B32A32_FLOAT;
		RTDesc.InitialResourceState = EResourceStateType::Common;
		RTDesc.bUseSRV = true;
		_RTGBufferNormal = _GALRenderDevice->CreateRenderTarget(RTDesc, L"_RTGBufferNormal");

	}

	// G-Buffer Albedo
	{
		GALRenderTargetDesc RTDesc;
		RTDesc.ResourceWidth = SwapChainBufferSize.X;
		RTDesc.ResourceHeight = SwapChainBufferSize.Y;
		RTDesc.ScissorRectSize.Min = Vector2f(0, 0);
		RTDesc.ScissorRectSize.Max = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
		RTDesc.DrawBoxSize.WidthHeight = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.MinDepth = 0.f;
		RTDesc.DrawBoxSize.MaxDepth = 1.f;
		RTDesc.Format = ERTColorFormat::R32G32B32A32_FLOAT;
		RTDesc.InitialResourceState = EResourceStateType::Common;
		RTDesc.bUseSRV = true;
		_RTGBufferAlbedo = _GALRenderDevice->CreateRenderTarget(RTDesc, L"_RTGBufferAlbedo");
	}

	// G-Buffer WorldPos
	{
		GALRenderTargetDesc RTDesc;
		RTDesc.ResourceWidth = SwapChainBufferSize.X;
		RTDesc.ResourceHeight = SwapChainBufferSize.Y;
		RTDesc.ScissorRectSize.Min = Vector2f(0, 0);
		RTDesc.ScissorRectSize.Max = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
		RTDesc.DrawBoxSize.WidthHeight = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.MinDepth = 0.f;
		RTDesc.DrawBoxSize.MaxDepth = 1.f;
		RTDesc.Format = ERTColorFormat::R32G32B32A32_FLOAT;
		RTDesc.InitialResourceState = EResourceStateType::Common;
		RTDesc.bUseSRV = true;
		_RTGBufferWorldPos = _GALRenderDevice->CreateRenderTarget(RTDesc, L"_RTGBufferWorldPos");
	}

	// G-Buffer MetallicRoughness
	{
		GALRenderTargetDesc RTDesc;
		RTDesc.ResourceWidth = SwapChainBufferSize.X;
		RTDesc.ResourceHeight = SwapChainBufferSize.Y;
		RTDesc.ScissorRectSize.Min = Vector2f(0, 0);
		RTDesc.ScissorRectSize.Max = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
		RTDesc.DrawBoxSize.WidthHeight = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.MinDepth = 0.f;
		RTDesc.DrawBoxSize.MaxDepth = 1.f;
		RTDesc.Format = ERTColorFormat::R32G32_FLOAT;
		RTDesc.InitialResourceState = EResourceStateType::Common;
		RTDesc.bUseSRV = true;
		_RTGBufferMetallicRoughness = _GALRenderDevice->CreateRenderTarget(RTDesc, L"_RTGBufferMetallicRoughness");
	}

	// G-Buffer Emissive
	{
		GALRenderTargetDesc RTDesc;
		RTDesc.ResourceWidth = SwapChainBufferSize.X;
		RTDesc.ResourceHeight = SwapChainBufferSize.Y;
		RTDesc.ScissorRectSize.Min = Vector2f(0, 0);
		RTDesc.ScissorRectSize.Max = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
		RTDesc.DrawBoxSize.WidthHeight = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.MinDepth = 0.f;
		RTDesc.DrawBoxSize.MaxDepth = 1.f;
		RTDesc.Format = ERTColorFormat::R32G32B32A32_FLOAT;
		RTDesc.InitialResourceState = EResourceStateType::Common;
		RTDesc.bUseSRV = true;
		_RTGBufferEmissive = _GALRenderDevice->CreateRenderTarget(RTDesc, L"_RTGBufferEmissive");
	}

	// PostProcessResult
	{
		GALRenderTargetDesc RTDesc;
		RTDesc.ResourceWidth = SwapChainBufferSize.X;
		RTDesc.ResourceHeight = SwapChainBufferSize.Y;
		RTDesc.ScissorRectSize.Min = Vector2f(0, 0);
		RTDesc.ScissorRectSize.Max = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
		RTDesc.DrawBoxSize.WidthHeight = Vector2f(SwapChainBufferSize.X, SwapChainBufferSize.Y);
		RTDesc.DrawBoxSize.MinDepth = 0.f;
		RTDesc.DrawBoxSize.MaxDepth = 1.f;
		RTDesc.Format = ERTColorFormat::R8G8B8A8_UNORM;
		RTDesc.InitialResourceState = EResourceStateType::Common;
		_RTPostProcessResult = _GALRenderDevice->CreateRenderTarget(RTDesc, L"_RTPostProcessResult");
	}

	// DeferredShadingContext
	{
		_DeferredShadingContext = _GALRenderDevice->CreateDeferredShadingPostProcessContext();
		_DeferredShadingContext->SetRTNormal(_RTGBufferNormal);
		_DeferredShadingContext->SetRTAlbedo(_RTGBufferAlbedo);
		_DeferredShadingContext->SetRTWorldPos(_RTGBufferWorldPos);
		_DeferredShadingContext->SetRTMetallicRoughness(_RTGBufferMetallicRoughness);
		_DeferredShadingContext->SetRTEmissive(_RTGBufferEmissive);
		_DeferredShadingContext->SyncGALPPCParam();
	}
}

void SSRenderer::PerFrame()
{
	// RenderTime
	{
		RenderWorld* WorldToScrap = (RenderWorld*)_MainRenderCamera->GetIcludedRenderWorld();

		_RenderInstancesToDraw.Clear();
		_RenderLightsToDraw.Clear();
		_CubeMapToDraw = nullptr;
		ScrapRenderInstsances(
			_RenderInstancesToDraw,
			_RenderLightsToDraw,
			_CubeMapToDraw,
			_MainRenderCamera);
	}


	for (IRenderInstance* RIItem : _RenderInstancesToDraw)
	{
		_GALRenderDevice->SyncGALRIMetadataWithRI(RIItem);
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

			// Set Light Setting
			{
				for (IRenderLight* LightItem : _RenderLightsToDraw)
				{
					_MainDeviceContext->AddRenderLightToDraw(LightItem);
				}
			}

			// Shadow Map Draw
			{
				for (IRenderLight* LightItem : _RenderLightsToDraw)
				{
					if (LightItem->IsShadowMapEnabled() == false)
					{
						continue;
					}

					_MainDeviceContext->BeginDrawShadowMap(LightItem);

					for (IRenderInstance* ShadowCastingInstance : _RenderInstancesToDraw)
					{
						_MainDeviceContext->DrawShadow(ShadowCastingInstance);
					}

					_MainDeviceContext->EndDrawShadowMap();
				}
			}

			// Commit Render Light and Shadows
			_MainDeviceContext->CommitAddedRenderLights();



			// Default Render Target
			{
				{
					_MainDeviceContext->ResourceBarrier(_PixelPickerRenderTarget, EResourceStateType::CopySrc, EResourceStateType::RenderTarget);

					_MainDeviceContext->ResourceBarrier(_RTGBufferNormal, EResourceStateType::Common, EResourceStateType::RenderTarget);
					_MainDeviceContext->ResourceBarrier(_RTGBufferAlbedo, EResourceStateType::Common, EResourceStateType::RenderTarget);
					_MainDeviceContext->ResourceBarrier(_RTGBufferWorldPos, EResourceStateType::Common, EResourceStateType::RenderTarget);
					_MainDeviceContext->ResourceBarrier(_RTGBufferMetallicRoughness, EResourceStateType::Common, EResourceStateType::RenderTarget);
					_MainDeviceContext->ResourceBarrier(_RTGBufferEmissive, EResourceStateType::Common, EResourceStateType::RenderTarget);
				}

				{
					_MainDeviceContext->ClearRenderTarget(_RTGBufferNormal, Vector4f::Zero);
					_MainDeviceContext->ClearRenderTarget(_RTGBufferAlbedo, Vector4f::Zero);
					_MainDeviceContext->ClearRenderTarget(_RTGBufferWorldPos, Vector4f::Zero);
					_MainDeviceContext->ClearRenderTarget(_RTGBufferMetallicRoughness, Vector4f::Zero);
					_MainDeviceContext->ClearRenderTarget(_RTGBufferEmissive, Vector4f::Zero);

					_MainDeviceContext->ClearRenderTarget(_PixelPickerRenderTarget, Vector4f::Zero);

					_MainDeviceContext->ClearRenderTarget(_DSVRenderTarget, Vector4f::Zero);
				}


				GALRenderTarget* RenderTargets[RT_NUM_MAX] = { nullptr, };
				RenderTargets[0] = _RTGBufferNormal;
				RenderTargets[1] = _RTGBufferAlbedo;
				RenderTargets[2] = _RTGBufferWorldPos;
				RenderTargets[3] = _RTGBufferMetallicRoughness;
				RenderTargets[4] = _RTGBufferEmissive;
				RenderTargets[5] = _PixelPickerRenderTarget;
				_MainDeviceContext->SetRenderTarget(6, RenderTargets, _DSVRenderTarget);

				// TODO: BeginDrawMesh 랑 EndDrawMesh 만들기
				_MainDeviceContext->BeginDrawMesh();
				for (IRenderInstance* Item : _RenderInstancesToDraw)
				{
					_MainDeviceContext->DrawMesh(Item);
				}
				_MainDeviceContext->EndDrawMesh();

				{
					_MainDeviceContext->ResourceBarrier(_PixelPickerRenderTarget, EResourceStateType::RenderTarget, EResourceStateType::CopySrc);

					_MainDeviceContext->ResourceBarrier(_RTGBufferNormal, EResourceStateType::RenderTarget, EResourceStateType::Common);
					_MainDeviceContext->ResourceBarrier(_RTGBufferAlbedo, EResourceStateType::RenderTarget, EResourceStateType::Common);
					_MainDeviceContext->ResourceBarrier(_RTGBufferWorldPos, EResourceStateType::RenderTarget, EResourceStateType::Common);
					_MainDeviceContext->ResourceBarrier(_RTGBufferMetallicRoughness, EResourceStateType::RenderTarget, EResourceStateType::Common);
					_MainDeviceContext->ResourceBarrier(_RTGBufferEmissive, EResourceStateType::RenderTarget, EResourceStateType::Common);
				}
			}

			_MainDeviceContext->ResourceBarrier(_RTPostProcessResult, EResourceStateType::Common, EResourceStateType::RenderTarget);
			_MainDeviceContext->ResourceBarrier(_GALRenderDevice->GetDefaultViewportRenderTarget(), EResourceStateType::Present, EResourceStateType::CopyDest);

			// Post Processing
			_MainDeviceContext->BeginPostProcessing();
			{

				_MainDeviceContext->ClearRenderTarget(_RTPostProcessResult, { 0, 0, 0, 0 });
				_MainDeviceContext->SetRenderTarget(1, &_RTPostProcessResult, nullptr);

				if (_CubeMapToDraw != nullptr)
				{
					_MainDeviceContext->DrawSkyMap(_CubeMapToDraw);
				}

				_MainDeviceContext->ExecutePostProcessing(_DeferredShadingContext);
			}
			_MainDeviceContext->EndPostProcessing();

			// DrawDebug
			_MainDeviceContext->BeginDrawDebug();
			{
				if (_DebugDrawItemsWithDepth.GetSize() > 0)
				{
					_MainDeviceContext->SetRenderTarget(1, &_RTPostProcessResult, _DSVRenderTarget);

					for (const DebugDrawMeshDesc& DescItem : _DebugDrawItemsWithDepth)
					{
						_MainDeviceContext->DrawDebugWire(
							DescItem.MeshAsset,
							DescItem.WMatrix,
							DescItem.RotMatrix,
							DescItem.DrawColor,
							true);
					}

					_DebugDrawItemsWithDepth.Clear();
				}

				if (_DebugDrawItemsWithoutDepth.GetSize() > 0)
				{
					_MainDeviceContext->SetRenderTarget(1, &_RTPostProcessResult, nullptr);

					for (const DebugDrawMeshDesc& DescItem : _DebugDrawItemsWithoutDepth)
					{
						_MainDeviceContext->DrawDebugWire(
							DescItem.MeshAsset,
							DescItem.WMatrix,
							DescItem.RotMatrix,
							DescItem.DrawColor,
							false);
					}

					_DebugDrawItemsWithoutDepth.Clear();
				}
			}
			_MainDeviceContext->EndDrawDebug();

			_MainDeviceContext->ResourceBarrier(_RTPostProcessResult, EResourceStateType::RenderTarget, EResourceStateType::CopySrc);
			_MainDeviceContext->CopyRenderTarget(_GALRenderDevice->GetDefaultViewportRenderTarget(), _RTPostProcessResult);

			_MainDeviceContext->ResourceBarrier(_GALRenderDevice->GetDefaultViewportRenderTarget(), EResourceStateType::CopyDest, EResourceStateType::Present);
			_MainDeviceContext->ResourceBarrier(_RTPostProcessResult, EResourceStateType::CopySrc, EResourceStateType::Common);


			// Copy to Pixel Picker RenderTarget
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
	delete _DeferredShadingContext;
	_DeferredShadingContext = nullptr;

	delete _RTPostProcessResult;
	delete _RTGBufferEmissive;
	delete _RTGBufferMetallicRoughness;
	delete _RTGBufferWorldPos;
	delete _RTGBufferAlbedo;
	delete _RTGBufferNormal;
	_RTPostProcessResult = nullptr;
	_RTGBufferEmissive = nullptr;
	_RTGBufferMetallicRoughness = nullptr;
	_RTGBufferWorldPos = nullptr;
	_RTGBufferAlbedo = nullptr;
	_RTGBufferNormal = nullptr;

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

void SSRenderer::DrawWireFrame(const DebugDrawMeshDesc& Desc)
{
	if (Desc.bUseDepth)
	{
		_DebugDrawItemsWithDepth.PushBack(Desc);
	}
	else
	{
		_DebugDrawItemsWithoutDepth.PushBack(Desc);
	}
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


void SSRenderer::ScrapRenderInstsances(
	SS::PooledList<IRenderInstance*>& OutRenderInstancesToDraw,
	SS::PooledList<IRenderLight*>& OutRenderLightsToDraw,
	IRICubeMap*& OutCubeMapToDraw, 
	IRenderCamera* InCamera)
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
		else if (RIType == ERenderInstanceType::StaticMesh ||
				RIType == ERenderInstanceType::SkinnedMesh)
		{
			OutRenderInstancesToDraw.PushBack(InstanceItem);
		}
		else if (RIType == ERenderInstanceType::CubeMap)
		{
			if (OutCubeMapToDraw != nullptr)
			{
				SS_ASSERT(false, L"There are two or more Cubemap in one world.");
				continue;
			}
			OutCubeMapToDraw = static_cast<IRICubeMap*>(InstanceItem);
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
