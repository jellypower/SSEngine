#include "SSEditor.h"

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


#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


SSEditor* g_Editor = nullptr;

SSEditor::SSEditor(IRenderer* EngineRenderer) :
	_hashMap_TMP(200)
{
	_Renderer = EngineRenderer;
}

SSEditor::~SSEditor()
{
}

void SSEditor::StartupEngine()
{
	_Renderer->StartUp();


	_FbxImporter = g_fpCreateSSFBXImporter();
	_FbxImporter->BindAssetManagerToImportAsset(_Renderer->GetMutableAssetManager());
	_FbxImporter->BindFbxSceneFile(_importFileName_TMP.C_Str());
	_FbxImporter->ImportCurrentFileToAssetManager();

	TEMP_CreateAssets();


	IRenderWorld* NewRenderWorld = _Renderer->CreateRenderWorld();

	_DefaultWorld = NewSObject<SWorld>(L"World");
	_DefaultWorld->InitializeWorld(NewRenderWorld);

	{
		
		SS::StringW BoundFileName = _FbxImporter->GetBoundFileName().C_Str();
		BoundFileName += ".mdlc";

		TEMP_MdlcObj = SRendererUtil::InstantiateModelObjTree(BoundFileName.C_Str());

//		TEMP_MdlcObj = SRendererUtil::InstantiateModel(L"frew worm monster.fbx/body.mdl");
//		TEMP_MdlcObj->SetScale(Vector4f(1000, 1000, 1000, 0));
//		TEMP_MdlcObj->SetRotation(Quaternion());
//		TEMP_MdlcObj->SetPosition(Vector4f::Zero);

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

	_Renderer->GetCommonRenderAssetSet()->CacheCommonRenderAssets();
	_Renderer->GetCommonRenderAssetSet()->AddRefCachedAssets();
}

void SSEditor::EnginePerFrame()
{
	TEMP_ProcessContents();
	_DefaultWorld->ProcessTransformCommit();
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

	_Renderer->CleanUp();
	delete _Renderer;
	_Renderer = nullptr;
}

void SSEditor::TEMP_CreateAssets()
{
	IAssetManagerMutable* AssetManager = _Renderer->GetMutableAssetManager();

	ITextureAssetMutable* BlackTex = AssetManager->CreateEmptyTextureAsset(L"BLACK.tex", L"Resource/Texture/BLACK.dds");
	AssetManager->AddToAssetPool(BlackTex);
	ITextureAssetMutable* EmptyTex = AssetManager->CreateEmptyTextureAsset(L"EMPTY.tex", L"Resource/Texture/EMPTY.dds");
	AssetManager->AddToAssetPool(EmptyTex);
	ITextureAssetMutable* EmptyNormalTex = AssetManager->CreateEmptyTextureAsset(L"EMPTYNORMAL.tex", L"Resource/Texture/EMPTYNORMAL.dds");
	AssetManager->AddToAssetPool(EmptyNormalTex);
	ITextureAssetMutable* WhiteTex = AssetManager->CreateEmptyTextureAsset(L"WHITE.tex", L"Resource/Texture/WHITE.dds");
	AssetManager->AddToAssetPool(WhiteTex);

	IMaterialAssetMutable* TempMtl = AssetManager->CreateEmptyMaterialAsset(L"EMPTY.mtl", "__EMPTY_PATH__");
	MtlDataDefaultPBR* EmptyDefaultPBR = DBG_NEW MtlDataDefaultPBR();
	EmptyDefaultPBR->_Type = EMaterialType::DefaultPBR;
	EmptyDefaultPBR->_BaseColorScale = Vector4f::One;
	EmptyDefaultPBR->_EmissiveScale = Vector4f::One;
	EmptyDefaultPBR->_NormalTexScale = 1;
	EmptyDefaultPBR->_Metallic = 0.5;
	EmptyDefaultPBR->_Roughness = 0.5;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::BaseColor] = EmptyTex;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::Normal] = EmptyNormalTex;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::Metallic] = WhiteTex;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::Emissive] = BlackTex;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::Occlusion] = BlackTex;
	TempMtl->InjectRawDataXXX(EmptyDefaultPBR);
	AssetManager->AddToAssetPool(TempMtl);
}

void SSEditor::TEMP_ProcessContents()
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

	if (SSInput::GetMouseDown(EMouseCode::MOUSE_LEFT))
	{
		Vector2i32 MousePos = SSInput::GetMousePos();
		_Renderer->RequestPixelPicking(MousePos.X, MousePos.Y);
	}

	SObjHashCode ObjID = _Renderer->GetPixelPickedObjectID();

	SObjectBase* PickedObj = ObjID.GetSObject();
	SGameObject* PickedGameObj = dynamic_cast<SGameObject*>(PickedObj);

	if (PickedGameObj != TEMP_PixelPickedObject)
	{
		TEMP_PixelPickedObject = PickedGameObj;
	}


	if (TEMP_PixelPickedObject != nullptr)
	{
		constexpr float OBJ_ROT_SPEED = 1;
		if (SSInput::GetKey(EKeyCode::KEY_LEFT))
		{
			Quaternion CurRot = TEMP_PixelPickedObject->GetTransform().Rotation;
			const SGameObject* Parent = TEMP_PixelPickedObject->GetParent();
			Vector4f UpVector = Parent->GetTransform().GetUp();

			CurRot = Quaternion::RotateAxisAngle(CurRot, UpVector, SSFrameInfo::GetDeltaTime() * 10);
			TEMP_PixelPickedObject->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_RIGHT))
		{
			Quaternion CurRot = TEMP_PixelPickedObject->GetTransform().Rotation;
			const SGameObject* Parent = TEMP_PixelPickedObject->GetParent();
			Vector4f UpVector = Parent->GetTransform().GetUp();

			CurRot = Quaternion::RotateAxisAngle(CurRot, UpVector, SSFrameInfo::GetDeltaTime() * -10);
			TEMP_PixelPickedObject->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_UP))
		{
			Quaternion CurRot = TEMP_PixelPickedObject->GetTransform().Rotation;
			const SGameObject* Parent = TEMP_PixelPickedObject->GetParent();
			Vector4f RightVector = Parent->GetTransform().GetRight();

			CurRot = Quaternion::RotateAxisAngle(CurRot, RightVector, SSFrameInfo::GetDeltaTime() * 10);
			TEMP_PixelPickedObject->SetRotation(CurRot);
		}

		if (SSInput::GetKey(EKeyCode::KEY_DOWN))
		{
			Quaternion CurRot = TEMP_PixelPickedObject->GetTransform().Rotation;
			const SGameObject* Parent = TEMP_PixelPickedObject->GetParent();
			Vector4f RightVector = Parent->GetTransform().GetRight();

			CurRot = Quaternion::RotateAxisAngle(CurRot, RightVector, SSFrameInfo::GetDeltaTime() * -10);
			TEMP_PixelPickedObject->SetRotation(CurRot);
		}
	}
}

