#pragma once
#include "IRigidbodySim.h"

class IRigidBodyDynamic : public IRigidbodySim
{
public:
	// Force & Impulse
	virtual void AddForce(const Vector4f& Force) = 0;
	virtual void AddImpulse(const Vector4f& Impulse) = 0;
	virtual void ClearForce() = 0;

	// Velocity
	virtual Vector4f GetLinearVelocity() const = 0;
	virtual void SetLinearVelocity(const Vector4f& Velocity) = 0;
	virtual Vector4f GetAngularVelocity() const = 0;
	virtual void SetAngularVelocity(const Vector4f& Velocity) = 0;

	// Mass
	virtual float GetMass() const = 0;
	virtual void SetMass(float InMass) = 0;

	// Damping
	virtual float GetLinearDamping() const = 0;
	virtual void SetLinearDamping(float Damping) = 0;
	virtual float GetAngularDamping() const = 0;
	virtual void SetAngularDamping(float Damping) = 0;

	// Gravity
	virtual bool IsGravityEnabled() const = 0;
	virtual void SetGravityEnabled(bool bEnable) = 0;

	// Kinematic
	virtual bool IsKinematic() const = 0;
	virtual void SetKinematic(bool bKinematic) = 0;
	virtual void SetKinematicTarget(const Vector4f& Pos, const Quaternion& Rot) = 0;
};
