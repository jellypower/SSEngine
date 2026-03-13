#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SCharacterComponent : public SComponentBase
{
private:
	Vector2f _MoveInput;
	Vector2f _MoveLateralVelocity;
	float _AccelMultiplier;
	float _GroundFriction;
	float _MaxSpeed; // m/s

	float _MaxTurnSpeed; // 이동속도가 Max에 가까울 때 회전속도


public:
	SCharacterComponent();

public:
	bool ShouldProcessPerFrameInherently() const override;
	void PerFrame(float DeltaTime) override;


public:
	void AddAccel(Vector2f InAccel);
};
