#include "SSEditor.h"

#include "ImGUI_GameObjectDetailViewer.h"
#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "ModuleEntryScriptRunner.h"
#include "SSImGUIInitializer.h"
#include "SSEngineDefault/Public/RawInput/KeyCodeEnums.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightDirectionalComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SStaticMeshRenderComponent.h"

#include "SSContentsBase/Public/AnimComponents/SSimpleAnimatorTestComponent.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/RawInput/SSInput.h"
#include "SSEngineDefault/Public/SSContainer/SSString/StringUtilityFunctions.h"


#include "SSFBXImporter/Public/ISSFBXImporter.h"


#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"




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
	_ImGUI_SelectedAssetManager_Type = EAssetType::Texture;


	_Renderer->GetCommonRenderAssetSet()->InitializeCommonAssets();
	TEMP_CreateAssets(); // CommonAssetSet을 초기화


	{
		_FbxImporter = g_fpCreateSSFBXImporter();
		_FbxImporter->BindAssetManagerToImportAsset(_Renderer->GetMutableAssetManager(), _Renderer->GetCommonRenderAssetSet());
	}


	{
		_FbxImporter->BindFbxSceneFile(L"D:\\FBXAssets\\Arrow.fbx");
		_FbxImporter->ImportCurrentFileToAssetManager();
	}

	{
		_FbxImporter->BindFbxSceneFile(_importFileName_TMP.C_Str());
		_FbxImporter->ImportCurrentFileToAssetManager();
	}


	IRenderWorld* NewRenderWorld = _Renderer->CreateRenderWorld();

	_DefaultWorld = NewSObject<SWorld>(L"World");
	_DefaultWorld->InitializeWorld(NewRenderWorld);

	{
		SGameObject* Floor = SRendererUtil::InstantiateModel(L"Cube1m.mdl");
		_DefaultWorld->AddToWorld(Floor);
		Floor->SetPosition(Vector4f(0, -0.1,0, 1));
		Floor->SetScale(Vector4f(10, 0.1, 10, 0));
	}

	{
		SGameObject* Floor = SRendererUtil::InstantiateModel(L"Sphere1m.mdl");
		_DefaultWorld->AddToWorld(Floor);
		Floor->SetPosition(Vector4f(0, 1, 0, 1));
		Floor->SetScale(Vector4f(1, 1, 1, 0));
	}

	{
		SGameObject* DirectionObject = SRendererUtil::InstantiateModel(L"Arrow/Arrow.mdl");
		_DefaultWorld->AddToWorld(DirectionObject);

		DirectionObject->SetRotation(Quaternion::FromLookDirect(Vector4f(1, 0, 1, 0)));
	}

	{
		
		SS::StringW BoundFileName = _FbxImporter->GetBoundFileName().C_Str();
		BoundFileName += ".mdlc";

		TEMP_MdlcObj = SRendererUtil::InstantiateModelObjTree(BoundFileName.C_Str());
		SSimpleAnimatorTestComponent* AnimComp = TEMP_MdlcObj->CreateComponent<SSimpleAnimatorTestComponent>(L"AnimatorComp");
		_DefaultWorld->AddToWorld(TEMP_MdlcObj);


//		SGameObject* LowerBody = TEMP_MdlcObj->FindChildOfName(L"下半身", true);
//		SStaticMeshRenderComponent* SM = LowerBody->CreateComponent<SStaticMeshRenderComponent>(L"StaticMesh");
//		SM->SetModelAsset(_Renderer->GetCommonRenderAssetSet()->GetCube1mModel()->GetAssetName());
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

	_Renderer->ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(&Run_g_ImGuiInitializer_OnEndFrameImGui);
	_Renderer->PerFrame();
}

