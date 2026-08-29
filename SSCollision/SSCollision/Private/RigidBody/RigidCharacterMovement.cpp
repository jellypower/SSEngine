#include "pch.h"
#include "RigidCharacterMovement.h"

#include <cmath>

#include "SSCollision/Private/CollDetect/CollDebug_Private.h"
#include "SSCollision/Private/CollInstance/CIUtils_Private.h"
#include "SSCollision/Public/DEBUG/CollDebugDrawDescs.h"
#include "SSCollision/Public/RigidBody/RigidCreationDesc.h"
#include "SSCollision/Private/Util_Private/PxCustomFilters.h"


RigidCharacterMovement::RigidCharacterMovement(const RIGID_CHARACTERMOVEMENT_DESC& InDesc, physx::PxRigidDynamic* InActor)
{
	_ComponentID = InDesc.ComponentID;

	_AccelMultiplier = InDesc.AccelMultiplier;
	_GroundFriction = InDesc.GroundFriction;
	_MaxSpeed = InDesc.MaxSpeed;
	_MaxTurnSpeed = InDesc.MaxTurnSpeed;
	_FaceTurnSpeed = InDesc.FaceTurnSpeed;
	_FaceMode = InDesc.FaceMode;
	_JumpImpulse = InDesc.JumpImpulse;
	_GravityScale = InDesc.GravityScale;


	_EnteredFace = { 0, 1 };
	_CurFace = { 0, 1 };

	_PxActor = InActor;
}

