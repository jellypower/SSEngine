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

	{
		// Floor
		SGameObject* Floor = SRendererUtil::InstantiateModel(CRAN::CUBE1M_MDL, L"Floor");
		_DefaultWorld->AddToWorld(Floor);
		Floor->SetPosition(Vector4f(0, -3, 0, 1));
		Floor->SetScale(Vector4f(10, 0.1, 10, 0));

		/*
		for (int32 i = 0; i < 9; i++)
		{
			float PosX = -4 + i;


			TEMP_MdlcObj = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Manny.mdlc");
			TEMP_MdlcObj->SetRotation(Quaternion::FromEulerRotation({ -XM_PIDIV2, 0, 0, 0 }));
			TEMP_MdlcObj->SetPosition({PosX, 0, 0, 1});
			//		TEMP_MdlcObj = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Quinn_Loco_1.mdlc");
			SBlendSpaceAnimTestComponent* AnimComp = TEMP_MdlcObj->CreateComponent<SBlendSpaceAnimTestComponent>(L"AnimatorComp");

			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Idle.ranim", E8Dir::None);
			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_F.ranim", E8Dir::U);
			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_FR.ranim", E8Dir::UR);
			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_R.ranim", E8Dir::R);
			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_BR.ranim", E8Dir::DR);
			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_B.ranim", E8Dir::D);
			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_BL.ranim", E8Dir::DL);
			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_L.ranim", E8Dir::L);
			AnimComp->SetRenderAnimAsset(L"ContentsAssets/SKM_Quinn_Loco_02/root|Run_FL.ranim", E8Dir::UL);


			_DefaultWorld->AddToWorld(TEMP_MdlcObj);
		}
		*/

	}


	{
		SGameObject* CubemapObject = NewSObject<SGameObject>(L"CubeMapObject");
		SCubeMapRenderComponent* CubeMapComp = CubemapObject->CreateComponent<SCubeMapRenderComponent>(L"CubemapComponent");
		CubeMapComp->SetCubeMapTextureAssetName("ContentsAssets/T_Skybox01.tex");
		SGameObjectConstructor::FinishConstructHierarchy(CubemapObject);
		_DefaultWorld->AddToWorld(CubemapObject);
	}


	Vector4f TEMP_Offset = {0, 0, 0 ,1};

	{
		SGameObject* CameraObject = NewSObject<SGameObject>(L"DefaultCameraObject");
		SCameraComponent* CameraComp = CameraObject->CreateComponent<SCameraComponent>(L"CameraComponent");
		SGameObjectConstructor::FinishConstructHierarchy(CameraObject);
		_DefaultWorld->AddToWorld(CameraObject);


		CameraComp->SetFOVWithDegrees(60);
		CameraComp->SetNearZ(0.01f);
		CameraComp->SetFarZ(20.f);
		CameraObject->SetPosition(Vector4f(0, 0, -10.f, 0) + TEMP_Offset);

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


	{
		SGameObject* Sphere01 = NewSObject<SGameObject>(L"Coll1");
		TEMP_Coll1 = Sphere01->CreateComponent<SSphereColliderComponent>(L"SSphereColliderComponent");
		SStaticMeshRenderComponent* SphereMesh = Sphere01->CreateComponent<SStaticMeshRenderComponent>("BoxMesh1");
		SphereMesh->SetMeshAsset(CRAN::SPHERE1M_MESH);
		Sphere01->SetPosition(Vector4f(0, 1, 0, 1) + TEMP_Offset);
		SGameObjectConstructor::FinishConstructHierarchy(Sphere01);
		dynamic_cast<SSphereColliderComponent*>(TEMP_Coll1)->SetRadius(0.5);
		_DefaultWorld->AddToWorld(Sphere01);


		SGameObject* Box01 = NewSObject<SGameObject>(L"Coll2");
		TEMP_Coll1 = Box01->CreateComponent<SBoxColliderComponent>(L"SBoxColliderComponent");
		SStaticMeshRenderComponent* SM1 = Box01->CreateComponent<SStaticMeshRenderComponent>("BoxMesh1");
		SM1->SetMeshAsset(CRAN::CUBE1M_MESH);
		Box01->SetPosition(Vector4f( - 2, 2, 0, 1 ) + TEMP_Offset);
		SGameObjectConstructor::FinishConstructHierarchy(Box01);
		dynamic_cast<SBoxColliderComponent*>(TEMP_Coll1)->SetExtent({ 0.5, 0.5, 0.5,0 });
		_DefaultWorld->AddToWorld(Box01);

		SGameObject* Box02 = NewSObject<SGameObject>(L"Coll3");
		TEMP_Coll2 = Box02->CreateComponent<SBoxColliderComponent>(L"SBoxColliderComponent");
		SStaticMeshRenderComponent* SM2 = Box02->CreateComponent<SStaticMeshRenderComponent>("BoxMesh2");
		SM2->SetMeshAsset(CRAN::CUBE1M_MESH);
		Box02->SetPosition(Vector4f( 2,2,0,1 ) + TEMP_Offset);
		SGameObjectConstructor::FinishConstructHierarchy(Box02);
		dynamic_cast<SBoxColliderComponent*>(TEMP_Coll2)->SetExtent({ 0.5, 0.5, 0.5,0 });
		_DefaultWorld->AddToWorld(Box02);
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
		ProcessImGUI();
	}


	{
		SCOPE_PROFILE(Contents);
		TEMP_ProcessContents();
		_DefaultWorld->PerFrameContents();
	}

	{
		SCOPE_PROFILE(SimulateCollision);
		{
			SCOPE_PROFILE(TransformCommit_Pre_Physics);
			g_FrameInfoProcessor->SetFramePhase(EFramePhase::Collision);
			_DefaultWorld->ProcessTransformCommit();
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
		}
		_DefaultWorld->ProcessDebugDraw(_Renderer);
		_Renderer->ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(&Run_g_ImGuiInitializer_OnEndFrameImGui);
		_Renderer->PerFrame();
	}

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


	// Collision Test
	{

		Vector4f a = TEMP_Coll1->GetGameObject()->GetTransform().Position;
		Vector4f b = TEMP_Coll2->GetGameObject()->GetTransform().Position;

		Vector4f ab = b - a;

		Vector4f FurthestA = TEMP_Coll1->CalcFurthest(ab);
		Vector4f FurthestB = TEMP_Coll2->CalcFurthest(-ab);



		bool bColl = false;
		{
			SCOPE_PROFILE(TEMP_CheckColl);

			for (int32 i=0;i<1;i++)
			{
				bColl = g_CollDevice->AreColliding(
					TEMP_Coll1->GetCollInstance(),
					TEMP_Coll2->GetCollInstance());
			}
		}


//		AABBBox Box = CollMath_Inline::UnionAABB(
//			TEMP_Coll1->GetCollInstance()->GetBBox(),
//			TEMP_Coll2->GetCollInstance()->GetBBox());
//		SRenderDebugUtil::DrawBoundBox(
//			_DefaultWorld,
//			Box,
//			true,
//			{ 1,0,0,1 });






		// 드로우
		IMeshAsset* Sphere = _Renderer->GetCommonRenderAssetSet()->GetSphere1mMesh();
		IMeshAsset* Arrow = _Renderer->GetCommonRenderAssetSet()->GetArrowMesh();

		Transform temp;
		temp.Scale = { 0.1, 0.1, 0.1, 0 };


		if (bColl)
		{
			temp.Position = a;
			SRenderDebugUtil::DrawDebugMesh(
				_DefaultWorld,
				temp,
				Sphere,
				false,
				{ 1,0,0,1 });

			temp.Position = b;
			SRenderDebugUtil::DrawDebugMesh(
				_DefaultWorld,
				temp,
				Sphere,
				false,
				{ 1,0,0,1 });
		}

		temp.Position = Vector4f::Zero;
		SRenderDebugUtil::DrawDebugMesh(
			_DefaultWorld,
			temp,
			Sphere,
			true,
			{ 0,1,0,1 });
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