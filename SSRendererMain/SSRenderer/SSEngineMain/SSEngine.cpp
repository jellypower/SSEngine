#include "SSEngine.h"

#include "ModuleEntryScriptRunner.h"
#include "SSEngineDefault/Public/RawInput/KeyCodeEnums.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"
#include "SSEngineDefault/Public/RawInput/RawInputUtils.h"

#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/RawInput/SSInput.h"


#include "SSFBXImporter/Public/ISSFBXImporter.h"
#include "SSFBXImporter/Public/ModuleEntry/SSFBXImporterFactory.h"
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


SSEngine* g_Engine = nullptr;

SSEngine::SSEngine(IRenderer* EngineRenderer) :
	_hashMap_TMP(200)
{
	_Renderer = EngineRenderer;
}

SSEngine::~SSEngine()
{
}

void SSEngine::StartupEngine()
{
	_Renderer->StartUp();

	SS::SHasherW HAsher = "ASDFASDF";

	_FbxImporter = g_fpCreateSSFBXImporter();
	_FbxImporter->BindAssetManagerToImportAsset(_Renderer->GetMutableAssetManager());
	_FbxImporter->BindFbxSceneFile(_importFileName_TMP.C_Str());
	_FbxImporter->ImportCurrentFileToAssetManager();


	// TEMP
	TEMP_CreateTEMPMaterial();


	IRenderWorld* NewRenderWorld = _Renderer->CreateRenderWorld();

	_DefaultWorld = NewSObject<SWorld>(L"World");
	_DefaultWorld->InitializeWorld(NewRenderWorld);

	{
		
		SS::StringW BoundFileName = _FbxImporter->GetBoundFileName().C_Str();
		BoundFileName += ".mdlc";

		TEMP_MdlcObj = SRendererUtil::InstantiateModelObjTree(BoundFileName.C_Str());
//		TEMP_MdlcObj = SRendererUtil::InstantiateModel(L"room.fbx/Plane.009.mdlc");
		_DefaultWorld->AddToWorld(TEMP_MdlcObj);

//		SGameObject* Parent = TEMP_MdlcObj->GetChild(0);
//		SGameObject* Child = Parent->GetChild(0);
//		TEMP_MdlcObj = Child;
//		Parent->SetScale(Vector4f(2, 1, 1, 0));

		int a = 0;
	}

	{
		SGameObject* CameraObject = NewSObject<SGameObject>(L"DefaultCameraObject");
		SCameraComponent* CameraComp = CameraObject->CreateComponent<SCameraComponent>(L"CameraComponent");
		SGameObjectConstructor::FinishConstructHierarchy(CameraObject);
		_DefaultWorld->AddToWorld(CameraObject);

		CameraComp->SetFOVWithDegrees(90);
		CameraComp->SetNearZ(0.01f);
		CameraComp->SetFarZ(10000.f);
		CameraObject->SetPosition(Vector4f(0,0,-10.f,0));
		CameraObject->SetRotation(Quaternion::FromLookDirect(Vector4f(0, 0, 1, 0)));
		TEMP_Camera = CameraComp;
		_Renderer->SetRenderCamera(CameraComp->GetRenderCamera());
	}
}

void SSEngine::EnginePerFrame()
{
	TEMP_ProcessInput();
	_DefaultWorld->ProcessTransformCommit();
	_Renderer->PerFrame();
}

void SSEngine::CleanupEngine()
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

	_Renderer->CleanUp();
	delete _Renderer;
	_Renderer = nullptr;
}

void SSEngine::TEMP_CreateTEMPMaterial()
{
	IAssetManagerMutable* AssetManager = _Renderer->GetMutableAssetManager();

	
	IMaterialAsset* NewMaterialAsset = AssetManager->CreateEmptyMaterialAsset(L"TEMP_Material", L"/TEMP_Material");
	NewMaterialAsset->_PSName = L"TempVertexShader";

	AssetManager->AddToAssetPool(NewMaterialAsset);
}

void SSEngine::TEMP_ProcessInput()
{
	float DeltaTime = SSFrameInfo::GetDeltaTime();
	SGameObject* CamGameObj = TEMP_Camera->GetParent();
	Vector4f Forward = CamGameObj->GetTransform().GetForward();
	Vector4f Right = CamGameObj->GetTransform().GetRight();
	Vector4f Up = CamGameObj->GetTransform().GetUp();

	float WheelDelta = SSInput::GetMouseWheelDelta();
	if (WheelDelta > 0.01 || WheelDelta < -0.01)
	{
		TEMP_Speed += (WheelDelta * 0.005);

		if (TEMP_Speed < 0.2)
		{
			TEMP_Speed = 0.2;
		}
		if (TEMP_Speed  > 10.f)
		{
			TEMP_Speed = 10.f;
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

	constexpr float OBJ_ROT_SPEED = 1;
	if (SSInput::GetKey(EKeyCode::KEY_LEFT))
	{
		TEMP_MdlcYRot -= OBJ_ROT_SPEED * SSFrameInfo::GetDeltaTime();
		TEMP_MdlcYRot = fmod(TEMP_MdlcYRot, XM_PI);
		TEMP_MdlcObj->SetRotation(Quaternion::FromEulerRotation(Vector4f(TEMP_MdlcXRot, TEMP_MdlcYRot, 0, 0)));

//		Vector4f MoveDelta = Vector4f::Right * SSFrameInfo::GetDeltaTime() * 10;
//		TEMP_MdlcObj->SetPosition(TEMP_MdlcObj->GetTransform().Position + MoveDelta);
	}

	if (SSInput::GetKey(EKeyCode::KEY_RIGHT))
	{
		TEMP_MdlcYRot += OBJ_ROT_SPEED * SSFrameInfo::GetDeltaTime();
		TEMP_MdlcYRot = fmod(TEMP_MdlcYRot, XM_PI);
		TEMP_MdlcObj->SetRotation(Quaternion::FromEulerRotation(Vector4f(TEMP_MdlcXRot, TEMP_MdlcYRot, 0, 0)));

//		Vector4f MoveDelta = Vector4f::Right * SSFrameInfo::GetDeltaTime() * -10;
//		TEMP_MdlcObj->SetPosition(TEMP_MdlcObj->GetTransform().Position + MoveDelta);
	}

	if (SSInput::GetKey(EKeyCode::KEY_UP))
	{
		TEMP_MdlcXRot -= OBJ_ROT_SPEED * SSFrameInfo::GetDeltaTime();
		TEMP_MdlcXRot = fmod(TEMP_MdlcXRot, XM_PI);
		TEMP_MdlcObj->SetRotation(Quaternion::FromEulerRotation(Vector4f(TEMP_MdlcXRot, TEMP_MdlcYRot, 0, 0)));
	}

	if (SSInput::GetKey(EKeyCode::KEY_DOWN))
	{
		TEMP_MdlcXRot += OBJ_ROT_SPEED * SSFrameInfo::GetDeltaTime();
		TEMP_MdlcXRot = fmod(TEMP_MdlcXRot, XM_PI);
		TEMP_MdlcObj->SetRotation(Quaternion::FromEulerRotation(Vector4f(TEMP_MdlcXRot, TEMP_MdlcYRot, 0, 0)));
	}
}

