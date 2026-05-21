#pragma once


enum class ECharacterFaceMode;
class ICollisionWorld;

struct RIGID_CREATION_DESC
{
	Vector4f InitialWorldPos;
	Quaternion InitialWorldRot;
	SObjHashCode ComponentID;
};



struct RIGID_STATIC_DESC : public RIGID_CREATION_DESC
{
};


struct RIGID_DYNAMIC_DESC : public RIGID_CREATION_DESC
{
	float Mass = 1.f;
	float LinearDamping = 0.f;
	float AngularDamping = 0.05f;
	bool bGravityEnabled = true;
	bool bKinematic = false;
};

struct RIGID_CHARACTERMOVEMENT_DESC : public RIGID_CREATION_DESC
{
	float AccelMultiplier = 20;
	float GroundFriction = 3;
	float MaxSpeed = 5.f;
	float MaxTurnSpeed = 5;
	float FaceTurnSpeed = 10;
	ECharacterFaceMode FaceMode;
	float JumpImpulse = 5.f;
	float GravityScale = 1.f;
};

