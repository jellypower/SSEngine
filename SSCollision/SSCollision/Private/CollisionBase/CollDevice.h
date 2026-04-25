#pragma once
#include "SSCollision/Public/CollisionBase/ICollDevice.h"

#include <physx/include/PxPhysics.h>
#include <physx/include/PxPhysicsAPI.h>

class CollDevice : public ICollDevice
{
private:
	physx::PxDefaultAllocator	mDefaultAllocatorCallback;
	physx::PxDefaultErrorCallback	mDefaultErrorCallback;
	physx::PxFoundation* mFoundation = NULL;
	physx::PxPhysics* mPhysics = NULL;
	physx::PxTolerancesScale	mToleranceScale;
	physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
	physx::PxScene* mScene = NULL;
	physx::PxMaterial* mMaterial = NULL;
	physx::PxPvd* mPvd = NULL;

public:
	CollDevice();

public:
	virtual ICollisionWorld* CreateCollWorld(SS::SHasherW InWorldName) const override;
	virtual ICIBox* CreateCollBox() override;
	virtual ICISphere* CreateCollSphere() override;
	virtual IRigidCahracterMovement* CreateCharacterMovement() override;


	// Static Functions
public:
	virtual bool AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2) override;
};
