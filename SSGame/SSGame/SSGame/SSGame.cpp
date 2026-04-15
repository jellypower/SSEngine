#include "pch.h"
#include "SSGame.h"

#include "ModuleEntryScriptRunner.h"
#include "Character/SCharacterComponent.h"
#include "PlayerController/SCameraController.h"
#include "PlayerController/SPlayerController.h"

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

#include "SSAssetDBManager/Public/IAssetDBLoader.h"




SSGame* g_Game;

SSGame::SSGame(IRenderer* EngineRenderer)
{
	_Renderer = EngineRenderer;
}

SSGame::~SSGame()
{
}

void SSGame::StartupEngine()
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
		IRenderWorld* NewRenderWorld = _Renderer->CreateRenderWorld();
		_DefaultWorld = NewSObject<SWorld>(L"World");
		_DefaultWorld->InitializeWorld(NewRenderWorld);
	}

	StartUpContents();
}

void SSGame::EnginePerFrame()
{
	{
		SCOPE_PROFILE(Contents);
		PerFrameContents();
		_DefaultWorld->PerFrameContents();
	}

	{
		SCOPE_PROFILE(Anim);
		_DefaultWorld->PerFrameAnim();
	}

	{
		SCOPE_PROFILE(TransformCommit);
		_DefaultWorld->ProcessTransformCommit();
	}


	{
		SCOPE_PROFILE(Render);
		_DefaultWorld->ProcessDebugDraw(_Renderer);
		_Renderer->PerFrame();
	}
}

void SSGame::CleanupEngine()
{

	_DefaultWorld->DestroyAllObjectsInWorld();

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
}