void RigidCharacterMovement::Release()
{
	delete this;
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

	_SimulEndPos = _SimulBeginPos;

	MovementPos(DeltaTime);
	MovementVertical(DeltaTime);
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
	_SimulEndPos = InPos;

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


	// ApplyMovement with sweep+slide
	{
		const float SqrLenPostInputProcess = _MoveLateralVelocity.GetSqrLength();

		if (SqrLenPostInputProcess < 0.01f * 0.01f)
		{
			_MoveLateralVelocity = Vector2f::Zero;
		}
		else
		{
			_bTransformModifiedOnThisTick = true;
			_bMovedOnThisSimulation = true;

			Vector4f DesiredMove = {
				_MoveLateralVelocity.X * DeltaTime,
				0.f,
				_MoveLateralVelocity.Y * DeltaTime,
				0.f
			};

			physx::PxScene* Scene = _PxActor->getScene();
			physx::PxShape* Shape = ExtractPxShape(_CollInstance);


			if (Scene == nullptr || Shape == nullptr) // 충돌이 없으면 그냥 움직임
			{
				_SimulEndPos.X += DesiredMove.X;
				_SimulEndPos.Z += DesiredMove.Z;
			}
			else
			{
				SelfExcludeFilter Filter(_PxActor); // 나는 제외하고 Overlap을 진행
				physx::PxQueryFilterData FilterData(
					physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::ePREFILTER);

				physx::PxTransform ShapeLocalPose = Shape->getLocalPose();

				Vector4f RemainingMove = DesiredMove;
				Vector4f CurPos = _SimulEndPos;

				for (int32 Iter = 0; Iter < 3; ++Iter)
				{
					float MoveSqrLen = RemainingMove.X * RemainingMove.X + RemainingMove.Z * RemainingMove.Z;
					if (MoveSqrLen < 0.00001f)
					{
						break;
					}

					float MoveLen = sqrtf(MoveSqrLen);
					Vector4f MoveDir = { RemainingMove.X / MoveLen, 0.f, RemainingMove.Z / MoveLen, 0.f };

					physx::PxTransform ActorPose;
					ActorPose.p = PxTransformConvert::Vec3ToPx(CurPos);
					ActorPose.q = _PxActor->getGlobalPose().q;
					physx::PxTransform WorldGeomPose = ActorPose * ShapeLocalPose;

					physx::PxVec3 SweepDir = PxTransformConvert::Vec3ToPx(MoveDir);

					physx::PxSweepBuffer Hit;
					bool bHit = Scene->sweep(
						Shape->getGeometry(), 
						WorldGeomPose, 
						SweepDir, 
						MoveLen, 
						Hit,
						physx::PxHitFlag::eDEFAULT, FilterData, &Filter);

					if (bHit == false || Hit.block.distance >= MoveLen) // 부딧히는게 없으면 그냥 이동
					{
						CurPos.X += RemainingMove.X;
						CurPos.Z += RemainingMove.Z;
						break;
					}

					// SafeDist: 내가 충돌없이 이동할 수 있는 최대 거리
					float SafeDist = Hit.block.distance > 0.001f ? Hit.block.distance - 0.001f : 0.f;
					CurPos.X += MoveDir.X * SafeDist;
					CurPos.Z += MoveDir.Z * SafeDist;

					// Wall sliding: 벽 법선에서 XZ 성분만 사용
					Vector4f HitNormal = PxTransformConvert::Vec3FromPx(Hit.block.normal);
					HitNormal.Y = 0.f;
					float NormalLen = sqrtf(HitNormal.X * HitNormal.X + HitNormal.Z * HitNormal.Z);
					if (NormalLen < 0.001f)
					{
						break;
					}
					HitNormal.X /= NormalLen;
					HitNormal.Z /= NormalLen;

					float DotN = RemainingMove.X * HitNormal.X + RemainingMove.Z * HitNormal.Z;
					// 노말이 밀어내는 방향이랑 내가 가는 방향이 반대라면
					if (DotN < 0.f)
					{
						// DotN: Normal에 내가 움직이는 방향으로 수선을 내린다
						// 즉 내 이동 방향이 얼마나 벽 쪽으로 이동하려 하는가를 구하는 것

						// 그래서 내 움직임에서 벽쪽으로 들어가려는 힘만 그냥 빼버린다
						RemainingMove.X -= HitNormal.X * DotN;
						RemainingMove.Z -= HitNormal.Z * DotN;
					}

					// RemainingLen: 원래 이동하려 했던 거리중 충돌해서 아직 못 간 거리 -> 아직 이동 가능한 최대 거리
					float RemainingLen = MoveLen - SafeDist;

					// SlideSqrLen: 벽으로 뚫고들어간 힘이 빠지고 남은 가야 할 거리
					float SlideSqrLen = RemainingMove.X * RemainingMove.X + RemainingMove.Z * RemainingMove.Z;

					// RemainingLen 은 실제로 트레이스 하고나서 가려했던 움직임의 크기이고
					// SlideSqrLen 은 실제로 슬라이딩을 완료한 이후의 움직임의 크기이다
					// 트레이스랑 슬라이딩 완료하고 난 값이랑 오차가 있을 수 있기 때문에 그 오차만큼 보정해준다
					if (SlideSqrLen > RemainingLen * RemainingLen && SlideSqrLen > 0.00001f)
					{
						float SlideLen = sqrtf(SlideSqrLen);
						// RemainingLen값 으로 벡터 길이를 맞춰준다
						RemainingMove.X *= (RemainingLen / SlideLen);
						RemainingMove.Z *= (RemainingLen / SlideLen);
					}
				}

				_SimulEndPos.X = CurPos.X;
				_SimulEndPos.Z = CurPos.Z;
			}
		}

	}



	// DEBUG
	{
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

void RigidCharacterMovement::MovementVertical(float DeltaTime)
{
	constexpr float GRAVITY = 9.8f;
	constexpr float FLOOR_SKIN = 0.01f;
	constexpr float GROUNDED_CHECK_DIST = 0.12f;

	physx::PxScene* Scene = _PxActor->getScene();
	physx::PxShape* Shape = ExtractPxShape(_CollInstance);
	

	// 바닥 감지 (낙하 중이거나 지면에 있을 때만)
	_bIsGrounded = false;
	if (Scene != nullptr && Shape != nullptr && _VerticalVelocity <= 0.f)
	{
		physx::PxTransform ActorPose;
		ActorPose.p = PxTransformConvert::Vec3ToPx(_SimulEndPos);
		ActorPose.q = _PxActor->getGlobalPose().q;
		physx::PxTransform WorldGeomPose = ActorPose * Shape->getLocalPose();

		SelfExcludeFilter Filter(_PxActor);
		physx::PxQueryFilterData FilterData(
			physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::ePREFILTER);

		physx::PxSweepBuffer GroundHit;
		bool bGroundHit = Scene->sweep(
			Shape->getGeometry(),  // 현재 Shape를 바닥으로 Sweep해본다
			WorldGeomPose,
			physx::PxVec3(0.f, -1.f, 0.f),
			GROUNDED_CHECK_DIST,
			GroundHit,
			physx::PxHitFlag::eDEFAULT, 
			FilterData, 
			&Filter);

		_bIsGrounded = bGroundHit;
	}

	// 바닥에 있을 때 점프
	if (_bJumpRequested && _bIsGrounded)
	{
		_VerticalVelocity = _JumpImpulse;
		_bIsGrounded = false;
		_bTransformModifiedOnThisTick = true;
	}
	_bJumpRequested = false;

	// 중력 적용
	if (_bIsGrounded && _VerticalVelocity <= 0.f)
	{
		_VerticalVelocity = 0.f;
	}
	else
	{
		_VerticalVelocity -= GRAVITY * _GravityScale * DeltaTime;
	}

	float VertDelta = _VerticalVelocity * DeltaTime;
	if (fabsf(VertDelta) < 0.0001f) // 수직으로 움직이지 않으면 패스
	{
		return;
	}

	_bTransformModifiedOnThisTick = true;

	// 수직 방향 Sweep으로 움직여야 하는 경우 처리
	if (Scene != nullptr && Shape != nullptr)
	{
		physx::PxTransform ActorPose;
		ActorPose.p = PxTransformConvert::Vec3ToPx(_SimulEndPos);
		ActorPose.q = _PxActor->getGlobalPose().q;
		physx::PxTransform WorldGeomPose = ActorPose * Shape->getLocalPose();

		float SweepSign = VertDelta > 0.f ? 1.f : -1.f; // 천장인지 바닥인지 체크
		float SweepDist = fabsf(VertDelta);

		SelfExcludeFilter Filter(_PxActor);
		physx::PxQueryFilterData FilterData(
			physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::ePREFILTER);

		physx::PxSweepBuffer HitResult;
		bool bHit = Scene->sweep(
			Shape->getGeometry(), 
			WorldGeomPose,
			physx::PxVec3(0.f, SweepSign, 0.f),
			SweepDist, 
			HitResult,
			physx::PxHitFlag::eDEFAULT, 
			FilterData, 
			&Filter);

		if (bHit && HitResult.block.distance < SweepDist)
		{
			float SafeDist = HitResult.block.distance > FLOOR_SKIN ? HitResult.block.distance - FLOOR_SKIN : 0.f;
			_SimulEndPos.Y += SweepSign * SafeDist;
			_VerticalVelocity = 0.f;
			if (SweepSign < 0.f) // 바닥에 부딧힌 경우는 Ground
			{
				_bIsGrounded = true;
			}
		}
		else
		{
			_SimulEndPos.Y += VertDelta;
		}
	}
	else
	{
		_SimulEndPos.Y += VertDelta;
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

void RigidCharacterMovement::RequestJump()
{
	_bJumpRequested = true;
}

bool RigidCharacterMovement::IsGrounded() const
{
	return _bIsGrounded;
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
