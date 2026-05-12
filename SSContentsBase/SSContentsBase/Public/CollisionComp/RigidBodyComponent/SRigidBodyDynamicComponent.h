#pragma once
#include "SRigidBodyBaseComponent.h"

class IRigidBodyDynamic;


class SSCONTENTBASE_MODULE SRigidBodyDynamicComponent : public SRigidBodyBaseComponent
{
private:
	IRigidBodyDynamic* _RigidBodyDynamic = nullptr;

	float _Mass = 1.f;
	float _LinearDamping = 0.5f;
	float _AngularDamping = 0.5f;
	bool _bGravityEnabled = true;
	bool _bKinematic = false;

public:
	void SetMass(float InMass);
	void SetLinearDamping(float Damping);
	void SetAngularDamping(float Damping);
	void SetGravityEnabled(bool bEnable);
	void SetKinematic(bool bKinematic);

	// 런타임 물리 제어
	void AddForce(const Vector4f& Force);
	void AddImpulse(const Vector4f& Impulse);
	void ClearForce();
	Vector4f GetLinearVelocity() const;
	void SetLinearVelocity(const Vector4f& Velocity);


	// SRigidBodyBaseComponent
public:
	IRigidBodyBase* GetRigidBodyInstance() const override;
	void PostCollision_SyncTransform() override;

protected:
	void ConstructRigidBodyInstance() override;
	void DestructRigidBodyInstance() override;
};
