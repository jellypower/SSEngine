#pragma once
#include "SSCollision/Public/RigidBody/IRigidCahracterMovement.h"


struct RIGID_CHARACTERMOVEMENT_DESC;

class RigidCharacterMovement : public IRigidCahracterMovement
{
private:
	SObjHashCode _ComponentID;

	float _AccelMultiplier;
	float _GroundFriction;
	float _MaxSpeed; // m/s
	float _MaxTurnSpeed; // 이동속도가 Max에 가까울 때 회전속도
	float _FaceTurnSpeed;
	ECharacterFaceMode _FaceMode = ECharacterFaceMode::LerpToVelocity;

private:
	Vector2f _MoveInput;

	Vector2f _MoveLateralVelocity;
	// _MoveLateralVelocity.Y값이 Z축 이동,
	// _MoveLateralVelocity.X값이 X축 이동
	Vector2f _CurFace;
	// _CurFace.Y 값이 Z축 이동,
	// _CurFace.X 값이 X축 이동
	Vector2f _EnteredFace;

private:
	bool _bTransformModifiedOnThisTick = false;
	bool _bMovedOnThisSimulation = false;
	bool _bFaceChangedOnThisTick = false;

	Vector4f _SimulBeginPos;
	Vector4f _SimulEndPos;

	float _VerticalVelocity = 0.f;
	bool _bIsGrounded = false;
	bool _bJumpRequested = false;
	float _JumpImpulse = 5.f;
	float _GravityScale = 1.f;

private:
	ICollisionWorld* _IncludedCollWorld = nullptr;
	ICollInstanceBase* _CollInstance = nullptr;

	physx::PxRigidDynamic* _PxActor = nullptr;

public:
	RigidCharacterMovement(const RIGID_CHARACTERMOVEMENT_DESC& InDesc, physx::PxRigidDynamic* InActor);

public:
	virtual ERigidBodyType GetRigidBodyType() const override;

	virtual bool IsTransformModifiedOnThisTick() const override;

	// Transform
	virtual void SetSimulBeginPosAndRot_ByContent(const Vector4f& InPos, const Quaternion& InRot) override;

	virtual const Vector4f& GetSimulBeginPos() const override;
	virtual const Vector4f& GetSimulEndPos() const override;
	virtual Vector4f CalcPosDelta() const override;

	virtual const Quaternion& GetSimulBeginRot() const override;
	virtual const Quaternion& GetSimulEndRot() const override;
	virtual Quaternion CalcRotDelta() const override;

	// Simulation
	virtual void OnBeginSimulation() override;
	virtual void SimulateMovement(float DeltaTime) override;
	virtual void OnEndSimulation() override;

	virtual ICollInstanceBase* GetCollInstance() const override;
	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) override;
	virtual void DetachCollInstance(ICollInstanceBase* BoundCI) override;

	virtual SObjHashCode GetGameObjectID() const override;
	ICollisionWorld* GetIncludedCollWorld() const override;
	virtual void OnEnterTheCollWorld(ICollisionWorld* InRenderWorld) override;
	virtual void OnExitFromCollWorld() override;

public:
	virtual bool IsCurFaceEditedOnThisTick() const override;
	virtual Vector2f GetCurFaceDir() const override;
	virtual Vector2f GetLateralVelocity() const override;
	virtual float GetMaxSpeed() const override;

	virtual void SetFaceMode(ECharacterFaceMode Mode) override;
	virtual void SetEnteredFace(Vector2f InDir) override;
	virtual void AddMovementAccel(Vector2f InAccel) override;

	virtual void RequestJump() override;
	virtual bool IsGrounded() const override;

private:
	void MovementPos(float DeltaTime);
	void MovementVertical(float DeltaTime);
	void MovementRotate(float DeltaTime);

public:
	physx::PxRigidBody* GetPxActor() const { return _PxActor; }
};
