#include "pch.h"
#include "RigidCharacterMovement.h"

#include <cmath>

#include "SSCollision/Private/CollDetect/CollDebug_Private.h"
#include "SSCollision/Private/CollInstance/CIUtils_Private.h"
#include "SSCollision/Public/CollInstance/ICIBox.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/DEBUG/CollDebugDrawDescs.h"
#include "SSCollision/Public/RigidBody/RigidCreationDesc.h"


RigidCharacterMovement::RigidCharacterMovement(const RIGID_CHARACTERMOVEMENT_DESC& InDesc, physx::PxRigidDynamic* InActor)
{
	_ComponentID = InDesc.ComponentID;

	_AccelMultiplier = InDesc.AccelMultiplier;
	_GroundFriction = InDesc.GroundFriction;
	_MaxSpeed = InDesc.MaxSpeed;
	_MaxTurnSpeed = InDesc.MaxTurnSpeed;
	_FaceTurnSpeed = InDesc.FaceTurnSpeed;
	_FaceMode = InDesc.FaceMode;


	_EnteredFace = { 0, 1 };
	_CurFace = { 0, 1 };

	_PxActor = InActor;
}

ERigidBodyType RigidCharacterMovement::GetRigidBodyType() const
{
	return ERigidBodyType::CharacterMovement;
}


bool RigidCharacterMovement::IsTransformModifiedOnThisTick() const
{
	return _bTransformModifiedOnThisTick;
}

void RigidCharacterMovement::SimulateMovement(float DeltaTime)
{
	_bMovedOnThisSimulation = false;
	if (DeltaTime > 0.1f)
	{
		DeltaTime = 0.1f; // 너무 큰 델타타임은 금지
	}

	MovementPos(DeltaTime);
	MovementRotate(DeltaTime);


	physx::PxTransform Target;
	Target.p = PxTransformConvert::Vec3ToPx(_SimulEndPos);

	// SCharacterMovementComponent::PostCollision_SyncTransform 함수를 보면 결국
	// 최종 로테이션의 Yaw값은 float Yaw = atan2(CurFace.X, CurFace.Y); 이렇게 계산함
	// 그런데, PhysX는 오른손 좌표계니까 Yaw값만 뒤집어줘야 제대로된 KinematicTarget이 들어감
	float Yaw = -atan2(_CurFace.X, _CurFace.Y);
	Target.q = physx::PxQuat(Yaw, physx::PxVec3(0, 1, 0));
	_PxActor->setKinematicTarget(Target);
}

void RigidCharacterMovement::OnEndSimulation()
{
#if DEBUG
	CollDebug_Private::DrawPXRigid(this, {0, 1, 0, 0}, true);
#endif

	_MoveInput = Vector2f::Zero;
}

void RigidCharacterMovement::SetSimulBeginPosAndRot_ByContent(const Vector4f& InPos, const Quaternion& InRot)
{
	_SimulBeginPos = InPos;

	physx::PxTransform Pose;
	Pose.p = PxTransformConvert::Vec3ToPx(InPos);
	Pose.q = PxTransformConvert::QuatToPx(InRot);
	_PxActor->setGlobalPose(Pose);
}

const Vector4f& RigidCharacterMovement::GetSimulBeginPos() const
{
	return _SimulBeginPos;
}


const Vector4f& RigidCharacterMovement::GetSimulEndPos() const
{
	return _SimulEndPos;
}

Vector4f RigidCharacterMovement::CalcPosDelta() const
{
	return _SimulEndPos - _SimulBeginPos;
}

const Quaternion& RigidCharacterMovement::GetSimulBeginRot() const
{
	// 캐릭터의 Rotation은 _CurFace로 취급합니다.
	// 즉, 물리 시뮬레이션에 의한 RotationDelta는 존재하지 않습니다.
	return Quaternion();
}


const Quaternion& RigidCharacterMovement::GetSimulEndRot() const
{
	// 캐릭터의 Rotation은 _CurFace로 취급합니다.
	// 즉, 물리 시뮬레이션에 의한 RotationDelta는 존재하지 않습니다.
	return Quaternion();
}

Quaternion RigidCharacterMovement::CalcRotDelta() const
{
	// 캐릭터의 Rotation은 _CurFace로 취급합니다.
	// 즉, 물리 시뮬레이션에 의한 RotationDelta는 존재하지 않습니다.
	return  Quaternion();
}

void RigidCharacterMovement::OnBeginSimulation()
{
	_bTransformModifiedOnThisTick = false;
	_SimulBeginPos = _SimulEndPos;
}


void RigidCharacterMovement::MovementPos(float DeltaTime)
{
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


	// ApplyMovement
	{
		float PostSqrLen = _MoveLateralVelocity.GetSqrLength();

		if (PostSqrLen < 0.01f * 0.01f)
		{
			_MoveLateralVelocity = Vector2f::Zero;
		}
		else
		{
			_bTransformModifiedOnThisTick = true;
			_bMovedOnThisSimulation = true;
			
			_SimulEndPos.X = _SimulBeginPos.X + (_MoveLateralVelocity.X * DeltaTime);
			_SimulEndPos.Z = _SimulBeginPos.Z + (_MoveLateralVelocity.Y * DeltaTime);
		}

	}



	// DEBUG
	{
		float VeloSqrLen = _MoveLateralVelocity.GetSqrLength();
		float VelLen = sqrt(VeloSqrLen);
		Vector2f Velo = _MoveLateralVelocity.GetNormalized();
		Velo = Velo * (VelLen / _MaxSpeed);
		Vector4f End = _SimulEndPos;
		End.X += Velo.X;
		End.Z += Velo.Y;

		CDDD_Line Desc;
		Desc.Start = _SimulEndPos;
		Desc.End = End;
		Desc.Color = { 1, 0, 0, 1 };
		Desc.bUseDepth = true;

		// CollDebug_Private::DrawLine(_CollInstance->GetIncludedCollWorld(), Desc);
	}

}

