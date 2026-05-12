#include "pch.h"
#include "CollDevice.h"

#include "CollisionWorld.h"

#include "SSCollision/Private/CollInstance/CIBox.h"
#include "SSCollision/Private/CollDetect/CollCalc_Private.h"
#include "SSCollision/Private/CollInstance/CISphere.h"
#include "SSCollision/Private/RigidBody/RigidBodyStatic.h"
#include "SSCollision/Private/RigidBody/RigidBodyDynamic.h"
#include "SSCollision/Private/RigidBody/RigidCharacterMovement.h"
#include "SSCollision/Public/CollInstance/CICreationDesc.h"
#include "SSCollision/Public/RigidBody/RigidCreationDesc.h"

#include "SSEngineDefault/Public/Collision/CollMathInline.h"

CollDevice::CollDevice()
{
	_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _DefaultAllocatorCallback, _DefaultErrorCallback);
	if (!_Foundation) throw("PxCreateFoundation failed!");
	_Pvd = PxCreatePvd(*_Foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	_ToleranceScale.length = 1;          // typical length of an object (meter scale)
	_ToleranceScale.speed = 9.81f;       // typical speed of an object, gravity*1s is a reasonable choice
	_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_Foundation, _ToleranceScale, true, _Pvd);

	_Dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	_DefaultMaterial = _Physics->createMaterial(0.6f, 0.6f, 0.0f);
}

CollDevice::~CollDevice()
{
	PX_RELEASE(_DefaultMaterial);
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

ICIBox* CollDevice::CreateCollBox(const CI_BOX_DESC& InDesc)
{
	physx::PxShape* Shape = _Physics->createShape(
		physx::PxBoxGeometry(InDesc.Extent.X, InDesc.Extent.Y, InDesc.Extent.Z),
		*_DefaultMaterial,
		true);

	CIBox* NewBox = DBG_NEW CIBox(InDesc, Shape);
	return NewBox;
}

ICISphere* CollDevice::CreateCollSphere(const CI_SPHERE_DESC& InDesc)
{
	physx::PxShape* Shape = _Physics->createShape(
		physx::PxSphereGeometry(InDesc.Radius),
		*_DefaultMaterial,
		true);

	CISphere* NewSphere = DBG_NEW CISphere(InDesc, Shape);
	return NewSphere;
}

IRigidBodyBase* CollDevice::CreateStaticRigidBody(const RIGID_STATIC_DESC& InDesc)
{
	physx::PxTransform InitialPose;
	InitialPose.p = PxTransformConvert::Vec3ToPx(InDesc.InitialWorldPos);
	InitialPose.q = PxTransformConvert::QuatToPx(InDesc.InitialWorldRot);
	physx::PxRigidStatic* Actor = _Physics->createRigidStatic(InitialPose);

	return DBG_NEW RigidBodyStatic(InDesc, Actor);
}

IRigidCahracterMovement* CollDevice::CreateCharacterMovement(const RIGID_CHARACTERMOVEMENT_DESC& InDesc)
{
	physx::PxTransform InitialPose;
	InitialPose.p = PxTransformConvert::Vec3ToPx(InDesc.InitialWorldPos);
	InitialPose.q = PxTransformConvert::QuatToPx(InDesc.InitialWorldRot);

	physx::PxRigidDynamic* body = _Physics->createRigidDynamic(InitialPose);

	body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);

	return DBG_NEW RigidCharacterMovement(InDesc, body);
}

IRigidBodyDynamic* CollDevice::CreateDynamicRigidBody(const RIGID_DYNAMIC_DESC& InDesc)
{
	physx::PxTransform InitialPose;
	InitialPose.p = PxTransformConvert::Vec3ToPx(InDesc.InitialWorldPos);
	InitialPose.q = PxTransformConvert::QuatToPx(InDesc.InitialWorldRot);

	physx::PxRigidDynamic* Actor = _Physics->createRigidDynamic(InitialPose);
	physx::PxRigidBodyExt::updateMassAndInertia(*Actor, InDesc.Mass);

	return DBG_NEW RigidBodyDynamic(InDesc, Actor);
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
