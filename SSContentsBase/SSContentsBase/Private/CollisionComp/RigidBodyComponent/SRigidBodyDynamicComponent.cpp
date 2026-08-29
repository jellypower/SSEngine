#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SRigidBodyDynamicComponent.h"

#include "SSCollision/Public/CollisionBase/ICollDevice.h"
#include "SSCollision/Public/RigidBody/IRigidBodyDynamic.h"

#include "SSCollision/Public/RigidBody/RigidCreationDesc.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"


void SRigidBodyDynamicComponent::SetMass(float InMass)
{
	_Mass = InMass;
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->SetMass(InMass);
	}
}

void SRigidBodyDynamicComponent::SetLinearDamping(float Damping)
{
	_LinearDamping = Damping;
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->SetLinearDamping(Damping);
	}
}

void SRigidBodyDynamicComponent::SetAngularDamping(float Damping)
{
	_AngularDamping = Damping;
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->SetAngularDamping(Damping);
	}
}

void SRigidBodyDynamicComponent::SetGravityEnabled(bool bEnable)
{
	_bGravityEnabled = bEnable;
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->SetGravityEnabled(bEnable);
	}
}

void SRigidBodyDynamicComponent::SetKinematic(bool bKinematic)
{
	_bKinematic = bKinematic;
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->SetKinematic(bKinematic);
	}
}

void SRigidBodyDynamicComponent::AddForce(const Vector4f& Force)
{
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->AddForce(Force);
	}
}

void SRigidBodyDynamicComponent::AddImpulse(const Vector4f& Impulse)
{
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->AddImpulse(Impulse);
	}
}

void SRigidBodyDynamicComponent::ClearForce()
{
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->ClearForce();
	}
}

Vector4f SRigidBodyDynamicComponent::GetLinearVelocity() const
{
	if (_RigidBodyDynamic != nullptr)
	{
		return _RigidBodyDynamic->GetLinearVelocity();
	}
	return Vector4f::Zero;
}

void SRigidBodyDynamicComponent::SetLinearVelocity(const Vector4f& Velocity)
{
	if (_RigidBodyDynamic != nullptr)
	{
		_RigidBodyDynamic->SetLinearVelocity(Velocity);
	}
}

IRigidBodyBase* SRigidBodyDynamicComponent::GetRigidBodyInstance() const
{
	return _RigidBodyDynamic;
}

void SRigidBodyDynamicComponent::PostCollision_SyncTransform()
{
	SGameObject* GO = GetGameObject();
	Transform NewTransform = GO->GetTransform();
	NewTransform.Position = _RigidBodyDynamic->GetSimulEndPos();
	NewTransform.Rotation = _RigidBodyDynamic->GetSimulEndRot();
	GO->SetTransform(NewTransform);
}

void SRigidBodyDynamicComponent::ConstructRigidBodyInstance()
{
	const SGameObject* GO = GetGameObject();

	RIGID_DYNAMIC_DESC Desc;
	Desc.ComponentID = GetHashCode();
	Desc.InitialWorldPos = GO->GetTransform().Position;
	Desc.InitialWorldRot = GO->GetTransform().Rotation;
	Desc.Mass = _Mass;
	Desc.LinearDamping = _LinearDamping;
	Desc.AngularDamping = _AngularDamping;
	Desc.bGravityEnabled = _bGravityEnabled;
	Desc.bKinematic = _bKinematic;

	_RigidBodyDynamic = g_CollDevice->CreateDynamicRigidBody(Desc);
}

void SRigidBodyDynamicComponent::DestructRigidBodyInstance()
{
	_RigidBodyDynamic->Release();
	_RigidBodyDynamic = nullptr;
}
