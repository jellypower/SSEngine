#pragma once
#include "SSCollision/Public/RigidBody/IRigidBodyDynamic.h"


struct RIGID_DYNAMIC_DESC;

class RigidBodyDynamic : public IRigidBodyDynamic
{
private:
	SObjHashCode _ComponentID;

	Vector4f _SimulBeginPos;
	Vector4f _SimulEndPos;
	Quaternion _SimulBeginRot;
	Quaternion _SimulEndRot;


private:
	ICollisionWorld* _IncludedCollWorld = nullptr;
	ICollInstanceBase* _CollInstance = nullptr;

	physx::PxRigidDynamic* _PxActor = nullptr;

public:
	RigidBodyDynamic(const RIGID_DYNAMIC_DESC& InDesc, physx::PxRigidDynamic* InActor);

public:
	virtual ERigidBodyType GetRigidBodyType() const override;

	virtual bool IsTransformModifiedOnThisTick() const override;

	// Transform
	virtual void SetSimulBeginPosAndRot_ByContent(const Vector4f& InPos, const Quaternion& InRot) override;
	virtual const Vector4f& GetSimulBeginPos() const override;
	virtual const Vector4f& GetSimulEndPos() const override;
	virtual Vector4f CalcPosDelta() const override;
	virtual const Quaternion& GetSimulBeginRot() const override;
	virtual const Quaternion& GetSimulEndRot() const override;
	virtual Quaternion CalcRotDelta() const override;

	// Simulation
	virtual void OnBeginSimulation() override;
	virtual void OnEndSimulation() override;

	virtual ICollInstanceBase* GetCollInstance() const override;
	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) override;
	virtual void DetachCollInstance(ICollInstanceBase* BoundCI) override;

	virtual SObjHashCode GetGameObjectID() const override;
	virtual ICollisionWorld* GetIncludedCollWorld() const override;
	virtual void OnEnterTheCollWorld(ICollisionWorld* InCollWorld) override;
	virtual void OnExitFromCollWorld() override;

	// IRigidBodyDynamic
public:
	virtual void AddForce(const Vector4f& Force) override;
	virtual void AddImpulse(const Vector4f& Impulse) override;
	virtual void ClearForce() override;

	virtual Vector4f GetLinearVelocity() const override;
	virtual void SetLinearVelocity(const Vector4f& Velocity) override;
	virtual Vector4f GetAngularVelocity() const override;
	virtual void SetAngularVelocity(const Vector4f& Velocity) override;

	virtual float GetMass() const override;
	virtual void SetMass(float InMass) override;

	virtual float GetLinearDamping() const override;
	virtual void SetLinearDamping(float Damping) override;
	virtual float GetAngularDamping() const override;
	virtual void SetAngularDamping(float Damping) override;

	virtual bool IsGravityEnabled() const override;
	virtual void SetGravityEnabled(bool bEnable) override;

	virtual bool IsKinematic() const override;
	virtual void SetKinematic(bool bKinematic) override;
	virtual void SetKinematicTarget(const Vector4f& Pos, const Quaternion& Rot) override;

public:
	physx::PxRigidDynamic* GetPxActor() const { return _PxActor; }
};
