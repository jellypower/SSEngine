#pragma once


enum class ECharacterFaceMode;
class ICollisionWorld;

struct RIGID_CREATION_DESC
{
	Vector4f InitialWorldPos;
	Quaternion InitialWorldRot;
	SObjHashCode ComponentID;
};



struct RIGID_CHARACTERMOVEMENT_DESC : public RIGID_CREATION_DESC
{
	float AccelMultiplier = 20;
	float GroundFriction = 3;
	float MaxSpeed = 5.f;
	float MaxTurnSpeed = 5;
	float FaceTurnSpeed = 10;
	ECharacterFaceMode FaceMode;
};

