#pragma once
#include "SRigidBodyBaseComponent.h"

enum class ECharacterFaceMode;
class IRigidCahracterMovement;
class SColliderBaseComponent;
class SBlendSpaceAnimTestComponent;


class SSCONTENTBASE_MODULE SCharacterMovementComponent : public SRigidBodyBaseComponent
{
private:
	Vector2f _PrevBlendPoint;

private:
	float _FaceTurnSpeed;
	float _AnimLerpSpeed;

	SBlendSpaceAnimTestComponent* _AnimComp = nullptr;

private:
	IRigidCahracterMovement* _RigidCharacterMovement = nullptr;


public:
	SCharacterMovementComponent();

public:
	bool ShouldProcessPerFrameInherently() const override;
	void PerFrame(float DeltaTime) override;



public:
	void BindAnimComp(SBlendSpaceAnimTestComponent* Comp);

	void SetFaceMode(ECharacterFaceMode Mode);
	void SetEnteredFace(Vector2f InDir);
	void AddAccel(Vector2f InAccel);

private:
	void Animate(float DeltaTime);


	// RigidBodyComponent
public:
	IRigidBodyBase* GetRigidBodyInstance() const override;
	void PostCollision_SyncTransform() override;

protected:
	void ConstructRigidBodyInstance() override;
	void DestructRigidBodyInstance() override;

};
