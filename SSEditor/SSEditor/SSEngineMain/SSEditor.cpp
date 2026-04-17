#include "pch.h"

#include "SSEditor.h"



#include "ImGUI_AssetManager.h"
#include "ImGUI_Profiler.h"
#include "ImGUI_WorldManager.h"
#include "ModuleEntryScriptRunner.h"
#include "SSImGUIInitializer.h"
#include "TestCodes/MeshSerializeTest.h"



#include "SSContentsBase/Public/AnimComponents/SBlendSpaceAnimTestComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SStaticMeshRenderComponent.h"
#include "SSContentsBase/Public/CollisionComp/SBoxColliderComponent.h"
#include "SSContentsBase/Public/CollisionComp/SSphereColliderComponent.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightDirectionalComponent.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"


#include "SSEngineDefault/Public/CommonTypes/DirEnums.h"
#include "SSEngineDefault/Public/RawInput/KeyCodeEnums.h"
#include "SSEngineDefault/Public/RawInput/SSInput.h"
#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"
#include "SSEngineDefault/Public/RawProfiler/ScopedProfile.h"
#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/Collision/CollMathInline.h"

#include "SSAssetDBManager/Public/IAssetDBLoader.h"

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"


#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"
#include "SSCollision/Public/CollisionBase/ICollDevice.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"


#include "SSGameModule/Public/SSGame.h"
#include "SSGameModule/Public/PlayerController/SPlayerController.h"


SSEditor* g_Editor = nullptr;

SSEditor::SSEditor(IRenderer* EngineRenderer, ICollDevice* EngineCollDevice) :
	_hashMap_TMP(200)
{

	_Renderer = EngineRenderer;
	_CollDevice = EngineCollDevice;

	if (g_ImGuiInitializer != nullptr)
	{
		SS_INTERRUPT();
	}
	g_ImGuiInitializer = DBG_NEW SSImGUIInitializer;
}

SSEditor::~SSEditor()
{
	delete g_ImGuiInitializer;
}

void SSEditor::StartupEngine()
{
	_Renderer->StartUp();
	g_ImGuiInitializer->StartupImGui(_Renderer);


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
	}

	{
		_Renderer->GetCommonRenderAssetSet()->InitializeCommonAssets();
	}

	// DEBUG
	{
		IAssetManagerMutable* AM = _Renderer->GetMutableAssetManager();

		const SS::HashMap<SS::SHasherW, IAssetBase*>& MeshAssetMap = AM->GetAssetMap(EAssetType::Mesh);
		for (const SS::pair<SS::SHasherW, IAssetBase*>& MeshAssetItemPair : MeshAssetMap)
		{
			MeshSerializeTest(_Renderer, MeshAssetItemPair.first);
		}

		const SS::HashMap<SS::SHasherW, IAssetBase*>& MdlcAssetMap = AM->GetAssetMap(EAssetType::ModelCombination);
		for (const SS::pair<SS::SHasherW, IAssetBase*>& MdlcAssetItemPair : MdlcAssetMap)
		{
			MdlcSerializeTest(_Renderer, MdlcAssetItemPair.first);
		}

		const SS::HashMap<SS::SHasherW, IAssetBase*>& RenderAnimMap = AM->GetAssetMap(EAssetType::RenderAnim);
		for (const SS::pair<SS::SHasherW, IAssetBase*>& RenderAnimAssetItemPair : RenderAnimMap)
		{
			RenderAnimSerializeTest(_Renderer, RenderAnimAssetItemPair.first);
		}

	}
	// ~DEBUG
	

	IRenderWorld* NewRenderWorld = _Renderer->CreateRenderWorld();
	ICollisionWorld* NewCollWorld = _CollDevice->CreateCollWorld("EditorCollWorld");

	_DefaultWorld = NewSObject<SWorld>(L"World");
	_DefaultWorld->InitializeWorld(NewRenderWorld, NewCollWorld);

	{
		_ImGUI_AssetViewer = DBG_NEW ImGUI_AssetManager(_Renderer);
		_ImGUI_WorldManager = DBG_NEW ImGUI_WorldManager(_DefaultWorld);
		_ImGUI_Profiler = DBG_NEW ImGUI_Profiler();
	}



	if (true)
	{
		_Game = DBG_NEW SSGame(_DefaultWorld);
		_Game->SetInGameFocus(true);
		_Game->StartUpGame();

		_Renderer->SetMainRenderCamera(
			_Game->GetMainPlayerController()->GetCameraComp()->GetRenderCamera());
		
	}
	


	{
		SGameObject* CameraObject = NewSObject<SGameObject>(L"EditorFreeCam");
		SCameraComponent* CameraComp = CameraObject->CreateComponent<SCameraComponent>(L"CameraComponent");
		SGameObjectConstructor::FinishConstructHierarchy(CameraObject);
		_DefaultWorld->AddToWorld(CameraObject);


		CameraComp->SetFOVWithDegrees(60);
		CameraComp->SetNearZ(0.01f);
		CameraComp->SetFarZ(20.f);
		CameraObject->SetPosition(Vector4f(0, 0, -10.f, 0));

		Quaternion StartRot = Quaternion::FromLookDirect(Vector4f(0, 0.25, 1, 0));
		CameraObject->SetRotation(StartRot);
		_FreeCam = CameraComp;

		Vector4f RotEuler = XMEulerFromQuaternion(StartRot.SimdVec);
		TEMP_CamXRot = RotEuler.X;
		TEMP_CamYRot = RotEuler.Y;
	}
}

