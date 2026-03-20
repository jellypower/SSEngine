#include "pch.h"
#include "SCharacterComponent.h"
#include <cmath>

#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/AnimComponents/SBlendSpaceAnimTestComponent.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

SCharacterComponent::SCharacterComponent()
{
	_AccelMultiplier = 20;
	_GroundFriction = 3;
	_MaxSpeed = 5.f;
	_MaxTurnSpeed = 5;
	_FaceTurnSpeed = 10;
	_AnimLerpSpeed = 5;
	_FaceMode = ECharacterFaceMode::LerpToVelocity;

	_EnteredFace = { 0, 1 };
	_CurFace = { 0, 1 };
}

bool SCharacterComponent::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SCharacterComponent::PerFrame(float DeltaTime)
{
	double SmoothDeltaTime = SSFrameInfo::GetSmoothDeltaTime();
	double TimeScale = GetIncludedWorld()->GetTimeScale();

	PerFrameMovement(SmoothDeltaTime * TimeScale);
	Animate(DeltaTime);
}

void SCharacterComponent::BindAnimComp(SBlendSpaceAnimTestComponent* Comp)
{
	_AnimComp = Comp;
}


void SCharacterComponent::SetFaceMode(ECharacterFaceMode Mode)
{
	_FaceMode = Mode;
}


void SCharacterComponent::SetEnteredFace(Vector2f InDir)
{
	_EnteredFace = InDir;
}

void SCharacterComponent::AddAccel(Vector2f InAccel)
{
	_MoveInput = _MoveInput + InAccel;
}


void SCharacterComponent::MovementRotate(float DeltaTime)
{
	SGameObject* GO = GetGameObject();
	bool bCurFaceEdited = false;

	do
	{
		if (_FaceMode == ECharacterFaceMode::LerpToVelocity)
		{
			float VelLenSqr = _MoveLateralVelocity.GetSqrLength();
			if (VelLenSqr < 0.0001f)
			{
				break;
			}

			float VelLen = sqrt(VelLenSqr);
			Vector2f VelocityNormalized = _MoveLateralVelocity / VelLen;
			float FaceCosSim = SS::Dot(_CurFace, VelocityNormalized);
			if (FaceCosSim > 0.9999f)
			{
				break;
			}

			float TurnAmount = _FaceTurnSpeed * DeltaTime;
			TurnAmount = TurnAmount > 1 ? 1 : TurnAmount;

			

			_CurFace = SS::Slerp2D(_CurFace, VelocityNormalized, TurnAmount);
			bCurFaceEdited = true;
		}
		else if (_FaceMode == ECharacterFaceMode::LerpToEnteredFace)
		{
			float VelLenSqr = _MoveLateralVelocity.GetSqrLength();
			if (VelLenSqr < 0.0001f)
			{
				break;
			}

			float EnteredFaceSqrLen = _EnteredFace.GetSqrLength();
			if (EnteredFaceSqrLen < 0.0001)
			{
				_EnteredFace = _CurFace;
				break; // goto
			}
			if (EnteredFaceSqrLen < 0.9999)
			{
				_EnteredFace = _EnteredFace.GetNormalized();
			}

			float FaceCosSim = SS::Dot(_CurFace, _EnteredFace);
			if (FaceCosSim < 0.99f)
			{
				float TurnAmount = _FaceTurnSpeed * DeltaTime;
				TurnAmount = TurnAmount > 1 ? 1 : TurnAmount;

				_CurFace = SS::Slerp2D(_CurFace, _EnteredFace, TurnAmount);
				bCurFaceEdited = true;
			}
		}
		else
		{
			SS_ASSERT(false);
		}
	}
	while (false); // goto-target


	{
		Vector4f Start = GO->GetTransform().Position;
		Vector4f End = Start;
		End.X += _CurFace.X;
		End.Z += _CurFace.Y;
		IMeshAsset* Arrow = g_Renderer->GetCommonRenderAssetSet()->GetArrowMesh();
		SRenderDebugUtil::DrawDirectionalMesh(GetIncludedWorld(), Start, End, Arrow, false, 0.5f, 
			{ 0,0,1,1 });
	}
	
	if (bCurFaceEdited)
	{
		float Yaw = atan2(_CurFace.X, _CurFace.Y); // 모델 방향이 Z+ 를 바라보는것이 전제라서 y랑 x방향 바꿔줌
		GO->SetRotation(Quaternion::FromEulerRotation({ 0, Yaw, 0, 0 }));
	}
}

