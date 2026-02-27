#include "pch.h"

#include "SSEditor.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

#include "ImGUI_AssetManager.h"
#include "ImGUI_Profiler.h"
#include "ImGUI_WorldManager.h"
#include "ModuleEntryScriptRunner.h"
#include "SSImGUIInitializer.h"

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"


#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightDirectionalComponent.h"


#include "SSEngineDefault/Public/RawInput/KeyCodeEnums.h"


#include "SSEngineDefault/Public/RawInput/SSInput.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"

#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"
#include "SSEngineDefault/Public/RawProfiler/ScopedProfile.h"
#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"



#include "SSFBXImporter/Public/ISSFBXImporter.h"

#include "SSAssetDBManager/Public/IAssetDBLoader.h"


#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "TestCodes/MeshSerializeTest.h"


SSEditor* g_Editor = nullptr;

SSEditor::SSEditor(IRenderer* EngineRenderer) :
	_hashMap_TMP(200)
{

	_Renderer = EngineRenderer;

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
		_AssetDBLoader->CreateAssetInstancesFromInter();
		_AssetDBLoader->RelocateCreatedAssets(AssetListToImport);
		_AssetDBLoader->ClearDB();

		_AssetDBLoader->StartLoadDB(L"ContentsAssets");
		_AssetDBLoader->LoadAllAssetDataFromDB();
		_AssetDBLoader->CreateAssetInstancesFromInter();
		_AssetDBLoader->RelocateCreatedAssets(AssetListToImport);
		_AssetDBLoader->ClearDB();

		_FbxImporter = g_fpCreateSSFBXImporter();
		_FbxImporter->BindFbxSceneFile(_importFileName_TMP.C_Str());
		_FbxImporter->GenerateImportedAssets();
		_FbxImporter->RelocateCreatedAssets(AssetListToImport);


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
		const SS::HashMap<SS::SHasherW, IAssetBase*>& MeshAssetMap =
			_Renderer->GetMutableAssetManager()->GetAssetMap(EAssetType::Mesh);

		for (const SS::pair<SS::SHasherW, IAssetBase*>& MeshAssetItemPair : MeshAssetMap)
		{
			MeshSerializeTest(_Renderer, MeshAssetItemPair.first);
		}

		const SS::HashMap<SS::SHasherW, IAssetBase*>& MdlcAssetMap =
			_Renderer->GetMutableAssetManager()->GetAssetMap(EAssetType::ModelCombination);

		for (const SS::pair<SS::SHasherW, IAssetBase*>& MdlcAssetItemPair : MdlcAssetMap)
		{
			MdlcSerializeTest(_Renderer, MdlcAssetItemPair.first);
		}

		const SS::HashMap<SS::SHasherW, IAssetBase*>& RenderAnimMap =
			_Renderer->GetMutableAssetManager()->GetAssetMap(EAssetType::RenderAnim);

		for (const SS::pair<SS::SHasherW, IAssetBase*>& RenderAnimAssetItemPair : RenderAnimMap)
		{
			RenderAnimSerializeTest(_Renderer, RenderAnimAssetItemPair.first);
		}

	}
	// ~DEBUG
	

	IRenderWorld* NewRenderWorld = _Renderer->CreateRenderWorld();

	_DefaultWorld = NewSObject<SWorld>(L"World");
	_DefaultWorld->InitializeWorld(NewRenderWorld);

	{
		_ImGUI_AssetViewer = DBG_NEW ImGUI_AssetManager(_Renderer);
		_ImGUI_WorldManager = DBG_NEW ImGUI_WorldManager(_DefaultWorld);
		_ImGUI_Profiler = DBG_NEW ImGUI_Profiler();
	}

	{
		// Floor
		SGameObject* Floor = SRendererUtil::InstantiateModel(CRAN::CUBE1M_MDL, L"Floor");
		_DefaultWorld->AddToWorld(Floor);
		Floor->SetPosition(Vector4f(0, -0.1, 0, 1));
		Floor->SetScale(Vector4f(10, 0.1, 10, 0));


		/*

		static const SS::SHasherW ArrowMeshName = _Renderer->GetCommonRenderAssetSet()->GetArrowMesh()->GetAssetName();

		// X
		SGameObject* DirectionObject = SRendererUtil::InstantiateMesh(ArrowMeshName, L"Arrow-X");
		_DefaultWorld->AddToWorld(DirectionObject);
		DirectionObject->SetRotation(Quaternion::CalcPitchYawRotationFromDir(Vector4f(1, 0, 0, 0)));
		DirectionObject->SetPosition(Vector4f(0, 0.2f, 0, 1));

		// Y
		DirectionObject = SRendererUtil::InstantiateMesh(ArrowMeshName, L"Arrow-Y");
		_DefaultWorld->AddToWorld(DirectionObject);
		DirectionObject->SetRotation(Quaternion::CalcPitchYawRotationFromDir(Vector4f(0, 1, 0, 0)));
		DirectionObject->SetPosition(Vector4f(0, 0.2f, 0, 1));

		// Z
		DirectionObject = SRendererUtil::InstantiateMesh(ArrowMeshName, L"Arrow-Z");
		_DefaultWorld->AddToWorld(DirectionObject);
		DirectionObject->SetRotation(Quaternion::CalcPitchYawRotationFromDir(Vector4f(0, 0, 1, 0)));
		DirectionObject->SetPosition(Vector4f(0, 0.2f, 0, 1));

		TEMP_MdlcObj = SRendererUtil::InstantiateModelObjTree(L"ContentsAssets/SKM_Vivian.mdlc");
		SSimpleAnimatorTestComponent* AnimComp = TEMP_MdlcObj->CreateComponent<SSimpleAnimatorTestComponent>(L"AnimatorComp");
		_DefaultWorld->AddToWorld(TEMP_MdlcObj);



		SS::SHasherW BoundAsset = _FbxImporter->GetRepresentingAssetName();

		TEMP_MdlcObj = SRendererUtil::InstantiateModelObjTree(BoundAsset.C_Str());
		AnimComp = TEMP_MdlcObj->CreateComponent<SSimpleAnimatorTestComponent>(L"AnimatorComp");
		_DefaultWorld->AddToWorld(TEMP_MdlcObj);

		*/
	}


	{
		SGameObject* CubemapObject = NewSObject<SGameObject>(L"CubeMapObject");
		SCubeMapRenderComponent* CubeMapComp = CubemapObject->CreateComponent<SCubeMapRenderComponent>(L"CubemapComponent");
		CubeMapComp->SetCubeMapTextureAssetName("ContentsAssets/T_Skybox01.tex");
		SGameObjectConstructor::FinishConstructHierarchy(CubemapObject);
		_DefaultWorld->AddToWorld(CubemapObject);
	}


	{
		SGameObject* CameraObject = NewSObject<SGameObject>(L"DefaultCameraObject");
		SCameraComponent* CameraComp = CameraObject->CreateComponent<SCameraComponent>(L"CameraComponent");
		SGameObjectConstructor::FinishConstructHierarchy(CameraObject);
		_DefaultWorld->AddToWorld(CameraObject);


		CameraComp->SetFOVWithDegrees(60);
		CameraComp->SetNearZ(0.01f);
		CameraComp->SetFarZ(20.f);
		CameraObject->SetPosition(Vector4f(0, 0, -10.f, 0));

		Quaternion StartRot = Quaternion::FromLookDirect(Vector4f(0, 0.25, 1, 0));
		CameraObject->SetRotation(StartRot);
		TEMP_Camera = CameraComp;

		Vector4f RotEuler = XMEulerFromQuaternion(StartRot.SimdVec);
		TEMP_CamXRot = RotEuler.X;
		TEMP_CamYRot = RotEuler.Y;

		_Renderer->SetMainRenderCamera(CameraComp->GetRenderCamera());
	}


	{
		SGameObject* LightObject = NewSObject<SGameObject>(L"GlobalLight");
		SRenderLightDirectionalComponent* LightComp = LightObject->CreateComponent<SRenderLightDirectionalComponent>(L"SRenderLightDirectionalComponent");
		LightComp->_Desc.ShadowMapSize = Vector2f(4096.f, 4096.f);
		LightComp->_Desc.bEnableShadowMap = true;
		SGameObjectConstructor::FinishConstructHierarchy(LightObject);
		_DefaultWorld->AddToWorld(LightObject);

		TEMP_Light = LightComp;
	}
}

