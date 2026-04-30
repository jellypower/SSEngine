#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SRigidBodyBaseComponent.h"

#include <SSCollision/Public/CollisionBase/ICollisionWorld.h>
#include <SSCollision/Public/RigidBody/IRigidBodyBase.h>

#include "SSContentsBase/Public/CollisionComp/SColliderBaseComponent.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

void SRigidBodyBaseComponent::PostConstructHierarchy()
{
	ConstructRigidBodyInstance();
}

void SRigidBodyBaseComponent::OnEnterTheWorld()
{
	SWorld* IncludedWorld = GetIncludedWorld();
	ICollisionWorld* CollWorld = IncludedWorld->GetCollWorld();

	IRigidBodyBase* RI = GetRigidBodyInstance();
	ICollInstanceBase* BoundICI = GetRigidBodyInstance()->GetCollInstance();
	if (BoundICI == nullptr)
	{
		RI->BindCollisionInstance(_ColliderComp->GetCollInstance());
	}


	CollWorld->AddToWorld(GetRigidBodyInstance());
}

void SRigidBodyBaseComponent::OnExitTheWorld()
{
	SWorld* IncludedWorld = GetIncludedWorld();
	ICollisionWorld* CollWorld = IncludedWorld->GetCollWorld();

	IRigidBodyBase* RigidInstance = GetRigidBodyInstance();
	CollWorld->RemoveRigidFromWorld(RigidInstance);
}

void SRigidBodyBaseComponent::PreDestructHierarchy()
{
	DestructRigidBodyInstance();
}

void SRigidBodyBaseComponent::BindColliderComponent(SColliderBaseComponent* InCollider)
{
	_ColliderComp = InCollider;
	ICollInstanceBase* ICI = _ColliderComp->GetCollInstance();
	IRigidBodyBase* IRI =GetRigidBodyInstance();

	if (ICI != nullptr && IRI != nullptr)
	{
		IRI->BindCollisionInstance(ICI);
	}
}

void SRigidBodyBaseComponent::OnGameObjectTransformCommited(EFramePhase CommitPhase)
{
	
}
