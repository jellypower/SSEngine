#include "pch.h"
#define SSGAME_MODULE_EXPORT
#include "SSGameModule/Public/SSGame.h"

#include <ctime>

#include "SSCollision/Public/CollisionBase/ICollDevice.h"

#include "SSGameModule/Public/PlayerController/SPlayerController.h"
#include "SSGameModule/Public/PlayerController/SCameraController.h"

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
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"


#include "SSContentsBase/Public/AnimComponents/SBlendSpaceAnimTestComponent.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightDirectionalComponent.h"
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SCharacterMovementComponent.h"
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SRigidBodyDynamicComponent.h"
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SRigidBodyStaticComponent.h"
#include "SSContentsBase/Public/CollisionComp/SBoxColliderComponent.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"


#include "SSAssetDBManager/Public/IAssetDBLoader.h"

#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"



SSGame* g_Game;


SSGame::SSGame(SWorld* InDefaultWorld)
{
	SS_ASSERT(g_Game == nullptr);
	g_Game = this;
	_DefaultWorld = InDefaultWorld;
}

SSGame::~SSGame()
{
}

void SSGame::StartUpGame()
{
	{
		// Floor
		SGameObject* Floor = SRendererUtil::InstantiateModel(CRAN::CUBE1M_MDL, L"Floor", false);
		Floor->SetPosition(Vector4f(0, -0.1, 0, 1));
		Floor->SetScale(Vector4f(30, 0.1, 30, 0));

		SBoxColliderComponent* BoxCollider = Floor->CreateComponent<SBoxColliderComponent>(L"SBoxColliderComponent");
		BoxCollider->SetExtent({ 0.5f, 0.5f, 0.5f, 0 });
		Floor->CreateComponent<SRigidBodyStaticComponent>(L"SRigidBodyDynamicComponent");

		SGameObjectConstructor::FinishConstructHierarchy(Floor);
		_DefaultWorld->AddToWorld(Floor);


		// CubeMap
		SGameObject* CubemapObject = NewSObject<SGameObject>(L"CubeMapObject");
		SCubeMapRenderComponent* CubeMapComp = CubemapObject->CreateComponent<SCubeMapRenderComponent>(L"CubemapComponent");
		CubeMapComp->SetCubeMapTextureAssetName("ContentsAssets/T_Skybox01.tex");
		SGameObjectConstructor::FinishConstructHierarchy(CubemapObject);
		_DefaultWorld->AddToWorld(CubemapObject);
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

		//		SGameObject* CharacterModel = SRendererUtil::InstantiateMDLC(L"ContentsAssets/SKM_Quinn_Loco_02.mdlc", false);

		SBlendSpaceAnimTestComponent* AnimComp = CharacterModel->CreateComponent<SBlendSpaceAnimTestComponent>(L"AnimatorComp");

		_MainCharacter = NewSObject<SGameObject>("Character");
		CharacterModel->SetParent(_MainCharacter);

		SBoxColliderComponent* BoxComp = _MainCharacter->CreateComponent<SBoxColliderComponent>(L"SBoxColliderComponent");
		BoxComp->SetExtent(Vector4f(0.3f, 0.9, 0.3f, 0));
		BoxComp->SetOffset(Vector4f(0, 0.9f, 0.f, 0));
		SCharacterMovementComponent* CharacterComp = _MainCharacter->CreateComponent<SCharacterMovementComponent>(L"SCharacterComponent");

		CharacterComp->BindAnimComp(AnimComp);



		SGameObjectConstructor::FinishConstructHierarchy(_MainCharacter);


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



		_DefaultWorld->AddToWorld(_MainCharacter);


		// =====================================================================


		// GameManager
		SGameObject* GameManager = NewSObject<SGameObject>(L"GameManager");
		_MainPalyerController = GameManager->CreateComponent<SPlayerController>(L"PlayerController");
		_MainPalyerController->BindCharacter(_MainCharacter);


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

	if (true)
	{

		srand(time(NULL));

		for (int32 x = -4; x <= 4; x += 4)
		{
			for (int32 z = -4; z <= 4; z += 4)
			{

				// generate random from -PI to PI
				float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
				float PosNoiseX = (float)rand() / ((float)RAND_MAX / (2.0)) - 1;
				float PosNoiseZ = (float)rand() / ((float)RAND_MAX / (2.0)) - 1;

				float ScaleNoiseX = (float)rand() / ((float)RAND_MAX / (0.75)) + 0.25;
				float ScaleNoiseZ = (float)rand() / ((float)RAND_MAX / (0.75)) + 0.25;

				PosNoiseX += x;
				PosNoiseZ += z;
				Transform CubeTransform;
				CubeTransform.Position = { PosNoiseX,0.5,PosNoiseZ,1 };
				CubeTransform.Rotation = Quaternion::FromEulerRotation({ 0, r,0,0 });
				CubeTransform.Scale = { ScaleNoiseX , 1 , ScaleNoiseZ, 0 };

				SGameObject* Cube = CreateDynamicCube(CubeTransform);
				_DefaultWorld->AddToWorld(Cube);
			}
		}
	}
	else
	{
		Transform CubeTransform;
		CubeTransform.Position = { 1,0.5,0,1 };
		CubeTransform.Scale = { 2, 1, 0.25f, 0 };
		SGameObject* Cube = CreateDynamicCube(CubeTransform);
		_DefaultWorld->AddToWorld(Cube);
	}
}

void SSGame::PerFrameGame()
{
	SCOPE_PROFILE(PerFrameGame);

	if (SSInput::GetKeyDown(EKeyCode::KEY_1))
	{
		IWindow* MainWindow = g_MainWindowManager->GetMainWindow();
		MainWindow->SetForceMouseCenter(false);
		MainWindow->SetVisibleMouse(true);
	}
	else if (SSInput::GetKeyDown(EKeyCode::KEY_2))
	{
		IWindow* MainWindow = g_MainWindowManager->GetMainWindow();
		MainWindow->SetForceMouseCenter(true);
		MainWindow->SetVisibleMouse(false);
	}

	if (SSInput::GetKeyDown(EKeyCode::KEY_3))
	{
		Transform CharacterTransform = _MainCharacter->GetTransform();
		Vector4f Forward = CharacterTransform.GetForward();
		Vector4f NewPos = CharacterTransform.Position;
		NewPos = NewPos + Forward;

		_MainCharacter->SetPosition(NewPos);

	}


	PerFrame_DEBUGDRAW();
}

void SSGame::CleanupGame()
{
}

void SSGame::SetInGameFocus(bool bFocus)
{
	_bInGameFocus = bFocus;

	if (bFocus)
	{
		IWindow* MainWindow = g_MainWindowManager->GetMainWindow();
		MainWindow->SetForceMouseCenter(true);
		MainWindow->SetVisibleMouse(false);
	}
	else
	{
		IWindow* MainWindow = g_MainWindowManager->GetMainWindow();
		MainWindow->SetForceMouseCenter(false);
		MainWindow->SetVisibleMouse(true);
	}
}


void SSGame::PerFrame_DEBUGDRAW()
{
	IMeshAsset* Sphere = g_Renderer->GetCommonRenderAssetSet()->GetSphere1mMesh();
	IMeshAsset* Arrow = g_Renderer->GetCommonRenderAssetSet()->GetArrowMesh();
	IMeshAsset* HemiSphere = g_Renderer->GetCommonRenderAssetSet()->GetHemiSphereOutline1mMesh();

	ICollisionWorld* CollWorld = _DefaultWorld->GetCollWorld();
	SColliderBaseComponent* CharacterCollider = _MainCharacter->FindComponent<SColliderBaseComponent>();

	SS::PooledList<ICollInstanceBase*> _Collidables;
	CollWorld->QueryCollidableWith(_Collidables, CharacterCollider->GetCollInstance());

	for (ICollInstanceBase* ICI : _Collidables)
	{
		Transform DebugDrawTransform;

//		DebugDrawTransform.Position = ICI->GetWorldPos();
		DebugDrawTransform.Scale = { 0.1f, 0.1f, 0.1f, 0 };

		SRenderDebugUtil::DrawDebugMesh(
			_DefaultWorld,
			DebugDrawTransform,
			Sphere,
			false,
			{ 1,0,0,1 });
	}

	Transform DebugDrawTransform;
	DebugDrawTransform.Scale = { 1, 1, 1, 0 };
	DebugDrawTransform.Position = { 0, 1,0,1 };

	SRenderDebugUtil::DrawDebugMesh(
		_DefaultWorld,
		DebugDrawTransform,
		HemiSphere,
		true,
		{ 1,0,0,1 });


	SRenderDebugUtil::DrawLine(
		_DefaultWorld,
		{0, 0, 0, 1},
		{ 1,0,0,1 },
		false,
		{ 1,0,0,1 });

	SRenderDebugUtil::DrawLine(
		_DefaultWorld,
		{ 0, 0, 0, 1 },
		{ 0,1,0,1 },
		false,
		{ 0,1,0,1 });

	SRenderDebugUtil::DrawLine(
		_DefaultWorld,
		{ 0, 0, 0, 1 },
		{ 0,0, 1,1 },
		false,
		{ 0,0,1,1 });

}

SGameObject* SSGame::CreateDynamicCube(Transform InTransform)
{
	SGameObject* Cube = SRendererUtil::InstantiateModel(CRAN::CUBE1M_MDL, L"Cube", false);
	Cube->SetTransform(InTransform);

	SBoxColliderComponent* BoxCollider = Cube->CreateComponent<SBoxColliderComponent>(L"SBoxColliderComponent");
	BoxCollider->SetExtent({ 0.5f, 0.5f, 0.5f, 0 });
	SRigidBodyDynamicComponent* RB = Cube->CreateComponent<SRigidBodyDynamicComponent>(L"SRigidBodyDynamicComponent");
	RB->SetMass(20);
	SGameObjectConstructor::FinishConstructHierarchy(Cube);

	return Cube;
}