void SSEditor::EnginePerFrame()
{
	SCOPE_PROFILE(Engine);

	{
		SCOPE_PROFILE(BeginImGUI);
		Run_g_ImGuiInitializer__OnBeginFrameImGui();
	}

	{
		SCOPE_PROFILE(Editor);
		ProcessImGUI();
	}


	{
		SCOPE_PROFILE(Contents);
		TEMP_ProcessContents();
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
		_Renderer->ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(&Run_g_ImGuiInitializer_OnEndFrameImGui);
		_Renderer->PerFrame();
	}

	int a = 0;
}

void SSEditor::CleanupEngine()
{
	{
		delete _ImGUI_Profiler;
		_ImGUI_Profiler = nullptr;

		delete _ImGUI_WorldManager;
		_ImGUI_WorldManager = nullptr;

		delete _ImGUI_AssetViewer;
		_ImGUI_AssetViewer = nullptr;
	}

	_DefaultWorld->DestroyAllObjectsInWorld();

	bool IsAnyObjectReminInWorld = _DefaultWorld->IsAnyObjectRemainInWorld();
	SS_ASSERT(IsAnyObjectReminInWorld == false);

	DelSObject(_DefaultWorld);
	_DefaultWorld = nullptr;


	_FbxImporter->ClearFbxSceneFile();
	delete _FbxImporter;
	_FbxImporter = nullptr;

	_AssetDBLoader->ClearDB();
	delete _AssetDBLoader;
	_AssetDBLoader = nullptr;

	_Renderer->GetCommonRenderAssetSet()->ReleaseCachedAssets();

	g_ImGuiInitializer->CleanUpImGui();
	delete g_ImGuiInitializer;
	g_ImGuiInitializer = nullptr;

	_Renderer->CleanUp();
	delete _Renderer;
	_Renderer = nullptr;
}

void SSEditor::TEMP_ProcessContents()
{
	float DeltaTime = SSFrameInfo::GetDeltaTime();
	SGameObject* CamGameObj = TEMP_Camera->GetGameObject();
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

	Quaternion::FromEulerRotation(Vector4f(45, 45, 90, 0));


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