void SSEditor::CleanupEngine()
{

	_DefaultWorld->DestroyAllObjectsInWorld();

	bool IsAnyObjectReminInWorld = _DefaultWorld->IsAnyObjectRemainInWorld();
	SS_ASSERT(IsAnyObjectReminInWorld == false);

	DelSObject(_DefaultWorld);
	_DefaultWorld = nullptr;


	_FbxImporter->ClearFbxSceneFile();
	_FbxImporter->ClearRendererToImportAsset();
	delete _FbxImporter;
	_FbxImporter = nullptr;

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
			
			ITextureAssetMutable* NewTex = AssetManager->CreateEmptyTextureAsset(NameCStr, PathCStr, TexType);
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

	if (SSInput::GetMouse(EMouseCode::MOUSE_RIGHT))
	{
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
	}



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


	if (SSInput::GetMouse(EMouseCode::MOUSE_RIGHT))
	{	
		constexpr float CAM_ROT_SPEED = 2;
		constexpr float CAM_XROT_MAX = 0.9;

		TEMP_CamYRot += SSInput::GetMouseDelta().X * CAM_ROT_SPEED;

		if (TEMP_CamXRot > -XM_PIDIV2 * CAM_XROT_MAX && SSInput::GetMouseDelta().Y > 0)
			TEMP_CamXRot -= SSInput::GetMouseDelta().Y * CAM_ROT_SPEED;

		if (TEMP_CamXRot < XM_PIDIV2 * CAM_XROT_MAX && SSInput::GetMouseDelta().Y < 0)
			TEMP_CamXRot -= SSInput::GetMouseDelta().Y * CAM_ROT_SPEED;

		CamGameObj->SetRotation(Quaternion::FromEulerRotation(Vector4f(TEMP_CamXRot, TEMP_CamYRot, 0, 0)));
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

	ImGUI_AssetManagerWindow();
	ImGUI_FrameInfo();
	ImGUI_ShowGameObjectDetail(_PickedObject);
	ImGUI_DrawHierarchy();
}

void SSEditor::ImGUI_AssetManagerWindow()
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& TextureList = AssetManager->GetAssetMap(EAssetType::Texture);
	const SS::HashMap<SS::SHasherW, IAssetBase*>& MeshList = AssetManager->GetAssetMap(EAssetType::Mesh);
	const SS::HashMap<SS::SHasherW, IAssetBase*>& MtlList = AssetManager->GetAssetMap(EAssetType::Material);
	const SS::HashMap<SS::SHasherW, IAssetBase*>& ModelList = AssetManager->GetAssetMap(EAssetType::Model);

	ImGui::Begin("Asset Editor");
	{
		ImGui::BeginTabBar("AssetManager_Tabbar");

		if (ImGui::TabItemButton("Texture"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Texture;
		}
		else if (ImGui::TabItemButton("Mesh"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Mesh;
		}
		else if (ImGui::TabItemButton("Material"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Material;
		}
		else if (ImGui::TabItemButton("Model"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::Model;
		}
		else if (ImGui::TabItemButton("RenderAnim"))
		{
			_ImGUI_SelectedAssetManager_Type = EAssetType::RenderAnim;
		}
		ImGui::EndTabBar();

		switch (_ImGUI_SelectedAssetManager_Type)
		{
		case EAssetType::Texture: ImGUI_AssetManagerWindow_Texture(); break;
		case EAssetType::Mesh: ImGUI_AssetManager_Mesh(); break;
		case EAssetType::Material: ImGUI_AssetManager_Material(); break;
		case EAssetType::Model: ImGUI_AssetManager_Model(); break;
		case EAssetType::RenderAnim: ImGUI_AssetManager_RenderAnim(); break;
		default:
			SS_ASSERT(false);
			break;
		}
	}
	ImGui::End();
}

void SSEditor::ImGUI_AssetManagerWindow_Texture()
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& TextureList = AssetManager->GetAssetMap(EAssetType::Texture);

	if (ImGui::BeginTable("Textures", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
	{
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Texture Name");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Texture Path");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ref Cnt");

		for (const SS::pair<SS::SHasherW, IAssetBase*>& TexturePairItem : TextureList)
		{
			IAssetBase* TextureItem = TexturePairItem.second;
			ImGui::TableNextColumn();

			uint32 AssetStrLen = 0;
			const utf16* AssetCstr = nullptr;

			{
				constexpr int32 BUFFER_SIZE = 256;
				utf8 Converter[BUFFER_SIZE];
				AssetCstr = TextureItem->GetAssetName().C_Str(&AssetStrLen);
				UTF16StrToUtf8Str(AssetCstr, AssetStrLen, Converter, BUFFER_SIZE);

				ImGui::Text(Converter);
			}

			{
				ImGui::TableNextColumn();

				constexpr int32 BUFFER_SIZE = 256;
				utf8 Converter[BUFFER_SIZE];
				AssetCstr = TextureItem->GetAssetPath().C_Str(&AssetStrLen);
				UTF16StrToUtf8Str(AssetCstr, AssetStrLen, Converter, BUFFER_SIZE);

				ImGui::Text(Converter);
			}

			{
				ImGui::TableNextColumn();

				constexpr int32 BUFFER_SIZE = 256;
				utf8 StrBuffer[BUFFER_SIZE];

				int32 RefCnt = TextureItem->GetAssetInstanceReferenceCnt();
				_itoa(RefCnt, StrBuffer, 10);
				ImGui::Text(StrBuffer);
			}
		}

		ImGui::EndTable();
	}
}
void SSEditor::ImGUI_AssetManager_Mesh()
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& MeshList = AssetManager->GetAssetMap(EAssetType::Mesh);

	if (ImGui::BeginTable("Meshes", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
	{
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Name");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Path");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ref Cnt");

		for (const SS::pair<SS::SHasherW, IAssetBase*>& MeshItemPair : MeshList)
		{
			IAssetBase* MeshItem = MeshItemPair.second;
			ImGui::TableNextColumn();

			uint32 AssetStrLen = 0;
			const utf16* AssetCstr = nullptr;

			{
				constexpr int32 BUFFER_SIZE = 256;
				utf8 Converter[BUFFER_SIZE];
				AssetCstr = MeshItem->GetAssetName().C_Str(&AssetStrLen);
				UTF16StrToUtf8Str(AssetCstr, AssetStrLen, Converter, BUFFER_SIZE);

				ImGui::Text(Converter);
			}

			{
				ImGui::TableNextColumn();

				constexpr int32 BUFFER_SIZE = 256;
				utf8 Converter[BUFFER_SIZE];
				AssetCstr = MeshItem->GetAssetPath().C_Str(&AssetStrLen);
				UTF16StrToUtf8Str(AssetCstr, AssetStrLen, Converter, BUFFER_SIZE);

				ImGui::Text(Converter);
			}

			{
				ImGui::TableNextColumn();

				constexpr int32 BUFFER_SIZE = 256;
				utf8 StrBuffer[BUFFER_SIZE];

				int32 RefCnt = MeshItem->GetAssetInstanceReferenceCnt();
				_itoa(RefCnt, StrBuffer, 10);
				ImGui::Text(StrBuffer);
			}
		}
		ImGui::EndTable();
	}
}

void SSEditor::ImGUI_AssetManager_Material()
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& TextureList = AssetManager->GetAssetMap(EAssetType::Texture);
	const SS::HashMap<SS::SHasherW, IAssetBase*>& MtlList = AssetManager->GetAssetMap(EAssetType::Material);


	for (const SS::pair<SS::SHasherW, IAssetBase*>& MaterialItemPair : MtlList)
	{
		IMaterialAssetMutable* MtlItem = (IMaterialAssetMutable*)MaterialItemPair.second;
		MtlDataBase* MtlData = MtlItem->GetMutableMtlData();
		ImGui::TableNextColumn();

		SS::SHasherW MtlName = MtlItem->GetAssetName();
		uint32 MtlNameStrLen = 0;
		const utf16* MtlNameCStr = nullptr;
		MtlNameCStr = MtlName.C_Str(&MtlNameStrLen);

		SS::SHasherW MtlPath = MtlItem->GetAssetPath();
		uint32 MtlPathStrLen = 0;
		const utf16* MtlPathCStr = nullptr;
		MtlPathCStr = MtlPath.C_Str(&MtlPathStrLen);

		constexpr int32 BUFFER_SIZE = 256;
		utf8 u8MtlName[BUFFER_SIZE];
		UTF16StrToUtf8Str(MtlNameCStr, MtlNameStrLen, u8MtlName, BUFFER_SIZE);

		utf8 u8MtlPath[BUFFER_SIZE];
		UTF16StrToUtf8Str(MtlPathCStr, MtlPathStrLen, u8MtlPath, BUFFER_SIZE);

		bool bIsMtlEdited = false;

		if (ImGui::CollapsingHeader(u8MtlName))
		{
			ImGui::PushID(u8MtlName);
			{
				// ============================== Mtl Path ==============================
				ImGui::Text("Material Path: %s", u8MtlPath);
				ImGui::Dummy(ImVec2(1, 10));

				//
				if (MtlData->_Type == EMaterialType::DefaultPBR)
				{
					MtlDataDefaultPBR* PbrMtlData = static_cast<MtlDataDefaultPBR*>(MtlData);

					// ============================== Mtl Factor ==============================
					ImGui::Text("Material Factor");
					{
						float BaseColor[4];
						float EmissiveColor[4];
						float NormalTexScale = PbrMtlData->_NormalTexScale;
						float Metallic = PbrMtlData->_Metallic;
						float Roughness = PbrMtlData->_Roughness;

						const Vector4f& v4BaseColor = PbrMtlData->_BaseColorScale;
						const Vector4f& v4EmissiveColor = PbrMtlData->_EmissiveScale;

						memcpy(BaseColor, &v4BaseColor, sizeof(Vector4f));
						memcpy(EmissiveColor, &v4EmissiveColor, sizeof(Vector4f));


						if (ImGui::ColorEdit4("BaseColorFactor", BaseColor))
						{
							memcpy_s(&(PbrMtlData->_BaseColorScale), sizeof(Vector4f),
								BaseColor, sizeof(Vector4f));

							bIsMtlEdited = true;
						}
						if (ImGui::ColorEdit4("EmissiveColorFactor", EmissiveColor))
						{
							memcpy_s(&(PbrMtlData->_EmissiveScale), sizeof(Vector4f),
								EmissiveColor, sizeof(Vector4f));

							bIsMtlEdited = true;
						}
						if (ImGui::SliderFloat("NormalTexScale", &NormalTexScale, 0.0f, 1.0f))
						{
							PbrMtlData->_NormalTexScale = NormalTexScale;
							bIsMtlEdited = true;
						}
						if (ImGui::SliderFloat("Metallic", &Metallic, 0.0f, 1.0f))
						{
							PbrMtlData->_Metallic = Metallic;
							bIsMtlEdited = true;
						}
						if (ImGui::SliderFloat("Roughness", &Roughness, 0.0f, 1.0f))
						{
							PbrMtlData->_Roughness = Roughness;
							bIsMtlEdited = true;
						}
					}

					// ============================== Mtl Textures ==============================
					if (ImGui::TreeNode("Material Textures"))
					{
						for (int32 i=0;i< (int32)EDefaultPBRMatTexTypes::Count;i++)
						{
							EDefaultPBRMatTexTypes TexType = (EDefaultPBRMatTexTypes)i;
							const char* TexTypeStr = to_string(TexType);
							ITextureAsset* TexItem = PbrMtlData->_Textures[i];
							SS::SHasherW EquippedTexName;

							constexpr int32 BUFFER_SIZE = 256;
							utf8 u8EquippedTexName[BUFFER_SIZE] = "EMPTY";
							if (TexItem != nullptr)
							{
								EquippedTexName = TexItem->GetAssetName();
								uint32 EquippedTexNameCStrLen = 0;
								const utf16* EquippedTexNameCStr = EquippedTexName.C_Str(&EquippedTexNameCStrLen);
								UTF16StrToUtf8Str(EquippedTexNameCStr, EquippedTexNameCStrLen, u8EquippedTexName, BUFFER_SIZE);
							}



							if (ImGui::BeginCombo(TexTypeStr, u8EquippedTexName, ImGuiComboFlags_WidthFitPreview))
							{
								for (const SS::pair<SS::SHasherW, IAssetBase*>& ItemPair : TextureList)
								{
									ITextureAsset* SelectTexItem = (ITextureAsset*)ItemPair.second;
									SS::SHasherW SelectTexItemName = SelectTexItem->GetAssetName();
									uint32 SelectTexItemCStrLen = 0;
									const utf16* SelectTexItemCStr = SelectTexItemName.C_Str(&SelectTexItemCStrLen);
									utf8 u8SelectTexItemName[BUFFER_SIZE];
									UTF16StrToUtf8Str(SelectTexItemCStr, SelectTexItemCStrLen, u8SelectTexItemName, BUFFER_SIZE);

									bool bIsSelected = false;
									if (EquippedTexName == SelectTexItemName)
									{
										bIsSelected = true;
									}

									if (ImGui::Selectable(u8SelectTexItemName, bIsSelected))
									{
										PbrMtlData->_Textures[i] = SelectTexItem;
										bIsMtlEdited = true;
									}

									if (bIsSelected)
									{
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}
						}
						ImGui::TreePop();
					}
				}
				else
				{
					SS_ASSERT(false);
				}
			}
			ImGui::PopID();

			if (bIsMtlEdited)
			{
				MtlItem->NotifyMtlDataModified();
			}
		}
	}
}

void SSEditor::ImGUI_AssetManager_Model()
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& MeshList = AssetManager->GetAssetMap(EAssetType::Mesh);
	const SS::HashMap<SS::SHasherW, IAssetBase*>& MtlList = AssetManager->GetAssetMap(EAssetType::Material);
	const SS::HashMap<SS::SHasherW, IAssetBase*>& ModelList = AssetManager->GetAssetMap(EAssetType::Model);

	for (const SS::pair<SS::SHasherW, IAssetBase*>& ModelItemPair : ModelList)
	{
		IModelAssetMutable* ModelItem = (IModelAssetMutable*)ModelItemPair.second;
		uint32 ModelNameStrLen = 0;
		const utf16* ModelNameCStr = ModelItem->GetAssetName().C_Str(&ModelNameStrLen);

		uint32 ModelPathStrLen = 0;
		const utf16* ModelPathCStr = ModelItem->GetAssetPath().C_Str(&ModelPathStrLen);

		constexpr int32 BUFFER_SIZE = 256;
		utf8 u8ModelName[BUFFER_SIZE];
		UTF16StrToUtf8Str(ModelNameCStr, ModelNameStrLen, u8ModelName, BUFFER_SIZE);

		utf8 u8ModelPath[BUFFER_SIZE];
		UTF16StrToUtf8Str(ModelPathCStr, ModelPathStrLen, u8ModelPath, BUFFER_SIZE);

		if (ImGui::CollapsingHeader(u8ModelName))
		{
			ImGui::PushID(u8ModelName);
			{
				// Model Path
				ImGui::Text("Model Path: %s", u8ModelPath);
				ImGui::Dummy(ImVec2(1, 10));

				// Mesh Editing
				ImGui::Text("Mesh");
				{
					IMeshAsset* SelectedMesh = ModelItem->GetMeshAsset();
					SS::SHasherW SelectedMeshName = SelectedMesh->GetAssetName();

					uint32 MeshAssetNameStrLen = 0;
					const utf16* u16SelectedMeshAssetName = SelectedMeshName.C_Str(&MeshAssetNameStrLen);

					utf8 u8MeshName[BUFFER_SIZE];
					UTF16StrToUtf8Str(u16SelectedMeshAssetName, MeshAssetNameStrLen, u8MeshName, BUFFER_SIZE);
					if (ImGui::BeginCombo("Mesh", u8MeshName, ImGuiComboFlags_WidthFitPreview))
					{
						for (const SS::pair<SS::SHasherW, IAssetBase*>& MeshItemInListPair : MeshList)
						{
							IMeshAsset* MeshItemInList = (IMeshAsset*)MeshItemInListPair.second;
							SS::SHasherW MeshItemInListName = MeshItemInList->GetAssetName();

							uint32 MeshItemInListNameStrLen = 0;
							const utf16* u16MeshItemInListName = MeshItemInListName.C_Str(&MeshItemInListNameStrLen);

							UTF16StrToUtf8Str(u16MeshItemInListName, MeshItemInListNameStrLen, u8MeshName, BUFFER_SIZE);

							bool bIsSelectedItem = SelectedMeshName == MeshItemInListName;
							bool bSelectNewItem = ImGui::Selectable(u8MeshName, bIsSelectedItem);

							if (bSelectNewItem)
							{
								ModelItem->SetMesh(MeshItemInList);
							}

							if (bIsSelectedItem)
							{
								ImGui::SetItemDefaultFocus();
							}
						}

						ImGui::EndCombo();
					}
				}


				ImGui::Dummy(ImVec2(1, 10));
				ImGui::Text("Material");

				// Material Editing
				for (int MtlIdx = 0; MtlIdx < ModelItem->GetSubMeshCnt(); MtlIdx++)
				{
					uint32 MtlAssetNameStrLen = 0;
					const utf16* MtlAssetName = nullptr;

					IMaterialAsset* SelectedMaterial = ModelItem->GetMaterialAsset(MtlIdx);
					SS::SHasherW SelectedMtlName;
					if (SelectedMaterial != nullptr)
					{
						SelectedMtlName = SelectedMaterial->GetAssetName();

						MtlAssetNameStrLen = 0;
						MtlAssetName = SelectedMtlName.C_Str(&MtlAssetNameStrLen);
					}
					else
					{
						MtlAssetName = L"EMPTY";
						MtlAssetNameStrLen = wcslen(MtlAssetName);
					}

					utf8 u8MtlName[BUFFER_SIZE];
					UTF16StrToUtf8Str(MtlAssetName, MtlAssetNameStrLen, u8MtlName, BUFFER_SIZE);

					char MtlHeader[50] = "Material_";
					_itoa(MtlIdx, MtlHeader + 9, 10);


					if (ImGui::BeginCombo(MtlHeader, u8MtlName, ImGuiComboFlags_WidthFitPreview))
					{
						for (const SS::pair<SS::SHasherW, IAssetBase*>& MtlItemPair : MtlList)
						{
							IAssetBase* MaterialItemInList = MtlItemPair.second;
							SS::SHasherW MtlItemInListName = MaterialItemInList->GetAssetName();

							uint32 MtlStrLen = 0;
							const utf16* u16MtlStr = MtlItemInListName.C_Str(&MtlStrLen);

							UTF16StrToUtf8Str(u16MtlStr, MtlStrLen, u8MtlName, BUFFER_SIZE);

							bool bIsSelectedItem = SelectedMtlName == MtlItemInListName;
							bool bSelectNewItem = ImGui::Selectable(u8MtlName, bIsSelectedItem);

							if (bSelectNewItem)
							{
								if (EAssetType::Material == MaterialItemInList->GetAssetType())
								{
									ModelItem->SetMaterial((IMaterialAsset*)MaterialItemInList, MtlIdx);
								}
								else
								{
									SS_ASSERT(false);
								}
							}

							if (bIsSelectedItem)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
				}
			}
			ImGui::PopID();
		}

	}

}

void SSEditor::ImGUI_AssetManager_RenderAnim()
{
	IAssetManager* AssetManager = _Renderer->GetAssetManager();
	const SS::HashMap<SS::SHasherW, IAssetBase*>& RenderAnimList = AssetManager->GetAssetMap(EAssetType::RenderAnim);

	if (ImGui::BeginTable("Anims", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
	{
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Name");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Path");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ref Cnt");

		for (const SS::pair<SS::SHasherW, IAssetBase*>& MeshItemPair : RenderAnimList)
		{
			IAssetBase* AnimItem = MeshItemPair.second;
			ImGui::TableNextColumn();

			uint32 AssetStrLen = 0;
			const utf16* AssetCstr = nullptr;

			{
				constexpr int32 BUFFER_SIZE = 256;
				utf8 Converter[BUFFER_SIZE];
				AssetCstr = AnimItem->GetAssetName().C_Str(&AssetStrLen);
				UTF16StrToUtf8Str(AssetCstr, AssetStrLen, Converter, BUFFER_SIZE);

				ImGui::Text(Converter);
			}

			{
				ImGui::TableNextColumn();

				constexpr int32 BUFFER_SIZE = 256;
				utf8 Converter[BUFFER_SIZE];
				AssetCstr = AnimItem->GetAssetPath().C_Str(&AssetStrLen);
				UTF16StrToUtf8Str(AssetCstr, AssetStrLen, Converter, BUFFER_SIZE);

				ImGui::Text(Converter);
			}

			{
				ImGui::TableNextColumn();

				constexpr int32 BUFFER_SIZE = 256;
				utf8 StrBuffer[BUFFER_SIZE];

				int32 RefCnt = AnimItem->GetAssetInstanceReferenceCnt();
				_itoa(RefCnt, StrBuffer, 10);
				ImGui::Text(StrBuffer);
			}
		}
		ImGui::EndTable();
	}

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
	uint32 iObjectNameLen = 0;
	const utf16* u16ObjectName = sObjectName.C_Str(&iObjectNameLen);

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
