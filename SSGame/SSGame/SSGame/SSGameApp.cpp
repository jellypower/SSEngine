#include "pch.h"
#include "SSGameApp.h"

#include <ctime>


#include "ModuleEntryScriptRunner.h"

#include "SSEngineDefault/Public/RawInput/SSInput.h"
#include "SSEngineDefault/Public/CommonTypes/DirEnums.h"
#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"
#include "SSEngineDefault/Public/WindowManager/IWindowManager.h"
#include "SSEngineDefault/Public/WindowManager/IWindow.h"

#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSContentsBase/Public/AnimComponents/SBlendSpaceAnimTestComponent.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightDirectionalComponent.h"
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SCharacterMovementComponent.h"
#include "SSContentsBase/Public/CollisionComp/SBoxColliderComponent.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"


#include "SSAssetDBManager/Public/IAssetDBLoader.h"

#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/CollisionBase/ICollDevice.h"

#include "SSGameModule/Public/SSGame.h"



SSGameApp* g_GameApp;


SSGameApp::SSGameApp(IRenderer* EngineRenderer, ICollDevice* EngineCollDevice)
{
	_Renderer = EngineRenderer;
	_CollDevice = EngineCollDevice;
}

SSGameApp::~SSGameApp()
{
}

void SSGameApp::StartupEngine()
{
	_Renderer->StartUp();


	{
		SS::PooledList<IAssetBase*> AssetListToImport(1024);

		_AssetDBLoader = g_fpCreateAssetDBLoader();

		_AssetDBLoader->StartLoadDB(CRAN::NS_DEFAULT_ASSET);
		_AssetDBLoader->LoadAllAssetDataFromDB();
		_AssetDBLoader->GenerateAssetInstancesFromInter();
		_AssetDBLoader->RelocateCreatedAssets(AssetListToImport);
		_AssetDBLoader->ClearDB();

		_AssetDBLoader->StartLoadDB(L"ContentsAssets");
		_AssetDBLoader->LoadAllAssetDataFromDB();
		_AssetDBLoader->GenerateAssetInstancesFromInter();
		_AssetDBLoader->RelocateCreatedAssets(AssetListToImport);
		_AssetDBLoader->ClearDB();


		IAssetManagerMutable* AM = _Renderer->GetMutableAssetManager();

		for (IAssetBase* AssetItem : AssetListToImport)
		{
			AM->AddToAssetPool(AssetItem);
		}


		_Renderer->GetCommonRenderAssetSet()->InitializeCommonAssets();
	}

	{
		ICollisionWorld* NewCollWorld = _CollDevice->CreateCollWorld("EditorCollWorld");
		IRenderWorld* NewRenderWorld = _Renderer->CreateRenderWorld();
		_DefaultWorld = NewSObject<SWorld>(L"World");
		_DefaultWorld->InitializeWorld(NewRenderWorld, NewCollWorld);
	}


	_Game = DBG_NEW SSGame(_DefaultWorld);
	_Game->SetInGameFocus(true);
	_Game->StartUpGame();
}

void SSGameApp::EnginePerFrame()
{
	SCOPE_PROFILE(Engine);
	g_FrameInfoProcessor->SetFramePhase(EFramePhase::Contents);


	{
		SCOPE_PROFILE(Contents);
		_Game->PerFrameGame();
		_DefaultWorld->PerFrameContents();
	}

	{
		SCOPE_PROFILE(SimulateCollision);
		{
			SCOPE_PROFILE(TransformCommit_Pre_Physics);
			g_FrameInfoProcessor->SetFramePhase(EFramePhase::Collision);
			_DefaultWorld->ProcessTransformCommit();
			SS_ASSERT(_DefaultWorld->DEBUG_Validate_TransformCommit());
		}
		_DefaultWorld->PerFrameCollision();
	}

	{
		SCOPE_PROFILE(Anim);
		_DefaultWorld->PerFrameAnim();
	}

	{
		SCOPE_PROFILE(Render);
		{
			SCOPE_PROFILE(TransformCommit_Pre_Render);
			g_FrameInfoProcessor->SetFramePhase(EFramePhase::Render);
			_DefaultWorld->ProcessTransformCommit();
			SS_ASSERT(_DefaultWorld->DEBUG_Validate_TransformCommit());
		}
		_DefaultWorld->ProcessDebugDraw(_Renderer);
		_Renderer->PerFrame();
	}
}

