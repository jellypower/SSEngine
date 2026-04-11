#pragma once
#include "SSCollision/Public/RigidBody/IRigidCahracterMovement.h"




class RigidCharacterMovement : public IRigidCahracterMovement
{
private:
	Vector2f _MoveInput;
	Vector2f _MoveLateralVelocity;
	Vector2f _CurFace;
	Vector2f _PrevBlendPoint;

	float _AccelMultiplier;
	float _GroundFriction;
	float _MaxSpeed; // m/s
	float _MaxTurnSpeed; // 이동속도가 Max에 가까울 때 회전속도

private:
	Vector2f _EnteredFace;
	float _FaceTurnSpeed;
	ECharacterFaceMode _FaceMode = ECharacterFaceMode::LerpToVelocity;

private:
	Vector4f _SimulatedPosResult;
	Quaternion _SimulatedRotResult;

private:
	ICollInstanceBase* _CollInstance = nullptr;


public:
	RigidCharacterMovement();

public:
	virtual void SimulateTick(float DeltaTime) override;
	virtual Vector4f GetSimulatedPos() const override;
	virtual Quaternion GetSimulatedRot() const override;

	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) override;


	virtual void SetFaceMode(ECharacterFaceMode Mode) override;
	virtual void SetEnteredFace(Vector2f InDir) override;
	virtual void AddAccel(Vector2f InAccel) override;

private:
	void MovementRotate(float DeltaTime);
	
};
