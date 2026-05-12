#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SRigidBodyStaticComponent.h"

#include "SSCollision/Public/CollisionBase/ICollDevice.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"

#include "SSCollision/Public/RigidBody/RigidCreationDesc.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"


IRigidBodyBase* SRigidBodyStaticComponent::GetRigidBodyInstance() const
{
	return _RigidBodyStatic;
}

void SRigidBodyStaticComponent::PostCollision_SyncTransform()
{
	SS_ASSERT_MSG(false, L"Static bodies are never moved by physics simulation");
}

void SRigidBodyStaticComponent::ConstructRigidBodyInstance()
{
	const SGameObject* GO = GetGameObject();

	RIGID_STATIC_DESC Desc;
	Desc.ComponentID = GetHashCode();
	Desc.InitialWorldPos = GO->GetTransform().Position;
	Desc.InitialWorldRot = GO->GetTransform().Rotation;

	_RigidBodyStatic = g_CollDevice->CreateStaticRigidBody(Desc);
}

void SRigidBodyStaticComponent::DestructRigidBodyInstance()
{
	delete _RigidBodyStatic;
	_RigidBodyStatic = nullptr;
}
