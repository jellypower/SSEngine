#include "pch.h"
#include "SCharacterComponent.h"

#include <cmath>

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/AnimComponents/SBlendSpaceAnimTestComponent.h"

SCharacterComponent::SCharacterComponent()
{
	_AccelMultiplier = 20;
	_GroundFriction = 3;
	_MaxSpeed = 5.f;
	_MaxTurnSpeed = 5;
}

bool SCharacterComponent::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SCharacterComponent::PerFrame(float DeltaTime)
{
	PerFrameMovement(DeltaTime);
	Animate(DeltaTime);
}

void SCharacterComponent::PostConstructHierarchy()
{
	SGameObject* GO = GetGameObject();
	_AnimComp = GO->FindComponent<SBlendSpaceAnimTestComponent>();

	_CameraBoom = NewSObject<SGameObject>(L"PlayerCameraBoom");
	_CameraBoom->SetPosition({ 0, 1, -5, 0 });
	_CameraBoom->SetParent(GO);
}

const Transform& SCharacterComponent::CalcCameraTransform() const
{
	return _CameraBoom->CalcWorldTransform();
}

void SCharacterComponent::SetFaceDir(Vector2f InDir)
{
	float SqrLen = InDir.GetSqrLength();
	if (SqrLen > 0.0001)
	{
		_FaceDir = { 1, 0 };
	}
	else
	{
		float Len = sqrt(SqrLen);
		_FaceDir = InDir * (1 / Len);
	}
}


void SCharacterComponent::AddAccel(Vector2f InAccel)
{
	_MoveInput = _MoveInput + InAccel;
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
		Vector2f PosDelta = _MoveLateralVelocity * DeltaTime;
		CurPos.X += (_MoveLateralVelocity.X * DeltaTime);
		CurPos.Z += (_MoveLateralVelocity.Y * DeltaTime);

		GO->SetPosition(CurPos);
	}
}

void SCharacterComponent::Animate(float DeltaTime)
{
	const float VeloSqrLen = _MoveLateralVelocity.GetSqrLength();
	const float LateralSpeed = sqrt(VeloSqrLen);
	const float MaxSpeedSqr = _MaxSpeed * _MaxSpeed;

	Vector2f BlendPoint;

	if (LateralSpeed > 0.01f)
	{
		Vector2f LaterlVeloNormalized = _MoveLateralVelocity;

		float SpeedRatio = LateralSpeed / MaxSpeedSqr;

		BlendPoint = LaterlVeloNormalized * SpeedRatio;
	}

	_AnimComp->SetBlendPoint(BlendPoint);
}