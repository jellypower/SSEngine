#pragma once
#include "SSCollision/Public/RigidBody/IRigidCahracterMovement.h"




class RigidCharacterMovement : public IRigidCahracterMovement
{
private:
	Vector2f _MoveInput;
	Vector2f _MoveLateralVelocity;
	Vector2f _CurFace;

	float _AccelMultiplier;
	float _GroundFriction;
	float _MaxSpeed; // m/s
	float _MaxTurnSpeed; // 이동속도가 Max에 가까울 때 회전속도

private:
	Vector2f _EnteredFace;
	float _FaceTurnSpeed;
	ECharacterFaceMode _FaceMode = ECharacterFaceMode::LerpToVelocity;

private:
	bool _bMovedOnThisTick = false;
	Vector4f _SimulatedPosDelta;
	bool _bFaceChangedOnThisTick = false;

private:
	ICollInstanceBase* _CollInstance = nullptr;


public:
	RigidCharacterMovement();

public:
	virtual ERigidBodyType GetRigidBodyType() const override;

	virtual bool IsMovedOnThisTick() const override;
	virtual bool IsRotatedOnThisTick() const override;
	virtual Vector4f GetSimulatedPosDelta() const override;
	virtual Quaternion GetSimulatedRotDelta() const override;

	virtual void SimulateTick(float DeltaTime) override;
	virtual void OnEndSimulation() override;

	virtual ICollInstanceBase* GetCollInstance() const override;
	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) override;

	virtual void OnEnterTheCollWorld(ICollisionWorld* InRenderWorld) override;
	virtual void OnExitFromCollWorld() override;

public:
	virtual bool IsCurFaceEditedOnThisTick() const override;
	virtual Vector2f GetCurFaceDir() const override;

	virtual void SetFaceMode(ECharacterFaceMode Mode) override;
	virtual void SetEnteredFace(Vector2f InDir) override;
	virtual void AddMovementAccel(Vector2f InAccel) override;

private:
	void MovementPos(float DeltaTime);
	void MovementRotate(float DeltaTime);
	
};
