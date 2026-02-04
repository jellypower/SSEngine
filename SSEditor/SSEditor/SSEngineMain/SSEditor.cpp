#include "SSEditor.h"

#include "ImGUI_AssetViewer.h"
#include "ImGUI_GameObjectDetailViewer.h"
#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "ModuleEntryScriptRunner.h"
#include "SSImGUIInitializer.h"
#include "SSEngineDefault/Public/RawInput/KeyCodeEnums.h"


#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightDirectionalComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SStaticMeshRenderComponent.h"

#include "SSContentsBase/Public/AnimComponents/SSimpleAnimatorTestComponent.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/RawInput/SSInput.h"
#include "SSEngineDefault/Public/SSContainer/SSString/StringUtilityFunctions.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"
#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"



#include "SSFBXImporter/Public/ISSFBXImporter.h"

#include "SSAssetDBManager/Public/IAssetDBLoader.h"


#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"

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





	TEMP_CreateAssets(); // CommonAssetSet을 초기화


	{
		_FbxImporter = g_fpCreateSSFBXImporter();
		_FbxImporter->BindAssetManagerToImportAsset(_Renderer->GetMutableAssetManager(), _Renderer->GetCommonRenderAssetSet());
	}


	int64 PC1 = GetPerofrmanceCounter();
	{
		_AssetDBLoader = g_fpCreateAssetDBLoader();
		_AssetDBLoader->BindAssetManagerToImportAsset(_Renderer->GetMutableAssetManager(), _Renderer->GetCommonRenderAssetSet());
		_AssetDBLoader->StartLoadDB(L"Resource/AssetDB/EngineDefaultAssets.sqlite");

		_AssetDBLoader->GenerateImportedAssets();
		_AssetDBLoader->RelocateImportedAssetsToAssetManager();
	}
	int64 PC2 = GetPerofrmanceCounter();
	int64 PF = GetPerformanceFrequency();
	double eTime = (PC2 - PC1) / (double)PF;
	int a = 0;

	{
		_Renderer->GetCommonRenderAssetSet()->InitializeCommonAssets();
	}


	{
		_FbxImporter->BindFbxSceneFile(L"D:\\FBXAssets\\Arrow.fbx");
		_FbxImporter->GenerateImportedAssets();
		_FbxImporter->RelocateImportedAssetsToAssetManager();
	}


	{
		_FbxImporter->BindFbxSceneFile(_importFileName_TMP.C_Str());
		_FbxImporter->GenerateImportedAssets();
		_FbxImporter->RelocateImportedAssetsToAssetManager();
	}



	// DEBUG
	{
		const SS::HashMap<SS::SHasherW, IAssetBase*>& MeshAssetMap = 
			_Renderer->GetMutableAssetManager()->GetAssetMap(EAssetType::Mesh);

		for (const SS::pair<SS::SHasherW, IAssetBase*>& MeshAssetItemPair : MeshAssetMap)
		{
			MeshSerializeTest(_Renderer, MeshAssetItemPair.first);
		}
	}
	// ~DEBUG


	{
		_ImGUI_AssetViewer = DBG_NEW ImGUI_AssetViewer(_Renderer);
	}


	_Renderer->GetCommonRenderAssetSet()->TEMP_CacheCommonAssetFromFBX();


	IRenderWorld* NewRenderWorld = _Renderer->CreateRenderWorld();

	_DefaultWorld = NewSObject<SWorld>(L"World");
	_DefaultWorld->InitializeWorld(NewRenderWorld);

	// Floor
	{
		SGameObject* Floor = SRendererUtil::InstantiateModel(L"__RUNTIME_CREATION__/Cube1m.mdl");
		_DefaultWorld->AddToWorld(Floor);
		Floor->SetPosition(Vector4f(0, -0.1,0, 1));
		Floor->SetScale(Vector4f(10, 0.1, 10, 0));
	}

	// Arrow
	{
		// X
		SGameObject* DirectionObject = SRendererUtil::InstantiateModel(L"Arrow/Arrow.mdl", L"Arrow-X");
		_DefaultWorld->AddToWorld(DirectionObject);
		DirectionObject->SetRotation(Quaternion::CalcPitchYawRotationFromDir(Vector4f(1, 0, 0, 0)));
		DirectionObject->SetPosition(Vector4f(0, 0.2f, 0, 1));

		// Y
		DirectionObject = SRendererUtil::InstantiateModel(L"Arrow/Arrow.mdl", L"Arrow-Y");
		_DefaultWorld->AddToWorld(DirectionObject);
		DirectionObject->SetRotation(Quaternion::CalcPitchYawRotationFromDir(Vector4f(0, 1, 0, 0)));
		DirectionObject->SetPosition(Vector4f(0, 0.2f, 0, 1));

		// Z
		DirectionObject = SRendererUtil::InstantiateModel(L"Arrow/Arrow.mdl", L"Arrow-Z");
		_DefaultWorld->AddToWorld(DirectionObject);
		DirectionObject->SetRotation(Quaternion::CalcPitchYawRotationFromDir(Vector4f(0, 0, 1, 0)));
		DirectionObject->SetPosition(Vector4f(0, 0.2f, 0, 1));
	}

	{
		
		SS::StringW BoundFileName = _FbxImporter->GetBoundFileName().C_Str();
		BoundFileName += ".mdlc";

		TEMP_MdlcObj = SRendererUtil::InstantiateModelObjTree(BoundFileName.C_Str());
		SSimpleAnimatorTestComponent* AnimComp = TEMP_MdlcObj->CreateComponent<SSimpleAnimatorTestComponent>(L"AnimatorComp");
		_DefaultWorld->AddToWorld(TEMP_MdlcObj);


	}

	{
		SGameObject* CubemapObject = NewSObject<SGameObject>(L"CubeMapObject");
		SCubeMapRenderComponent* CubeMapComp = CubemapObject->CreateComponent<SCubeMapRenderComponent>(L"CubemapComponent");
		CubeMapComp->SetCubeMapTextureAssetName("T_Skybox01.tex");
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
		CameraObject->SetPosition(Vector4f(0,0,-10.f,0));

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
	TEMP_ProcessContents();

	Run_g_ImGuiInitializer__OnBeginFrameImGui();
	ProcessImGUI();

	_DefaultWorld->PerFrameContents();
	_DefaultWorld->PerFrameAnim();

	_DefaultWorld->ProcessTransformCommit();

	_DefaultWorld->ProcessDebugDraw(_Renderer);

	_Renderer->ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(&Run_g_ImGuiInitializer_OnEndFrameImGui);
	_Renderer->PerFrame();
}

