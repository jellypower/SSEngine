#include "pch.h"
#include "RigidBodyStatic.h"

#include "SSCollision/Private/CollInstance/CIUtils_Private.h"
#include "SSCollision/Public/RigidBody/RigidCreationDesc.h"


RigidBodyStatic::RigidBodyStatic(const RIGID_STATIC_DESC& InDesc, physx::PxRigidStatic* InActor)
{
	_ComponentID = InDesc.ComponentID;
	_SimulBeginPos = InDesc.InitialWorldPos;
	_SimulBeginRot = InDesc.InitialWorldRot;
	_PxActor = InActor;
}

RigidBodyStatic::~RigidBodyStatic()
{
	PX_RELEASE(_PxActor);
}

ERigidBodyType RigidBodyStatic::GetRigidBodyType() const
{
	return ERigidBodyType::Static;
}

bool RigidBodyStatic::IsTransformModifiedOnThisTick() const
{
	return false;
}

void RigidBodyStatic::SetSimulBeginPosAndRot_ByContent(const Vector4f& InPos, const Quaternion& InRot)
{
	_SimulBeginPos = InPos;
	_SimulBeginRot = InRot;

	physx::PxTransform Pose;
	Pose.p = PxTransformConvert::Vec3ToPx(InPos);
	Pose.q = PxTransformConvert::QuatToPx(InRot);
	_PxActor->setGlobalPose(Pose);
}

const Vector4f& RigidBodyStatic::GetSimulBeginPos() const
{
	return _SimulBeginPos;
}

const Quaternion& RigidBodyStatic::GetSimulBeginRot() const
{
	return _SimulBeginRot;
}

void RigidBodyStatic::OnBeginSimulation()
{
}

void RigidBodyStatic::OnEndSimulation()
{
}

ICollInstanceBase* RigidBodyStatic::GetCollInstance() const
{
	return _CollInstance;
}

void RigidBodyStatic::BindCollisionInstance(ICollInstanceBase* BoundCI)
{
	if (_CollInstance != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_CollInstance = BoundCI;
	physx::PxShape* Shape = ExtractPxShape(_CollInstance);
	_PxActor->attachShape(*Shape);
}

void RigidBodyStatic::DetachCollInstance(ICollInstanceBase* BoundCI)
{
	if (_CollInstance != BoundCI)
	{
		SS_ASSERT(false);
		return;
	}

	physx::PxShape* Shape = ExtractPxShape(BoundCI);
	_PxActor->detachShape(*Shape);
	_CollInstance = nullptr;
}

SObjHashCode RigidBodyStatic::GetGameObjectID() const
{
	return _ComponentID;
}

ICollisionWorld* RigidBodyStatic::GetIncludedCollWorld() const
{
	return _IncludedCollWorld;
}

void RigidBodyStatic::OnEnterTheCollWorld(ICollisionWorld* InCollWorld)
{
	_IncludedCollWorld = InCollWorld;
}

void RigidBodyStatic::OnExitFromCollWorld()
{
	_IncludedCollWorld = nullptr;
}