void SSEditor::EnginePerFrame()
{
	SCOPE_PROFILE(Engine);
	g_FrameInfoProcessor->SetFramePhase(EFramePhase::Contents);
	{
		SCOPE_PROFILE(BeginImGUI);
		Run_g_ImGuiInitializer__OnBeginFrameImGui();
	}

	{
		SCOPE_PROFILE(Editor);

		if (_Game == nullptr || _Game->IsInGameFocus() == false)
		{
			ProcessImGUI();
		}
	}


	{
		SCOPE_PROFILE(Contents);

		if (SSInput::GetKeyDown(EKeyCode::KEY_P))
		{
			bool bWasGameFocus = _Game->IsInGameFocus();
			_Game->SetInGameFocus(!bWasGameFocus);

			if (bWasGameFocus)
			{
				Transform CharacterTransform =_Game->GetMainPlayerController()->GetCameraComp()->GetGameObject()->GetTransform();
				_FreeCam->GetGameObject()->SetTransform(CharacterTransform);
				_Renderer->SetMainRenderCamera(_FreeCam->GetRenderCamera());
			}
			else
			{
				_Renderer->SetMainRenderCamera(_Game->GetMainPlayerController()->GetCameraComp()->GetRenderCamera());
			}
		}


		if (_Game->IsInGameFocus())
		{
			_Game->PerFrameGame();
		}
		else
		{
			EditorControl();
		}

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
		_Renderer->ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(&Run_g_ImGuiInitializer_OnEndFrameImGui);
		_Renderer->PerFrame();
	}

}

void SSEditor::CleanupEngine()
{
	if (_Game != nullptr)
	{
		_Game->CleanupGame();
		delete _Game;
	}


	{
		delete _ImGUI_Profiler;
		_ImGUI_Profiler = nullptr;

		delete _ImGUI_WorldManager;
		_ImGUI_WorldManager = nullptr;

		delete _ImGUI_AssetViewer;
		_ImGUI_AssetViewer = nullptr;
	}

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
	g_ImGuiInitializer->CleanUpImGui();
	delete g_ImGuiInitializer;
	g_ImGuiInitializer = nullptr;

	_Renderer->CleanUp();
	delete _Renderer;
	_Renderer = nullptr;

	delete _CollDevice;
	_CollDevice = nullptr;
}

