#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SCharacterMovementComponent.h"

#include <cmath>

#include "SSCollision/Public/CollisionBase/ICollDevice.h"
#include "SSCollision/Public/RigidBody/RigidCreationDesc.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"

#include "SSContentsBase/Public/AnimComponents/SBlendSpaceAnimTestComponent.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

SCharacterMovementComponent::SCharacterMovementComponent()
{
	_FaceTurnSpeed = 10;
	_AnimLerpSpeed = 5;
}

bool SCharacterMovementComponent::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SCharacterMovementComponent::PerFrame(float DeltaTime)
{
	double SmoothDeltaTime = SSFrameInfo::GetSmoothDeltaTime();
	double TimeScale = GetIncludedWorld()->GetTimeScale();

	Animate(DeltaTime);
}

void SCharacterMovementComponent::BindAnimComp(SBlendSpaceAnimTestComponent* Comp)
{
	_AnimComp = Comp;
}


void SCharacterMovementComponent::SetFaceMode(ECharacterFaceMode Mode)
{
	_RigidCharacterMovement->SetFaceMode(Mode);
}


void SCharacterMovementComponent::SetEnteredFace(Vector2f InDir)
{
	_RigidCharacterMovement->SetEnteredFace(InDir);
}

void SCharacterMovementComponent::AddAccel(Vector2f InAccel)
{
	_RigidCharacterMovement->AddMovementAccel(InAccel);
}

void SCharacterMovementComponent::Animate(float DeltaTime)
{
	Vector2f CuFace = _RigidCharacterMovement->GetCurFaceDir();
	float MaxSpeed = _RigidCharacterMovement->GetMaxSpeed();
	Vector2f LateralVelocity = _RigidCharacterMovement->GetLateralVelocity();
	const float VeloSqrLen = LateralVelocity.GetSqrLength();



	Vector2f NewBlendPoint;

	if (VeloSqrLen > 0.0001f)
	{
		const float VeloLen = sqrt(VeloSqrLen);
		const float MaxSpeedSqr = MaxSpeed * MaxSpeed;
		Vector2f LaterlVeloNormalized = LateralVelocity / VeloLen;

		float LateralVeloYaw = atan2(LaterlVeloNormalized.X, LaterlVeloNormalized.Y);
		float FaceYaw = atan2(CuFace.X, CuFace.Y);

		float TEMP = atan2(0, 0);

		float AnimateYaw = LateralVeloYaw - FaceYaw;
		NewBlendPoint.Y = cos(AnimateYaw);
		NewBlendPoint.X = sin(AnimateYaw);

		float SpeedRatio = VeloLen / MaxSpeed;

		NewBlendPoint = NewBlendPoint * SpeedRatio;
	}


	// 애니메이션이 급격히 바뀌면 몸이 떨린다
	float tLerp = DeltaTime * _AnimLerpSpeed;
	tLerp = tLerp > 1 ? 1 : tLerp;
	NewBlendPoint = SS::Lerp(_PrevBlendPoint, NewBlendPoint, tLerp);


	// DEBUG
	{
		SGameObject* GO = GetGameObject();
		Vector4f Start = GO->GetTransform().Position;
		Vector4f End = Start;
		End.X += NewBlendPoint.X;
		End.Z += NewBlendPoint.Y;
		IMeshAsset* Arrow = g_Renderer->GetCommonRenderAssetSet()->GetArrowMesh();

		SRenderDebugUtil::DrawDirectionalMesh(GetIncludedWorld(), Start, End, Arrow, false);
	}

	_PrevBlendPoint = NewBlendPoint;
	SS_ASSERT(isnan(NewBlendPoint.X) == false);
	SS_ASSERT(isnan(NewBlendPoint.Y) == false);
	SS_ASSERT(isinf(NewBlendPoint.X) == false);
	SS_ASSERT(isinf(NewBlendPoint.Y) == false);

	_AnimComp->SetBlendPoint(NewBlendPoint);
}

IRigidBodyBase* SCharacterMovementComponent::GetRigidBodyInstance() const
{
	return _RigidCharacterMovement;
}

void SCharacterMovementComponent::PostCollision_SyncTransform()
{
	SGameObject* GO = GetGameObject();
	Transform NewTransform = GO->GetTransform();

	NewTransform.Position = _RigidCharacterMovement->GetSimulEndPos();

	Vector2f CurFace = _RigidCharacterMovement->GetCurFaceDir();

	float Yaw = atan2(CurFace.X, CurFace.Y); // 모델 방향이 Z+ 를 바라보는것이 전제라서 y랑 x방향 바꿔줌
	NewTransform.Rotation = Quaternion::FromEulerRotation({ 0, Yaw, 0, 0 });

	GO->SetTransform(NewTransform);
}

void SCharacterMovementComponent::ConstructRigidBodyInstance()
{
	RIGID_CHARACTERMOVEMENT_DESC Desc;

	const SGameObject* GO = GetGameObject();

	Desc.InitialWorldPos = GO->GetTransform().Position;
	Desc.InitialWorldRot = GO->GetTransform().Rotation;
	Desc.ComponentID = GetHashCode();
	Desc.FaceMode = ECharacterFaceMode::LerpToVelocity;


	_RigidCharacterMovement = g_CollDevice->CreateCharacterMovement(Desc);
}

void SCharacterMovementComponent::DestructRigidBodyInstance()
{
	delete _RigidCharacterMovement;
	_RigidCharacterMovement = nullptr;
}
