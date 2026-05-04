#include "pch.h"
#include "CollDevice.h"

#include "CollisionWorld.h"

#include "SSCollision/Private/CollInstance/CIBox.h"
#include "SSCollision/Private/CollDetect/CollCalc_Private.h"
#include "SSCollision/Private/CollInstance/CISphere.h"
#include "SSCollision/Private/RigidBody/RigidCharacterMovement.h"

#include "SSEngineDefault/Public/Collision/CollMathInline.h"

CollDevice::CollDevice()
{
	_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _DefaultAllocatorCallback, _DefaultErrorCallback);
	if (!_Foundation) throw("PxCreateFoundation failed!");
	_Pvd = PxCreatePvd(*_Foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	_ToleranceScale.length = 100;        // typical length of an object
	_ToleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
	_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_Foundation, _ToleranceScale, true, _Pvd);

	_Dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
}

CollDevice::~CollDevice()
{
	PX_RELEASE(_Dispatcher);
	PX_RELEASE(_Physics);
	if (_Pvd)
	{
		physx::PxPvdTransport* transport = _Pvd->getTransport();
		PX_RELEASE(_Pvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(_Foundation);
}

ICollisionWorld* CollDevice::CreateCollWorld(SS::SHasherW InWorldName) const
{
	physx::PxSceneDesc sceneDesc(_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.f, -9.81f, 0.f);
	sceneDesc.cpuDispatcher = _Dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	physx::PxScene* PhysXScene = _Physics->createScene(sceneDesc);


	physx::PxPvdSceneClient* pvdClient = PhysXScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}



	return DBG_NEW CollisionWorld(InWorldName, PhysXScene);
}

ICIBox* CollDevice::CreateCollBox()
{
	return DBG_NEW CIBox();
}

ICISphere* CollDevice::CreateCollSphere()
{
	return DBG_NEW CISphere();
}

IRigidCahracterMovement* CollDevice::CreateCharacterMovement()
{
	return DBG_NEW RigidCharacterMovement();
}

bool CollDevice::AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2)
{
	const AABBBox& BBoxC1 = c1->GetBBox();
	const AABBBox& BBoxC2 = c2->GetBBox();


	if (CollMath_Inline::BBIntersect(BBoxC1, BBoxC2) == false)
	{
		return false;
	}


	return CollCalc_Private::GJK(c1, c2);
}