void RigidCharacterMovement::MovementRotate(float DeltaTime)
{
	_bFaceChangedOnThisTick = false;

	float TurnAmount = _FaceTurnSpeed * DeltaTime;
	TurnAmount = TurnAmount > 1 ? 1 : TurnAmount;

	float PrevYaw = atan2(_CurFace.Y, _CurFace.X);
	PrevYaw += XM_2PI;
	PrevYaw = fmod(PrevYaw, XM_2PI);

	float TargetYaw = 0;


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

			// 만약 오른쪽(X+)으로 움직이는 경우에 TargetYaw값은 0이된다.
			TargetYaw = atan2(VelocityNormalized.Y, VelocityNormalized.X);
			TargetYaw += XM_2PI;
			TargetYaw = fmod(TargetYaw, XM_2PI);
			_bFaceChangedOnThisTick = true;
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
			if (FaceCosSim >= 0.99f)
			{
				break;
			}


			TargetYaw = atan2(_EnteredFace.Y, _EnteredFace.X);
			TargetYaw += XM_2PI;
			TargetYaw = fmod(TargetYaw, XM_2PI);
			_bFaceChangedOnThisTick = true;
		}
		else
		{
			SS_ASSERT(false);
		}
	} while (false); // goto-target


	if (_bFaceChangedOnThisTick)
	{
		_bTransformModifiedOnThisTick = true;

		float Diff = TargetYaw - PrevYaw;
		if (Diff > XM_PI) // ex) PrevYaw=0 to TargetYaw=270
		{
			PrevYaw += XM_2PI;
		}
		else if (Diff < -XM_PI) // ex) PrevYaw=270 to TargetYaw=0
		{
			TargetYaw += XM_2PI;
		}

		// TODO: 여기 문제있는듯. 고치자.
		float NewYaw = SS::Lerp(PrevYaw, TargetYaw, TurnAmount);
		 
		_CurFace.X = cos(NewYaw);
		_CurFace.Y = sin(NewYaw);
	}

	// Debug
	{
		Vector4f End = _SimulBeginPos;
		End.X += _CurFace.X;
		End.Z += _CurFace.Y;

		CDDD_Line Desc;
		Desc.Start = _SimulBeginPos;
		Desc.End = End;
		Desc.Color = { 0, 1, 0, 1 };
		Desc.bUseDepth = true;
		CollDebug_Private::DrawLine(_IncludedCollWorld, Desc);
	}
}

void RigidCharacterMovement::BindCollisionInstance(ICollInstanceBase* BoundCI)
{
	if (_CollInstance != nullptr)
	{
		SS_ASSERT_MSG(false, "TOOD: Attach multiple Colinstance");
		return;
	}

	_CollInstance = BoundCI;
	physx::PxShape* Shape = ExtractPxShape(_CollInstance);
	_PxActor->attachShape(*Shape);
}

void RigidCharacterMovement::DetachCollInstance(ICollInstanceBase* BoundCI)
{
	if (_CollInstance != BoundCI)
	{
		SS_ASSERT(false);
		return;
	}

	physx::PxShape* Shape = ExtractPxShape(BoundCI);
	_PxActor->detachShape(*Shape);
}


SObjHashCode RigidCharacterMovement::GetGameObjectID() const
{
	return _ComponentID;
}

ICollisionWorld* RigidCharacterMovement::GetIncludedCollWorld() const
{
	return _IncludedCollWorld;
}

ICollInstanceBase* RigidCharacterMovement::GetCollInstance() const
{
	return _CollInstance;
}

void RigidCharacterMovement::OnEnterTheCollWorld(ICollisionWorld* InRenderWorld)
{
	_IncludedCollWorld = InRenderWorld;
	// noop
}

void RigidCharacterMovement::OnExitFromCollWorld()
{
	_MoveInput = Vector2f();
	_MoveLateralVelocity = Vector2f();
	_EnteredFace = Vector2f();
	_IncludedCollWorld = nullptr;
}

bool RigidCharacterMovement::IsCurFaceEditedOnThisTick() const
{
	return _bFaceChangedOnThisTick;
}

Vector2f RigidCharacterMovement::GetCurFaceDir() const
{
	return _CurFace;
}

Vector2f RigidCharacterMovement::GetLateralVelocity() const
{
	return _MoveLateralVelocity;
}

float RigidCharacterMovement::GetMaxSpeed() const
{
	return _MaxSpeed;
}

void RigidCharacterMovement::SetFaceMode(ECharacterFaceMode Mode)
{
	_FaceMode = Mode;
}

void RigidCharacterMovement::SetEnteredFace(Vector2f InDir)
{
	_EnteredFace = InDir;
}

void RigidCharacterMovement::AddMovementAccel(Vector2f InAccel)
{
	_MoveInput = _MoveInput + InAccel;
}
