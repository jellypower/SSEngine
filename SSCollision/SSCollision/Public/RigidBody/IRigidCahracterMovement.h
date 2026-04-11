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
	virtual void SetFaceMode(ECharacterFaceMode Mode) = 0;
	virtual void SetEnteredFace(Vector2f InDir) = 0;
	virtual void AddAccel(Vector2f InAccel) = 0;
};
