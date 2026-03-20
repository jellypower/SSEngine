#include "pch.h"
#include "SPlayerController.h"

#include "SCameraController.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"

#include "SSEngineDefault/Public/RawInput/SSInput.h"
#include "SSEngineDefault/Public/SSAlgorithm.h"

#include "SSGame/Character/SCharacterComponent.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

SPlayerController::SPlayerController()
{
}

bool SPlayerController::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SPlayerController::PostConstructHierarchy()
{
	_PlayerCameraGO = NewSObject<SGameObject>(L"PlayerCamera");
	_PlayerCameraComp = _PlayerCameraGO->CreateComponent<SCameraComponent>("PlayerCamera");
	_PlayerCameraController = _PlayerCameraGO->CreateComponent<SCameraController>("CameraController");
	SGameObjectConstructor::FinishConstructHierarchy(_PlayerCameraGO);

	if (_CharacterGO != nullptr)
	{
		_PlayerCameraController->BindFollowTarget(_CharacterGO);
	}
}

void SPlayerController::OnEnterTheWorld()
{
	SWorld* World = GetIncludedWorld();
	World->AddToWorld(_PlayerCameraGO);
}

void SPlayerController::OnExitTheWorld()
{

}

void SPlayerController::PreDestructHierarchy()
{

}

const SCameraComponent* SPlayerController::GetCameraComp() const
{
	return _PlayerCameraComp;
}

void SPlayerController::PerFrame(float DeltaTime)
{
	// TODO: Hack임 나중에 변경하기
	static int32 InitialInputDelay = 10;
	
	if (InitialInputDelay > 0)
	{
		InitialInputDelay--;
		return;
	}

	const IRenderCamera* RenderCam = g_Renderer->GetMainRenderCamera();
	if (RenderCam != nullptr && RenderCam == _PlayerCameraComp->GetRenderCamera())
	{
		ProcessInput(DeltaTime);
	}
}

void SPlayerController::BindCharacter(SGameObject* InCharacterGO)
{
	_CharacterGO = InCharacterGO;
	_CharacterComp = _CharacterGO->FindComponent<SCharacterComponent>();

	if (_PlayerCameraController != nullptr)
	{
		_PlayerCameraController->BindFollowTarget(_CharacterGO);
	}
}


void SPlayerController::ProcessInput(float DeltaTime)
{
	{
		const Vector2f MouseDelta = SSInput::GetMouseDelta();

		_ControlYaw += MouseDelta.X * _ControlRotSensitivity;
		_ControlYaw = fmodf(_ControlYaw + XM_2PI, XM_2PI);

		_ControlPitch += (-MouseDelta.Y) * _ControlRotSensitivity;
		_ControlPitch = SS::Clamp(_ControlPitch, -XM_PIDIV4, XM_PIDIV4);

		_PlayerCameraController->SetCamTargetPitchYaw(_ControlPitch, _ControlYaw);
	}



	Quaternion Rotation = Quaternion::FromEulerRotation({ 0, _ControlYaw, 0, 0 });

	XMVECTOR LateralForward = { 0, 0,1, 0 };
	LateralForward = XMVector3Rotate(LateralForward, Rotation.SimdVec);

	XMVECTOR LateralRight = { 1, 0, 0, 0 };
	LateralRight = XMVector3Rotate(LateralRight, Rotation.SimdVec);

	Vector2f Forward = {LateralForward.m128_f32[0], LateralForward.m128_f32[2]};
	Vector2f Right = {LateralRight.m128_f32[0], LateralRight.m128_f32[2]};

	if (SSInput::GetMouseDown(EMouseCode::MOUSE_RIGHT))
	{
		_CharacterComp->SetFaceMode(ECharacterFaceMode::LerpToEnteredFace);
		_PlayerCameraController->SetCamTargetFOV(45);
	}
	else if (SSInput::GetMouseUp(EMouseCode::MOUSE_RIGHT))
	{
		_CharacterComp->SetFaceMode(ECharacterFaceMode::LerpToVelocity);
		_PlayerCameraController->SetCamTargetFOV(60);
	}

	if (SSInput::GetMouse(EMouseCode::MOUSE_RIGHT))
	{
		_CharacterComp->SetEnteredFace(Forward);
	}


	if (SSInput::GetKey(EKeyCode::KEY_A))
	{
		_CharacterComp->AddAccel(-Right);
	}
	else if (SSInput::GetKey(EKeyCode::KEY_D))
	{
		_CharacterComp->AddAccel(Right);
	}

	if (SSInput::GetKey(EKeyCode::KEY_W))
	{
		_CharacterComp->AddAccel(Forward);
	}
	else if (SSInput::GetKey(EKeyCode::KEY_S))
	{
		_CharacterComp->AddAccel(-Forward);
	}



}