void SCharacterComponent::PerFrameMovement(float DeltaTime)
{
	if (DeltaTime > 0.1f)
	{
		DeltaTime = 0.1f; // 너무 큰 델타타임은 금지
	}

	SGameObject* GO = GetGameObject();

	Vector4f CurPos = GO->GetTransform().Position;

	const float MoveInputSqrLen = _MoveInput.GetSqrLength();
	const float VeloSqrLen = _MoveLateralVelocity.GetSqrLength();

	Vector2f MoveInputNormalized = _MoveInput.GetNormalized();
	Vector2f VeloNormalized = _MoveLateralVelocity.GetNormalized();

	float VelAccelCosSim = SS::Dot(MoveInputNormalized, VeloNormalized);


	// 가속하지 않거나 정반대로 가려고 하는 경우에 마찰력 적용
	if (MoveInputSqrLen < 0.0001f ||
		VelAccelCosSim < 0
		)
	{
		constexpr float GRAVITIONAL_FORCE = 9.8f;
		// 마찰력 = 수직항력 * 마찰계수
		float FrictionForce = GRAVITIONAL_FORCE * _GroundFriction;

		float FrictionForceDelta = FrictionForce * DeltaTime;

		float FrictionForceDeltaSqr = FrictionForceDelta * FrictionForceDelta;
		if (FrictionForceDeltaSqr > VeloSqrLen) // 마찰력이 더 크면 그냥 속도를 리셋
		{
			_MoveLateralVelocity = Vector2f::Zero;
		}
		else
		{
			Vector2f VelocityDamping = -FrictionForceDelta * VeloNormalized;
			_MoveLateralVelocity = _MoveLateralVelocity + VelocityDamping;
		}
	}


	// 가속하는 경우
	if (MoveInputSqrLen >= 0.0001f)
	{
		constexpr double TEMP_DEG = SS::DegToRadians(95);
		static const double TURN_THRESHOLD = std::cos(TEMP_DEG);

		const float MaxSpeedSqr = _MaxSpeed * _MaxSpeed;

		// 속도가 거의 최대인 상황에서 반대방향이 아닌 곳으로
		// 회전하려 하면 그냥 바로 회전 가능하게 해주기
		if (VeloSqrLen > MaxSpeedSqr - 0.1 &&
			VelAccelCosSim > TURN_THRESHOLD)
		{
			float TurnAmount = _MaxTurnSpeed * DeltaTime;
			TurnAmount = TurnAmount > 1 ? 1 : TurnAmount;

			Vector2f NewVelo = SS::Slerp2D(VeloNormalized, MoveInputNormalized, TurnAmount);
			float VeloLen = sqrt(VeloSqrLen);

			_MoveLateralVelocity = NewVelo * VeloLen;
		}
		else // 회전이 목표가 아닌 경우엔 가속해주기
		{
			Vector2f VelocityDelta = _MoveInput * (DeltaTime * _AccelMultiplier);
			_MoveLateralVelocity = _MoveLateralVelocity + VelocityDelta;

			float SpeedSqr = _MoveLateralVelocity.GetSqrLength();
			if (SpeedSqr > 0.001f)
			{
				float Speed = sqrt(SpeedSqr);

				if (Speed > _MaxSpeed) // 속도가 너무 빠르면 제한
				{
					float SpeedInv = 1 / Speed;
					Vector2f Dir = _MoveLateralVelocity * SpeedInv;
					_MoveLateralVelocity = Dir * _MaxSpeed;
				}

			}
		}
	}



	_MoveInput = Vector2f::Zero;


	{
		float PostSqrLen = _MoveLateralVelocity.GetSqrLength();

		if (PostSqrLen < 0.01f * 0.01f)
		{
			_MoveLateralVelocity = Vector2f::Zero;
		}
	}

	{
		Vector4f NewPos;

		NewPos.X = CurPos.X + (_MoveLateralVelocity.X * DeltaTime);
		NewPos.Z = CurPos.Z + (_MoveLateralVelocity.Y * DeltaTime);

		GO->SetPosition(NewPos);
	}

	// DEBUG
	{
		SGameObject* GO = GetGameObject();
		float VeloSqrLen = _MoveLateralVelocity.GetSqrLength();
		float VelLen = sqrt(VeloSqrLen);
		Vector2f Velo =_MoveLateralVelocity.GetNormalized();
		Velo = Velo * (VelLen / _MaxSpeed);
		Vector4f Start = GO->GetTransform().Position;
		Vector4f End = Start;
		End.X += Velo.X;
		End.Z += Velo.Y;
		IMeshAsset* Arrow = g_Renderer->GetCommonRenderAssetSet()->GetArrowMesh();

		SRenderDebugUtil::DrawDirectionalMesh(GetIncludedWorld(), Start, End, Arrow, false, 0.5f,
				{1, 0, 0, 1});
	}

	MovementRotate(DeltaTime);
}

void SCharacterComponent::Animate(float DeltaTime)
{
	const float VeloSqrLen = _MoveLateralVelocity.GetSqrLength();

	Vector2f NewBlendPoint;

	if (VeloSqrLen > 0.0001f)
	{
		const float VeloLen = sqrt(VeloSqrLen);
		const float MaxSpeedSqr = _MaxSpeed * _MaxSpeed;
		Vector2f LaterlVeloNormalized = _MoveLateralVelocity / VeloLen;

		float LateralVeloYaw = atan2(LaterlVeloNormalized.X, LaterlVeloNormalized.Y);
		float FaceYaw = atan2(_CurFace.X, _CurFace.Y);

		float TEMP = atan2(0, 0);

		float AnimateYaw = LateralVeloYaw - FaceYaw;
		NewBlendPoint.Y = cos(AnimateYaw);
		NewBlendPoint.X = sin(AnimateYaw);

		float SpeedRatio = VeloLen / _MaxSpeed;

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