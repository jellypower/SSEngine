#include "pch.h"
#include "SSGame.h"

#include "ModuleEntryScriptRunner.h"
#include "Character/SCharacterComponent.h"
#include "PlayerController/SPlayerController.h"

#include "SSEngineDefault/Public/CommonTypes/DirEnums.h"
#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

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
		Floor->SetScale(Vector4f(10, 0.1, 10, 0));

		// CubeMap
		SGameObject* CubemapObject = NewSObject<SGameObject>(L"CubeMapObject");
		SCubeMapRenderComponent* CubeMapComp = CubemapObject->CreateComponent<SCubeMapRenderComponent>(L"CubemapComponent");
		CubeMapComp->SetCubeMapTextureAssetName("ContentsAssets/T_Skybox01.tex");
		SGameObjectConstructor::FinishConstructHierarchy(CubemapObject);
		_DefaultWorld->AddToWorld(CubemapObject);


		// Camera
		SGameObject* CameraObject = NewSObject<SGameObject>(L"DefaultCameraObject");
		SCameraComponent* CameraComp = CameraObject->CreateComponent<SCameraComponent>(L"CameraComponent");
		SGameObjectConstructor::FinishConstructHierarchy(CameraObject);
		_DefaultWorld->AddToWorld(CameraObject);

		CameraComp->SetFOVWithDegrees(60);
		CameraComp->SetNearZ(0.01f);
		CameraComp->SetFarZ(20.f);
		CameraObject->SetPosition(Vector4f(0, 5, -5.f, 0));

		Quaternion StartRot = Quaternion::FromLookDirect(Vector4f(0, -1, 1, 0));
		CameraObject->SetRotation(StartRot);
		_Renderer->SetMainRenderCamera(CameraComp->GetRenderCamera());
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

//		TEMP_MdlcObj = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Quinn_Loco_02.mdlc");
		SGameObject* Charcater = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Manny.mdlc");
		SBlendSpaceAnimTestComponent* AnimComp = Charcater->CreateComponent<SBlendSpaceAnimTestComponent>(L"AnimatorComp");
		Charcater->CreateComponent<SCharacterComponent>(L"SCharacterComponent");

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

		Quaternion Rot = Quaternion::FromEulerRotation({ -90, 0, 0, 0 });
		Charcater->SetRotation(Rot);

		_DefaultWorld->AddToWorld(Charcater);


		// =====================================================================


		// GameManager
		SGameObject* GameManager = NewSObject<SGameObject>(L"GameManager");
		SPlayerController* PlayerController = GameManager->CreateComponent<SPlayerController>(L"PlayerController");

		PlayerController->BindCharacter(Charcater);


		SGameObjectConstructor::FinishConstructHierarchy(GameManager);
		_DefaultWorld->AddToWorld(GameManager);
	}
}

void SSGame::PerFrameContents()
{

}
