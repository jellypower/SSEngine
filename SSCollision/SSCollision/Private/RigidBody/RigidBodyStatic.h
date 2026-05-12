#pragma once
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"


struct RIGID_STATIC_DESC;

class RigidBodyStatic : public IRigidBodyBase
{
private:
	SObjHashCode _ComponentID;

	Vector4f _SimulBeginPos;
	Quaternion _SimulBeginRot;

	ICollisionWorld* _IncludedCollWorld = nullptr;
	ICollInstanceBase* _CollInstance = nullptr;

	physx::PxRigidStatic* _PxActor = nullptr;

public:
	RigidBodyStatic(const RIGID_STATIC_DESC& InDesc, physx::PxRigidStatic* InActor);
	virtual ~RigidBodyStatic();

public:
	virtual ERigidBodyType GetRigidBodyType() const override;
	virtual bool IsTransformModifiedOnThisTick() const override;

	virtual void SetSimulBeginPosAndRot_ByContent(const Vector4f& InPos, const Quaternion& InRot) override;
	virtual const Vector4f& GetSimulBeginPos() const override;
	virtual const Quaternion& GetSimulBeginRot() const override;

	virtual void OnBeginSimulation() override;
	virtual void OnEndSimulation() override;

	virtual ICollInstanceBase* GetCollInstance() const override;
	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) override;
	virtual void DetachCollInstance(ICollInstanceBase* BoundCI) override;

	virtual SObjHashCode GetGameObjectID() const override;
	virtual ICollisionWorld* GetIncludedCollWorld() const override;
	virtual void OnEnterTheCollWorld(ICollisionWorld* InCollWorld) override;
	virtual void OnExitFromCollWorld() override;

public:
	physx::PxRigidStatic* GetPxActor() const { return _PxActor; }
};
