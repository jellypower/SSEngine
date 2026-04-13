#pragma once
#include "SSCollision/Public/RigidBody/IRigidCahracterMovement.h"




class RigidCharacterMovement : public IRigidCahracterMovement
{
private:
	SObjHashCode _GameObjectHashCode = nullptr;

	Vector4f _SimulateBeginPos;
	Quaternion _SimulateBeginRot;

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
	bool _bMovedOnThisSimulation = false;
	Vector4f _SimulatedPosDelta;
	bool _bFaceChangedOnThisTick = false;

private:
	ICollInstanceBase* _CollInstance = nullptr;


public:
	RigidCharacterMovement();

public:
	virtual ERigidBodyType GetRigidBodyType() const override;

	virtual void UpdateInitialTransform(Vector4f Pos, Quaternion Rot) override;

	virtual bool IsMovedOnThisTick() const override;
	virtual bool IsMovedOnThisSimulation() const override;
	virtual bool IsRotatedOnThisSimulation() const override;
	virtual Vector4f GetSimulatedPosDelta() const override;
	virtual Quaternion GetSimulatedRotDelta() const override;

	virtual void OnBeginSimulation() override;
	virtual void SimulateMovement(float DeltaTime) override;
	virtual void OnEndSimulation() override;

	virtual ICollInstanceBase* GetCollInstance() const override;
	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) override;

	virtual SObjHashCode GetGameObjectID() const override;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) override;

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

private:
	void MovementPos(float DeltaTime);
	void MovementRotate(float DeltaTime);
	
};
