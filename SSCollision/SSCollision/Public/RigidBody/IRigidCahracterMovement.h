#pragma once
#include "IRigidBodyBase.h"


enum class ECharacterFaceMode
{
	None,

	LerpToVelocity,
	LerpToEnteredFace
};


class IRigidCahracterMovement : public IRigidBodyBase
{
public:
	virtual bool IsCurFaceEditedOnThisTick() const = 0;
	virtual Vector2f GetCurFaceDir() const = 0;
	virtual Vector2f GetLateralVelocity() const = 0;
	virtual float GetMaxSpeed() const = 0;

	virtual void SetFaceMode(ECharacterFaceMode Mode) = 0;
	virtual void SetEnteredFace(Vector2f InDir) = 0;
	virtual void AddMovementAccel(Vector2f InAccel) = 0;

};
