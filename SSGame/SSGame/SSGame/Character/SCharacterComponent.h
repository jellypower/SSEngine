#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SBlendSpaceAnimTestComponent;

class SCharacterComponent : public SComponentBase
{
private:
	Vector2f _FaceDir;

	Vector2f _MoveInput;
	Vector2f _MoveLateralVelocity;
	float _AccelMultiplier;
	float _GroundFriction;
	float _MaxSpeed; // m/s

	float _MaxTurnSpeed; // 이동속도가 Max에 가까울 때 회전속도


	SBlendSpaceAnimTestComponent* _AnimComp = nullptr;

	SGameObject* _CameraBoom = nullptr;

public:
	SCharacterComponent();

public:
	bool ShouldProcessPerFrameInherently() const override;
	void PerFrame(float DeltaTime) override;

	void PostConstructHierarchy() override;


public:
	const Transform& CalcCameraTransform() const;

public:
	void SetFaceDir(Vector2f InDir);
	void AddAccel(Vector2f InAccel);

private:
	void PerFrameMovement(float DeltaTime);
	void Animate(float DeltaTime);
};