void SSEditor::CleanupEngine()
{
	delete _ImGUI_AssetViewer;
	_ImGUI_AssetViewer = nullptr;


	_DefaultWorld->DestroyAllObjectsInWorld();

	bool IsAnyObjectReminInWorld = _DefaultWorld->IsAnyObjectRemainInWorld();
	SS_ASSERT(IsAnyObjectReminInWorld == false);

	DelSObject(_DefaultWorld);
	_DefaultWorld = nullptr;


	_FbxImporter->ClearFbxSceneFile();
	_FbxImporter->ClearRendererToImportAsset();
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

void SSEditor::TEMP_CreateAssets()
{
	IAssetManagerMutable* AssetManager = _Renderer->GetMutableAssetManager();
	

	// Texture List 구성하기
	{
		struct STextureAssetList
		{
			const utf16* TextureName;
			const utf16* TexturePath;
			ETextureType Type;
		};

		const STextureAssetList TextureAssetList[]
			= {
				{L"rp_nathan_animated_003_dif.tex", L"Resource/Texture/rp_nathan_animated_003_dif.dds", ETextureType::Texture2D},

				{L"Worm_SSS_Color.tex", L"Resource/Texture/Worm_SSS_Color.dds", ETextureType::Texture2D},
				{L"Worm_reflection.tex", L"Resource/Texture/Worm_reflection.dds", ETextureType::Texture2D},
				{L"Worm_Bump.tex", L"Resource/Texture/Worm_Bump.dds", ETextureType::Texture2D},

				{L"Teeth_SSS_Color.tex", L"Resource/Texture/Teeth_SSS_Color.dds", ETextureType::Texture2D},
				{L"Teeth_reflection.tex", L"Resource/Texture/Teeth_reflection.dds", ETextureType::Texture2D},
				{L"Teeth_Bump.tex", L"Resource/Texture/Teeth_Bump.dds", ETextureType::Texture2D},

				{L"T_Manny_02_D.tex", L"Resource/Texture/T_Manny_02_D.DDS", ETextureType::Texture2D},
				{L"T_Manny_01_D.tex", L"Resource/Texture/T_Manny_01_D.DDS", ETextureType::Texture2D},
				{L"T_Manny_02_N.tex", L"Resource/Texture/T_Manny_02_N.DDS", ETextureType::Texture2D},
				{L"T_Manny_01_N.tex", L"Resource/Texture/T_Manny_01_N.DDS", ETextureType::Texture2D},
				{L"T_Manny_02_MSR_MSK.tex", L"Resource/Texture/T_Manny_02_MSR_MSK.DDS", ETextureType::Texture2D},
				{L"T_Manny_01_MSR_MSK.tex", L"Resource/Texture/T_Manny_01_MSR_MSK.DDS", ETextureType::Texture2D},

				{L"T_Skybox01.tex", L"Resource/Texture/T_Skybox01.dds", ETextureType::CubeMap},
				{L"T_Skybox02.tex", L"Resource/Texture/T_Skybox02.dds", ETextureType::CubeMap},
				{L"T_Skybox03.tex", L"Resource/Texture/T_Skybox03.dds", ETextureType::CubeMap},

				{L"T_Vivian_Body_D.tex", L"Resource/Texture/T_Vivian_Body_D.dds", ETextureType::Texture2D},
				{L"T_Vivian_Crystal_D.tex", L"Resource/Texture/T_Vivian_Crystal_D.dds", ETextureType::Texture2D},
				{L"T_Vivian_spa_h.tex", L"Resource/Texture/T_Vivian_spa_h.dds", ETextureType::Texture2D},
				{L"T_Vivian_Weapon_D.tex", L"Resource/Texture/T_Vivian_Weapon_D.dds", ETextureType::Texture2D},
				{L"T_Vivian_Weapon_Metallic.tex", L"Resource/Texture/T_Vivian_Weapon_Metallic.dds", ETextureType::Texture2D},
				{L"T_VivianHair_D.tex", L"Resource/Texture/T_VivianHair_D.dds", ETextureType::Texture2D},
				{L"T_Vivian_Face_D.tex", L"Resource/Texture/T_Vivian_Face_D.dds", ETextureType::Texture2D},
		};

		for (int32 i=0;i<_countof(TextureAssetList);i++)
		{
			const utf16* NameCStr = TextureAssetList[i].TextureName;
			const utf16* PathCStr = TextureAssetList[i].TexturePath;
			ETextureType TexType = TextureAssetList[i].Type;
			
			ITextureAssetMutable* NewTex = AssetManager->CreateEmptyTextureAsset("__TEMP__", NameCStr, PathCStr, TexType);
			AssetManager->AddToAssetPool(NewTex);
		}
	}
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


	// Pixel Picking
	{
		if (SSInput::GetMouseDown(EMouseCode::MOUSE_LEFT))
		{
			Vector2i32 MousePos = SSInput::GetMousePos();
			_Renderer->RequestPixelPicking(MousePos.X, MousePos.Y);
			_PixelPickingRequestFrameCounter = SWAP_CHAIN_FRAME_COUNT + 1; // PixelPicking용 프레임버퍼가 2프레임 뒤에 그려져서 그걸 생각해야함.
		}

		if (_PixelPickingRequestFrameCounter >= 0)
		{
			_PixelPickingRequestFrameCounter--;
		}

		if (_PixelPickingRequestFrameCounter == 0)
		{
			SObjHashCode PixelPickedObjID = _Renderer->GetPixelPickedObjectID();
			_PickedObject = PixelPickedObjID;
		}

		if (_HieararchyPickedObject != nullptr && _PickedObject != _HieararchyPickedObject)
		{
			_PickedObject = _HieararchyPickedObject;
			_HieararchyPickedObject = nullptr;
		}
	}

	Quaternion::FromEulerRotation(Vector4f(45, 45, 90, 0));


	SObjectBase* PickedObject = _PickedObject.GetSObject();
	SGameObject* PickedGameObject = nullptr;
	if (SComponentBase* PickedComponent = dynamic_cast<SComponentBase*>(PickedObject))
	{
		PickedGameObject = PickedComponent->GetGameObject();
	}
	else if (SGameObject* CastedPickedGameObject = dynamic_cast<SGameObject*>(PickedObject))
	{
		PickedGameObject = CastedPickedGameObject;
	}

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


	_ImGUI_AssetViewer->ImGUI_ShowAssetViewer();
	ImGUI_FrameInfo();
	ImGUI_ShowGameObjectDetail(_PickedObject);
	ImGUI_DrawHierarchy();
}



void SSEditor::ImGUI_FrameInfo()
{
	ImGui::Begin("Frame Info");
	{
		ImGui::Text("Elapsed time: %f", SSFrameInfo::GetElapsedTime());
		ImGui::Text("Delta time: %f", SSFrameInfo::GetDeltaTime());
		ImGui::Text("FPS: %f", SSFrameInfo::GetFPS());
	}
	ImGui::End();
}

void SSEditor::ImGUI_DrawHierarchy()
{
	if (ImGui::Begin("Node Debugger")) 
	{

		if (ImGui::BeginChild("SceneTree", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) 
		{
			SGameObject* RootObject = _DefaultWorld->GetWorldRootObject();

			int32 ChildCnt = RootObject->GetChildCnt();

			for (int i=0;i<ChildCnt;i++)
			{
				ImGUI_DrawHierarchy_Recursion(RootObject->GetChild(i));
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void SSEditor::ImGUI_DrawHierarchy_Recursion(SGameObject* Object)
{
	int32 ChildCnt = Object->GetChildCnt();

	SS::SHasherW sObjectName = Object->GetObjectName();
	uint32 iObjectNameLen = sObjectName.GetStrLen();
	const utf16* u16ObjectName = sObjectName.C_Str();

	utf8 u8ObjectName[SHASHER_STRLEN_MAX];
	UTF16StrToUtf8Str(u16ObjectName, iObjectNameLen, u8ObjectName, SHASHER_STRLEN_MAX);


	bool bColorNode = _HieararchyPickedObject == Object->GetHashCode();

	if (bColorNode)
	{
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	if (ImGui::TreeNodeEx(u8ObjectName, 
		ImGuiTreeNodeFlags_SpanLabelWidth |
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_Selected | 
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::IsItemClicked(0))
		{
			_HieararchyPickedObject = Object;
		}

		for (int i = 0; i < ChildCnt; i++)
		{
			ImGUI_DrawHierarchy_Recursion(Object->GetChild(i));
		}
		
		ImGui::TreePop();
	}


	if (bColorNode)
	{
		ImGui::PopStyleColor(); // Pop the green text color
	}
}