void SSGameApp::CleanupEngine()
{
	_Game->CleanupGame();
	delete _Game;

	_DefaultWorld->CleanupWorld();

	bool IsAnyObjectReminInWorld = _DefaultWorld->IsAnyObjectRemainInWorld();
	SS_ASSERT(IsAnyObjectReminInWorld == false);

	DelSObject(_DefaultWorld);
	_DefaultWorld = nullptr;

	_AssetDBLoader->ClearDB();
	delete _AssetDBLoader;
	_AssetDBLoader = nullptr;

	_Renderer->GetCommonRenderAssetSet()->ReleaseCachedAssets();


	_Renderer->FinalizeRendering();


	// _Renderer->CleanUp() 은 GALRenderDeviceContext::FinalizeDeviceContext 를 호출함
	// GPU작업이 전부 끝난 이후에야 리소스들을 지울 수 있음
	_Renderer->CleanUp();
	delete _Renderer;
	_Renderer = nullptr;

	delete _CollDevice;
	_CollDevice = nullptr;
}


void SSGameApp::MoveFreeCamera()
{
	if (SSInput::GetMouse(EMouseCode::MOUSE_RIGHT)) // 카메라 움직이기
	{
		constexpr float CAM_ROT_SPEED = 2;
		constexpr float CAM_XROT_MAX = 0.9;

		// 카메라 돌리기
		TEMP_CamYRot += SSInput::GetMouseDelta().X * CAM_ROT_SPEED;

		if (TEMP_CamXRot > -XM_PIDIV2 * CAM_XROT_MAX && SSInput::GetMouseDelta().Y > 0)
		{
			TEMP_CamXRot -= SSInput::GetMouseDelta().Y * CAM_ROT_SPEED;
		}

		if (TEMP_CamXRot < XM_PIDIV2 * CAM_XROT_MAX && SSInput::GetMouseDelta().Y < 0)
		{
			TEMP_CamXRot -= SSInput::GetMouseDelta().Y * CAM_ROT_SPEED;
		}

		const float DeltaTime = SSFrameInfo::GetDeltaTime();
		SGameObject* CamGO = _FreeCam->GetGameObject();
		Vector4f Forward = CamGO->GetTransform().GetForward();
		Vector4f Right = CamGO->GetTransform().GetRight();
		Vector4f Up = CamGO->GetTransform().GetUp();

		CamGO->SetRotation(Quaternion::FromEulerRotation(Vector4f(TEMP_CamXRot, TEMP_CamYRot, 0, 0)));

		// 카메라 속도조절
		float WheelDelta = SSInput::GetMouseWheelDelta();
		if (WheelDelta > 0.01 || WheelDelta < -0.01)
		{
			TEMP_Speed += (WheelDelta * 0.005);

			if (TEMP_Speed < 0.2)
			{
				TEMP_Speed = 0.2;
			}
			if (TEMP_Speed > 10.f)
			{
				TEMP_Speed = 10.f;
			}
		}


		// 카메라 무브
		if (SSInput::GetKey(EKeyCode::KEY_S))
		{
			Vector4f Pos = CamGO->GetTransform().Position;
			Pos = Pos + Forward * -DeltaTime * TEMP_Speed;
			CamGO->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_W))
		{
			Vector4f Pos = CamGO->GetTransform().Position;
			Pos = Pos + Forward * DeltaTime * TEMP_Speed;
			CamGO->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_D))
		{
			Vector4f Pos = CamGO->GetTransform().Position;
			Pos = Pos + Right * DeltaTime * TEMP_Speed;
			CamGO->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_A))
		{
			Vector4f Pos = CamGO->GetTransform().Position;
			Pos = Pos + Right * -DeltaTime * TEMP_Speed;
			CamGO->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_E))
		{
			Vector4f Pos = CamGO->GetTransform().Position;
			Pos = Pos + Up * DeltaTime * TEMP_Speed;
			CamGO->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_Q))
		{
			Vector4f Pos = CamGO->GetTransform().Position;
			Pos = Pos + Up * -DeltaTime * TEMP_Speed;
			CamGO->SetPosition(Pos);
		}
	}
}
