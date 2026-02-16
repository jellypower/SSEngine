#include "pch.h"

#include "SSEditor.h"

#include "ImGUI_AssetViewer.h"
#include "ImGUI_WorldManager.h"
#include "ModuleEntryScriptRunner.h"
#include "SSImGUIInitializer.h"

#include "EngineUtils/PWin32/OpenFilePathDialogue.h"

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"


#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightDirectionalComponent.h"

#include "SSContentsBase/Public/AnimComponents/SSimpleAnimatorTestComponent.h"


#include "SSEngineDefault/Public/RawInput/KeyCodeEnums.h"


#include "SSEngineDefault/Public/RawInput/SSInput.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SSContainer/SSString/StringUtilityFunctions.h"

#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"
#include "SSEngineDefault/Public/SystemUtilities.h"




#include "SSFBXImporter/Public/FRAN.h"
#include "SSFBXImporter/Public/ISSFBXImporter.h"

#include "SSAssetDBManager/Public/IAssetDBLoader.h"


#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAssetSerializer/IApakFileReader.h"
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "TestCodes/MeshSerializeTest.h"


SSEditor* g_Editor = nullptr;

SSEditor::SSEditor(IRenderer* EngineRenderer) :
	_hashMap_TMP(200)
{
	_IEDataPool.Reserve(1024 * 10);

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
		_FbxImporter = g_fpCreateSSFBXImporter();
		_FbxImporter->BindAssetManagerToImportAsset(_Renderer->GetMutableAssetManager(), _Renderer->GetCommonRenderAssetSet());
	}


	int64 PC1 = GetPerofrmanceCounter();
	{
		_AssetDBLoader = g_fpCreateAssetDBLoader();
		_AssetDBLoader->BindAssetManagerToImportAsset(_Renderer->GetMutableAssetManager(), _Renderer->GetCommonRenderAssetSet());

		_AssetDBLoader->StartLoadDB(CRAN::NS_DEFAULT_ASSET);
		_AssetDBLoader->LoadAllAssetDataFromDB();
		_AssetDBLoader->CreateLoadedAssetInstances();
		_AssetDBLoader->RelocateCreatedAssetInstancesToAssetManager();
		_AssetDBLoader->ClearDB();

		_AssetDBLoader->StartLoadDB(L"ContentsAssets");
		_AssetDBLoader->LoadAllAssetDataFromDB();
		_AssetDBLoader->CreateLoadedAssetInstances();
		_AssetDBLoader->RelocateCreatedAssetInstancesToAssetManager();
		_AssetDBLoader->ClearLoadedAssetData();
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

	{
		_ImGUI_WorldManager = DBG_NEW ImGUI_WorldManager(_DefaultWorld);
	}

	// Floor
	{
		SGameObject* Floor = SRendererUtil::InstantiateModel(L"__RUNTIME_CREATION__/Cube1m.mdl");
		_DefaultWorld->AddToWorld(Floor);
		Floor->SetPosition(Vector4f(0, -0.1, 0, 1));
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
	Run_g_ImGuiInitializer__OnBeginFrameImGui();

	ProcessImGUI();

	TEMP_ProcessContents();

	_DefaultWorld->PerFrameContents();
	_DefaultWorld->PerFrameAnim();

	_DefaultWorld->ProcessTransformCommit();

	_DefaultWorld->ProcessDebugDraw(_Renderer);

	_Renderer->ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(&Run_g_ImGuiInitializer_OnEndFrameImGui);
	_Renderer->PerFrame();
}

void SSEditor::CleanupEngine()
{
	delete _ImGUI_WorldManager;
	_ImGUI_WorldManager = nullptr;

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

void SSEditor::ProcessEditorCommand()
{
	if (SSInput::GetKey(EKeyCode::KEY_Ctrl))
	{
		if (SSInput::GetKeyDown(EKeyCode::KEY_S))
		{
			SS::PooledList<IAssetBase*> AssetListToSerialize;
			IAssetManager* AM = _Renderer->GetAssetManager();
			AM->FindAssetsOfNamespace(AssetListToSerialize, FRAN::NS_FBX_IMPORT, EAssetType::Mesh);

			if (AssetListToSerialize.GetSize() == 0)
			{
				return;
			}

			// TEMP
			int32 ArrowIdx = -1;
			for (int32 i = 0; i < AssetListToSerialize.GetSize(); i++)
			{
				if (AssetListToSerialize[i]->GetAssetName()== L"Arrow/Arrow.mesh")
				{
					ArrowIdx = i;
					break;
				}
			}

			AssetListToSerialize.RemoveAtAndFillLast(ArrowIdx);
			// ~TEMP

			_IEDataPool.Clear();
			AppendApakDataFromAssetList(_IEDataPool, AssetListToSerialize);

			SS::StringW OutString;
			HRESULT hr = OpenSystemPathDialogue(OutString, SPD_CREATEPATH);
			if (FAILED(hr))
			{
				SS_ASSERT(false);
				return;
			}

			FILE* hFile = nullptr;

			bool bResult = ConvertToWorkingDirPath(OutString);
			if (bResult == false)
			{
				SS_ASSERT(false);
				return;
			}

			SS::SHasherW SaveAssetWorkingDirPath = OutString.C_Str();
			for (IAssetBase* SerializedAssets : AssetListToSerialize)
			{
				SerializedAssets->SetAssetPathXXX(SaveAssetWorkingDirPath);
			}

			// TODO:
			// 1. 여기서 ExtractWorkDirRelativePath라는 함수 만들어서 상대경로 빼오기
			// 2. Asset에 직접적으로 Path를 Assign하는 간단한 기능 만들어서 Assign하기
			// 3. DBLoader에서 Assign한 Path를 기준으로 Namespace기준 상대 Path 만들기


			errno_t no = _wfopen_s(&hFile, OutString.C_Str(), L"wb+");
			if (no != 0)
			{
				fclose(hFile);
				SS_ASSERT(false);
				return;
			}

			fwrite(_IEDataPool.GetData(), 1, _IEDataPool.GetSize(), hFile);
			fclose(hFile);


			// TEMP
			AM->FindAssetsOfNamespace(AssetListToSerialize, FRAN::NS_FBX_IMPORT, EAssetType::Model);
			ArrowIdx = -1;
			for (int32 i = 0; i < AssetListToSerialize.GetSize(); i++)
			{
				if (AssetListToSerialize[i]->GetAssetName() == L"Arrow/Arrow.mdl")
				{
					ArrowIdx = i;
					break;
				}
			}

			AssetListToSerialize.RemoveAtAndFillLast(ArrowIdx);
			// ~TEMP

			_AssetDBLoader->PushAssetsToSaveToDB(AssetListToSerialize);
			_AssetDBLoader->LoadAssetListFromAssetsToSaveToDB();
			_AssetDBLoader->ClearAssetsToSaveToDB();
			_AssetDBLoader->SaveLoadedAssetsToDB();
		}

		if (SSInput::GetKeyDown(EKeyCode::KEY_L))
		{
			SS::StringW OutString;
			HRESULT hr = OpenSystemPathDialogue(OutString);
			if (FAILED(hr))
			{
				SS_ASSERT(false);
				return;
			}

			IApakFileReader* Accessor = CreateApakFileAccessor(OutString.C_Str());
			if (Accessor != nullptr)
			{
				delete Accessor;
			}
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


	_ImGUI_AssetViewer->ImGUI_ShowAssetViewer();
	_ImGUI_WorldManager->PerFrame();

	ProcessEditorCommand();
	ImGUI_FrameInfo();
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