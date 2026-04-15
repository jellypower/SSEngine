#include "pch.h"
#include "SCameraController.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"

bool SCameraController::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SCameraController::PerFrame(float DeltaTime)
{
	BlendCamera(DeltaTime);
}

void SCameraController::PostConstructHierarchy()
{
	
}

void SCameraController::OnEnterTheWorld()
{
	_PlayerCameraComp = GetGameObject()->FindComponent<SCameraComponent>();
	_PlayerCameraComp->SetFOVWithDegrees(60);
	_PlayerCameraComp->SetNearZ(0.01f);
	_PlayerCameraComp->SetFarZ(40.f);
}

void SCameraController::BindFollowTarget(SGameObject* Target)
{
	_FollowTarget = Target;
}

void SCameraController::SetCamTargetPitchYaw(float Pitch, float Yaw)
{
	_CamPitchTarget = Pitch;
	_CamYawTarget = Yaw;
}

void SCameraController::SetCamTargetFOV(float NewFOVInDeg)
{
	_CamTargetFOVInDeg = NewFOVInDeg;
}

void SCameraController::BlendCamera(float DeltaTime)
{
	SGameObject* Target = _FollowTarget.Get();
	if (Target == nullptr)
	{
		return;
	}


	float Alpha = DeltaTime * _CamBlendSpeed;
	Alpha = Alpha > 1 ? 1 : Alpha;



	if (_CamYawTarget - _CamCurYaw > XM_PI) // Target이 359이고 Cur이 1인 경우
	{
		_CamCurYaw += XM_2PI;
	}
	else if (_CamCurYaw - _CamYawTarget > XM_PI) // Cur이 359이고 Cur이 1인 경우
	{
		_CamYawTarget += XM_2PI;
	}
	_CamCurYaw = SS::Lerp(_CamCurYaw, _CamYawTarget, Alpha);
	_CamCurYaw = fmodf(_CamCurYaw, XM_2PI);
	_CamYawTarget = fmodf(_CamYawTarget, XM_2PI);

	_CamCurPitch = SS::Lerp(_CamCurPitch, _CamPitchTarget, Alpha);
	_CamCurFovInDeg = SS::Lerp(_CamCurFovInDeg, _CamTargetFOVInDeg, Alpha);


	XMMATRIX TargetTransformMatrix = Target->CalcWorldTransformMatrix();
	XMVECTOR TargetPos = TargetTransformMatrix.r[3];


	XMVECTOR RelativePos = { 0, _ArmHeight, -_TargetArmLen, 0 };
	Quaternion Rotation = Quaternion::FromEulerRotation({ _CamCurPitch, _CamCurYaw, 0, 0 });

	RelativePos = XMVector3Rotate(RelativePos, Rotation.SimdVec);

	Transform NewTransform(
		TargetPos + RelativePos,
		Rotation,
		{ 1, 1, 1, 0 });

	SGameObject* GO = GetGameObject();
	GO->SetTransform(NewTransform);

	_PlayerCameraComp->SetFOVWithDegrees(_CamCurFovInDeg);
}
