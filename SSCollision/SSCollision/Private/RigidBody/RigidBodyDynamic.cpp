#include "pch.h"
#include "RigidBodyDynamic.h"

#include "SSCollision/Private/CollInstance/CIUtils_Private.h"
#include "SSCollision/Public/RigidBody/RigidCreationDesc.h"


RigidBodyDynamic::RigidBodyDynamic(const RIGID_DYNAMIC_DESC& InDesc, physx::PxRigidDynamic* InActor)
{
	_ComponentID = InDesc.ComponentID;
	_PxActor = InActor;

	_PxActor->setMass(InDesc.Mass);
	_PxActor->setLinearDamping(InDesc.LinearDamping);
	_PxActor->setAngularDamping(InDesc.AngularDamping);
	_PxActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !InDesc.bGravityEnabled);
	_PxActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, InDesc.bKinematic);
}

ERigidBodyType RigidBodyDynamic::GetRigidBodyType() const
{
	return ERigidBodyType::Dynamic;
}

bool RigidBodyDynamic::IsTransformModifiedOnThisTick() const
{
	return _bTransformModifiedOnThisTick;
}

bool RigidBodyDynamic::IsMovedOnThisSimulation() const
{
	return _bMovedOnThisSimulation;
}

bool RigidBodyDynamic::IsRotatedOnThisSimulation() const
{
	return _bRotatedOnThisSimulation;
}

void RigidBodyDynamic::SetSimulBeginPosAndRot(const Vector4f& InPos, const Quaternion& InRot)
{
	_SimulBeginPos = InPos;
	_SimulBeginRot = InRot;

	physx::PxTransform Pose;
	Pose.p = { InPos.X, InPos.Y, InPos.Z };
	Pose.q = { InRot.X, InRot.Y, InRot.Z, InRot.W };
	_PxActor->setGlobalPose(Pose);
}

const Vector4f& RigidBodyDynamic::GetSimulBeginPos() const
{
	return _SimulBeginPos;
}

const Vector4f& RigidBodyDynamic::GetSimulEndPos() const
{
	return _SimulEndPos;
}

Vector4f RigidBodyDynamic::CalcPosDelta() const
{
	return _SimulEndPos - _SimulBeginPos;
}

const Quaternion& RigidBodyDynamic::GetSimulBeginRot() const
{
	return _SimulBeginRot;
}

const Quaternion& RigidBodyDynamic::GetSimulEndRot() const
{
	return _SimulEndRot;
}

Quaternion RigidBodyDynamic::CalcRotDelta() const
{
	return _SimulBeginRot.Inverse() * _SimulEndRot;
}

void RigidBodyDynamic::OnBeginSimulation()
{
	_bTransformModifiedOnThisTick = false;
	_SimulBeginPos = _SimulEndPos;
	_SimulBeginRot = _SimulEndRot;
}

void RigidBodyDynamic::OnEndSimulation()
{
	physx::PxTransform Pose = _PxActor->getGlobalPose();

	_SimulEndPos = { Pose.p.x, Pose.p.y, Pose.p.z, 1 };
	_SimulEndRot = Quaternion(XMVectorSet(Pose.q.x, Pose.q.y, Pose.q.z, Pose.q.w));

	Vector4f PosDelta = _SimulEndPos - _SimulBeginPos;
	_bMovedOnThisSimulation =
		XMVectorGetX(XMVector3LengthSq(PosDelta.SimdVec)) > 0.0001f;

	Quaternion RotDelta = CalcRotDelta();
	_bRotatedOnThisSimulation =
		XMVectorGetX(XMVector3LengthSq(RotDelta.SimdVec)) > 0.0001f;

	_bTransformModifiedOnThisTick = _bMovedOnThisSimulation || _bRotatedOnThisSimulation;
}

ICollInstanceBase* RigidBodyDynamic::GetCollInstance() const
{
	return _CollInstance;
}

void RigidBodyDynamic::BindCollisionInstance(ICollInstanceBase* BoundCI)
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

void RigidBodyDynamic::DetachCollInstance(ICollInstanceBase* BoundCI)
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

SObjHashCode RigidBodyDynamic::GetGameObjectID() const
{
	return _ComponentID;
}

ICollisionWorld* RigidBodyDynamic::GetIncludedCollWorld() const
{
	return _IncludedCollWorld;
}

void RigidBodyDynamic::OnEnterTheCollWorld(ICollisionWorld* InCollWorld)
{
	_IncludedCollWorld = InCollWorld;
}

void RigidBodyDynamic::OnExitFromCollWorld()
{
	_IncludedCollWorld = nullptr;
}

void RigidBodyDynamic::AddForce(const Vector4f& Force)
{
	_PxActor->addForce({ Force.X, Force.Y, Force.Z });
}

void RigidBodyDynamic::AddImpulse(const Vector4f& Impulse)
{
	_PxActor->addForce({ Impulse.X, Impulse.Y, Impulse.Z }, physx::PxForceMode::eIMPULSE);
}

void RigidBodyDynamic::ClearForce()
{
	_PxActor->clearForce();
}

Vector4f RigidBodyDynamic::GetLinearVelocity() const
{
	physx::PxVec3 V = _PxActor->getLinearVelocity();
	return { V.x, V.y, V.z, 0 };
}

void RigidBodyDynamic::SetLinearVelocity(const Vector4f& Velocity)
{
	_PxActor->setLinearVelocity({ Velocity.X, Velocity.Y, Velocity.Z });
}

Vector4f RigidBodyDynamic::GetAngularVelocity() const
{
	physx::PxVec3 V = _PxActor->getAngularVelocity();
	return { V.x, V.y, V.z, 0 };
}

void RigidBodyDynamic::SetAngularVelocity(const Vector4f& Velocity)
{
	_PxActor->setAngularVelocity({ Velocity.X, Velocity.Y, Velocity.Z });
}

float RigidBodyDynamic::GetMass() const
{
	return _PxActor->getMass();
}

void RigidBodyDynamic::SetMass(float InMass)
{
	_PxActor->setMass(InMass);
}

float RigidBodyDynamic::GetLinearDamping() const
{
	return _PxActor->getLinearDamping();
}

void RigidBodyDynamic::SetLinearDamping(float Damping)
{
	_PxActor->setLinearDamping(Damping);
}

float RigidBodyDynamic::GetAngularDamping() const
{
	return _PxActor->getAngularDamping();
}

void RigidBodyDynamic::SetAngularDamping(float Damping)
{
	_PxActor->setAngularDamping(Damping);
}

bool RigidBodyDynamic::IsGravityEnabled() const
{
	return !(_PxActor->getActorFlags() & physx::PxActorFlag::eDISABLE_GRAVITY);
}

void RigidBodyDynamic::SetGravityEnabled(bool bEnable)
{
	_PxActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !bEnable);
}

bool RigidBodyDynamic::IsKinematic() const
{
	return !!(_PxActor->getRigidBodyFlags() & physx::PxRigidBodyFlag::eKINEMATIC);
}

void RigidBodyDynamic::SetKinematic(bool bKinematic)
{
	_PxActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, bKinematic);
}

void RigidBodyDynamic::SetKinematicTarget(const Vector4f& Pos, const Quaternion& Rot)
{
	physx::PxVec3 PxPos(Pos.X, Pos.Y, Pos.Z);
	physx::PxQuat PxRot(Rot.X, Rot.Y, Rot.Z, Rot.W);
	_PxActor->setKinematicTarget(physx::PxTransform(PxPos, PxRot));
}
