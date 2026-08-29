#pragma once
#include <windows.h>

#include "SSCollision/Public/CollisionBase/ICollDevice.h"


class SSPhysXErrorCallback : public physx::PxErrorCallback
{
public:
	virtual void reportError(physx::PxErrorCode::Enum Code, const char* Message, const char* File, int Line) override
	{
		wchar_t Buf[1024];
		swprintf_s(Buf, L"[PhysX] %hs  (%hs:%d)\n", Message, File, Line);
		OutputDebugStringW(Buf);
	}
};


class CollDevice : public ICollDevice
{
private:
	physx::PxDefaultAllocator _DefaultAllocatorCallback;
	SSPhysXErrorCallback _DefaultErrorCallback;
	physx::PxTolerancesScale _ToleranceScale;

	physx::PxFoundation* _Foundation = nullptr; // PhysX용 winapi 셋같은거
	physx::PxPvd* _Pvd = nullptr; // 
	physx::PxPhysics* _Physics = nullptr;
	physx::PxDefaultCpuDispatcher* _Dispatcher = nullptr;

	physx::PxMaterial* _DefaultMaterial = nullptr;

public:
	CollDevice();
	void Release() override;

public:
	virtual ICollisionWorld* CreateCollWorld(SS::SHasherW InWorldName) const override;
	virtual ICIBox* CreateCollBox(const CI_BOX_DESC& InDesc) override;
	virtual ICISphere* CreateCollSphere(const CI_SPHERE_DESC& InDesc) override;
	virtual IRigidBodyBase* CreateStaticRigidBody(const RIGID_STATIC_DESC& InDesc) override;
	virtual IRigidCahracterMovement* CreateCharacterMovement(const RIGID_CHARACTERMOVEMENT_DESC& InDesc) override;
	virtual IRigidBodyDynamic* CreateDynamicRigidBody(const RIGID_DYNAMIC_DESC& InDesc) override;


	// Static Functions
public:
	virtual bool AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2) override;
};