void SSEditor::EditorControl()
{
	float DeltaTime = SSFrameInfo::GetDeltaTime();
	SGameObject* CamGameObj = _FreeCam->GetGameObject();
	Vector4f Forward = CamGameObj->GetTransform().GetForward();
	Vector4f Right = CamGameObj->GetTransform().GetRight();
	Vector4f Up = CamGameObj->GetTransform().GetUp();

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

		CamGameObj->SetRotation(Quaternion::FromEulerRotation(Vector4f(TEMP_CamXRot, TEMP_CamYRot, 0, 0)));

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
			Vector4f Pos = CamGameObj->GetTransform().Position;
			Pos = Pos + Forward * -DeltaTime * TEMP_Speed;
			CamGameObj->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_W))
		{
			Vector4f Pos = CamGameObj->GetTransform().Position;
			Pos = Pos + Forward * DeltaTime * TEMP_Speed;
			CamGameObj->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_D))
		{
			Vector4f Pos = CamGameObj->GetTransform().Position;
			Pos = Pos + Right * DeltaTime * TEMP_Speed;
			CamGameObj->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_A))
		{
			Vector4f Pos = CamGameObj->GetTransform().Position;
			Pos = Pos + Right * -DeltaTime * TEMP_Speed;
			CamGameObj->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_E))
		{
			Vector4f Pos = CamGameObj->GetTransform().Position;
			Pos = Pos + Up * DeltaTime * TEMP_Speed;
			CamGameObj->SetPosition(Pos);
		}
		if (SSInput::GetKey(EKeyCode::KEY_Q))
		{
			Vector4f Pos = CamGameObj->GetTransform().Position;
			Pos = Pos + Up * -DeltaTime * TEMP_Speed;
			CamGameObj->SetPosition(Pos);
		}
	}


	SGameObject* PickedGameObject = _ImGUI_WorldManager->GetPickedObject();
	if (PickedGameObject != nullptr)
	{
		XMMATRIX Mat = PickedGameObject->CalcWorldTransformMatrix();

		constexpr float OBJ_ROT_SPEED = 3;
		if (SSInput::GetKey(EKeyCode::KEY_LEFT))
		{
			Quaternion CurRot = PickedGameObject->GetTransform().Rotation;
			const SGameObject* Parent = PickedGameObject->GetParent();
			Vector4f UpVector = Parent->GetTransform().GetUp();

			CurRot = Quaternion::RotateAxisAngle(CurRot, UpVector, SSFrameInfo::GetDeltaTime() * OBJ_ROT_SPEED);
			PickedGameObject->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_RIGHT))
		{
			Quaternion CurRot = PickedGameObject->GetTransform().Rotation;
			const SGameObject* Parent = PickedGameObject->GetParent();
			Vector4f UpVector = Parent->GetTransform().GetUp();

			CurRot = Quaternion::RotateAxisAngle(CurRot, UpVector, SSFrameInfo::GetDeltaTime() * -OBJ_ROT_SPEED);
			PickedGameObject->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_UP))
		{
			Quaternion CurRot = PickedGameObject->GetTransform().Rotation;
			const SGameObject* Parent = PickedGameObject->GetParent();
			Vector4f RightVector = Parent->GetTransform().GetRight();

			CurRot = Quaternion::RotateAxisAngle(CurRot, RightVector, SSFrameInfo::GetDeltaTime() * OBJ_ROT_SPEED);
			PickedGameObject->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_DOWN))
		{
			Quaternion CurRot = PickedGameObject->GetTransform().Rotation;
			const SGameObject* Parent = PickedGameObject->GetParent();
			Vector4f RightVector = Parent->GetTransform().GetRight();

			CurRot = Quaternion::RotateAxisAngle(CurRot, RightVector, SSFrameInfo::GetDeltaTime() * -OBJ_ROT_SPEED);
			PickedGameObject->SetRotation(CurRot);
		}
	}
	else
	{
		constexpr float OBJ_ROT_SPEED = 3;
		if (SSInput::GetKey(EKeyCode::KEY_LEFT))
		{
			SGameObject* LightGO = TEMP_Light->GetGameObject();
			Quaternion CurRot = LightGO->GetTransform().Rotation;
			const SGameObject* Parent = LightGO->GetParent();
			Vector4f UpVector = Parent->GetTransform().GetUp();

			CurRot = Quaternion::RotateAxisAngle(CurRot, UpVector, SSFrameInfo::GetDeltaTime() * 10);
			LightGO->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_RIGHT))
		{
			SGameObject* LightGO = TEMP_Light->GetGameObject();
			Quaternion CurRot = LightGO->GetTransform().Rotation;
			const SGameObject* Parent = LightGO->GetParent();
			Vector4f UpVector = Parent->GetTransform().GetUp();

			CurRot = Quaternion::RotateAxisAngle(CurRot, UpVector, SSFrameInfo::GetDeltaTime() * -10);
			LightGO->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_UP))
		{
			SGameObject* LightGO = TEMP_Light->GetGameObject();
			Quaternion CurRot = LightGO->GetTransform().Rotation;
			const SGameObject* Parent = LightGO->GetParent();
			Vector4f RightVector = Parent->GetTransform().GetRight();

			CurRot = Quaternion::RotateAxisAngle(CurRot, RightVector, SSFrameInfo::GetDeltaTime() * 10);
			LightGO->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_DOWN))
		{
			SGameObject* LightGO = TEMP_Light->GetGameObject();
			Quaternion CurRot = LightGO->GetTransform().Rotation;
			const SGameObject* Parent = LightGO->GetParent();
			Vector4f RightVector = Parent->GetTransform().GetRight();

			CurRot = Quaternion::RotateAxisAngle(CurRot, RightVector, SSFrameInfo::GetDeltaTime() * -10);
			LightGO->SetRotation(CurRot);
		}
	}
}

void SSEditor::ProcessImGUI()
{
	ImGuiID dockspace_id = ImGui::GetID("IMGUI_SSEDITOR_DOCKSPACE");
	ImGui::DockSpaceOverViewport(dockspace_id, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);


	_ImGUI_AssetViewer->PerFrame();
	_ImGUI_WorldManager->PerFrame();
	_ImGUI_Profiler->PerFrame();
}