void SSGame::StartUpContents()
{
	{
		// Floor
		SGameObject* Floor = SRendererUtil::InstantiateModel(CRAN::CUBE1M_MDL, L"Floor");
		_DefaultWorld->AddToWorld(Floor);
		Floor->SetPosition(Vector4f(0, -0.1, 0, 1));
		Floor->SetScale(Vector4f(30, 0.1, 30, 0));

		// CubeMap
		SGameObject* CubemapObject = NewSObject<SGameObject>(L"CubeMapObject");
		SCubeMapRenderComponent* CubeMapComp = CubemapObject->CreateComponent<SCubeMapRenderComponent>(L"CubemapComponent");
		CubeMapComp->SetCubeMapTextureAssetName("ContentsAssets/T_Skybox01.tex");
		SGameObjectConstructor::FinishConstructHierarchy(CubemapObject);
		_DefaultWorld->AddToWorld(CubemapObject);


		// Camera
		SGameObject* CameraObject = NewSObject<SGameObject>(L"DefaultCameraObject");
		_FreeCam = CameraObject->CreateComponent<SCameraComponent>(L"CameraComponent");
		SGameObjectConstructor::FinishConstructHierarchy(CameraObject);
		_DefaultWorld->AddToWorld(CameraObject);

		_FreeCam->SetFOVWithDegrees(60);
		_FreeCam->SetNearZ(0.01f);
		_FreeCam->SetFarZ(20.f);
		CameraObject->SetPosition(Vector4f(0, 5, -5.f, 0));

		Quaternion StartRot = Quaternion::FromLookDirect(Vector4f(0, -1, 1, 0));
		CameraObject->SetRotation(StartRot);
	}



	{
		SGameObject* LightObject = NewSObject<SGameObject>(L"GlobalLight");


		float Rad = SS::DegToRadians(50);

		Quaternion StartRot = Quaternion::FromEulerRotation(Vector4f(Rad, 0, 0, 0));
		LightObject->SetRotation(StartRot);

		SRenderLightDirectionalComponent* LightComp = LightObject->CreateComponent<SRenderLightDirectionalComponent>(L"SRenderLightDirectionalComponent");

		constexpr float SHADOWMAP_SIZE = 1024 * 8;
		LightComp->_Desc.ShadowMapSize = Vector2f(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		LightComp->_Desc.bEnableShadowMap = true;
		SGameObjectConstructor::FinishConstructHierarchy(LightObject);
		_DefaultWorld->AddToWorld(LightObject);
	}

	{
		SGameObject* CharacterModel = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Manny.mdlc", false);
		Quaternion Rot = Quaternion::FromEulerRotation({ -XM_PIDIV2, 0, 0, 0 });
		CharacterModel->SetRotation(Rot);

//		SGameObject* Charcater = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Quinn_Loco_02.mdlc", false);

		SBlendSpaceAnimTestComponent* AnimComp = CharacterModel->CreateComponent<SBlendSpaceAnimTestComponent>(L"AnimatorComp");

		SGameObject* Character = NewSObject<SGameObject>("Character");
		CharacterModel->SetParent(Character);

		SCharacterComponent* CharacterComp = Character->CreateComponent<SCharacterComponent>(L"SCharacterComponent");
		CharacterComp->BindAnimComp(AnimComp);
		SGameObjectConstructor::FinishConstructHierarchy(Character);


		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Idle.ranim", E8Dir::None);
		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_F.ranim", E8Dir::U);
		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_FR.ranim", E8Dir::UR);
		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_R.ranim", E8Dir::R);
		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_BR.ranim", E8Dir::DR);
		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_B.ranim", E8Dir::D);
		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_BL.ranim", E8Dir::DL);
		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_L.ranim", E8Dir::L);
		AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_FL.ranim", E8Dir::UL);
		AnimComp->SetPauseAnim(false);



		_DefaultWorld->AddToWorld(Character);


		// =====================================================================


		// GameManager
		SGameObject* GameManager = NewSObject<SGameObject>(L"GameManager");
		_MainPalyerController = GameManager->CreateComponent<SPlayerController>(L"PlayerController");
		_MainPalyerController->BindCharacter(Character);


		SGameObjectConstructor::FinishConstructHierarchy(GameManager);
		_DefaultWorld->AddToWorld(GameManager);

	}


	{
		SGameObject* CharacterModel = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Quinn_Loco_1.mdlc", true);
		CharacterModel->SetPosition({ 2, 0, 0, 0 });
		_DefaultWorld->AddToWorld(CharacterModel);

		CharacterModel = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Manny.mdlc", true);
		Quaternion Rot = Quaternion::FromEulerRotation({ -XM_PIDIV2, 0, 0, 0 });
		CharacterModel->SetRotation(Rot);
		CharacterModel->SetPosition({ -2, 0, 0, 0 });
		_DefaultWorld->AddToWorld(CharacterModel);
	}


	{
		_bIsFreeCamMode = false;
		_Renderer->SetMainRenderCamera(_MainPalyerController->GetCameraComp()->GetRenderCamera());

		IWindow* MainWindow = g_MainWindowManager->GetMainWindow();
		MainWindow->SetForceMouseCenter(true);
		MainWindow->SetVisibleMouse(false);
	}
}

void SSGame::PerFrameContents()
{
	if (SSInput::GetKeyDown(EKeyCode::KEY_1))
	{
		_bIsFreeCamMode = true;
		_Renderer->SetMainRenderCamera(_FreeCam->GetRenderCamera());

		IWindow* MainWindow = g_MainWindowManager->GetMainWindow();
		MainWindow->SetForceMouseCenter(false);
		MainWindow->SetVisibleMouse(true);
	}
	else if (SSInput::GetKeyDown(EKeyCode::KEY_2))
	{
		_bIsFreeCamMode = false;
		_Renderer->SetMainRenderCamera(_MainPalyerController->GetCameraComp()->GetRenderCamera());

		IWindow* MainWindow = g_MainWindowManager->GetMainWindow();
		MainWindow->SetForceMouseCenter(true);
		MainWindow->SetVisibleMouse(false);
	}

	if (_bIsFreeCamMode)
	{
		MoveFreeCamera();
	}
}

void SSGame::MoveFreeCamera()
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
