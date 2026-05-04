#pragma once
#include "SSCollision/Public/CollisionBase/ICollDevice.h"


class CollDevice : public ICollDevice
{
private:
	physx::PxDefaultAllocator _DefaultAllocatorCallback;
	physx::PxDefaultErrorCallback _DefaultErrorCallback;
	physx::PxTolerancesScale _ToleranceScale;

	physx::PxFoundation* _Foundation = nullptr; // PhysX용 winapi 셋같은거
	physx::PxPvd* _Pvd = nullptr; // 
	physx::PxPhysics* _Physics = nullptr;
	physx::PxDefaultCpuDispatcher* _Dispatcher = nullptr;

	physx::PxMaterial* _Material = nullptr;

public:
	CollDevice();
	virtual ~CollDevice();

public:
	virtual ICollisionWorld* CreateCollWorld(SS::SHasherW InWorldName) const override;
	virtual ICIBox* CreateCollBox() override;
	virtual ICISphere* CreateCollSphere() override;
	virtual IRigidCahracterMovement* CreateCharacterMovement() override;


	// Static Functions
public:
	virtual bool AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2) override;
};
