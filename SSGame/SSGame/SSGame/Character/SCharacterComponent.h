#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SBlendSpaceAnimTestComponent;


enum class ECharacterFaceMode
{
	None,

	LerpToVelocity,
	LerpToEnteredFace
};


class SCharacterComponent : public SComponentBase
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
	float _AnimLerpSpeed;

	ECharacterFaceMode _FaceMode;


	SBlendSpaceAnimTestComponent* _AnimComp = nullptr;


public:
	SCharacterComponent();

public:
	bool ShouldProcessPerFrameInherently() const override;
	void PerFrame(float DeltaTime) override;



public:
	void BindAnimComp(SBlendSpaceAnimTestComponent* Comp);

	void SetFaceMode(ECharacterFaceMode Mode);
	void SetEnteredFace(Vector2f InDir);
	void AddAccel(Vector2f InAccel);

private:
	void PerFrameMovement(float DeltaTime);
	void MovementRotate(float DeltaTime);
	void Animate(float DeltaTime);
};
