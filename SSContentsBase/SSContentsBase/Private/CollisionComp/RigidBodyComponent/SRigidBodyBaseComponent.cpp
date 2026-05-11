#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SRigidBodyBaseComponent.h"

#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"

#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"

#include "SSContentsBase/Public/CollisionComp/SColliderBaseComponent.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

void SRigidBodyBaseComponent::PostConstructHierarchy()
{
	ConstructRigidBodyInstance();
}

void SRigidBodyBaseComponent::OnEnterTheWorld()
{
	if (GetGameObject()->IsRootInWorld() == false)
	{
		SS_INTERRUPT("RIgid body be must placed in Root.");
		return;
	}


	SWorld* IncludedWorld = GetIncludedWorld();
	ICollisionWorld* CollWorld = IncludedWorld->GetCollWorld();

	IRigidBodyBase* RI = GetRigidBodyInstance();
	ICollInstanceBase* BoundICI = RI->GetCollInstance();
	if (BoundICI == nullptr)
	{
		RI->BindCollisionInstance(_ColliderComp->GetCollInstance());
	}


	CollWorld->AddToWorld(RI);
}

void SRigidBodyBaseComponent::OnExitTheWorld()
{
	if (GetGameObject()->IsRootInWorld() == false)
	{
		SS_INTERRUPT("RIgid body be must placed in Root.");
		return;
	}

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
	IRigidBodyBase* IRI = GetRigidBodyInstance();

	if (ICI != nullptr && IRI != nullptr)
	{
		IRI->BindCollisionInstance(ICI);
	}
}

void SRigidBodyBaseComponent::OnGameObjectTransformCommited(EFramePhase CommitPhase)
{
	if (CommitPhase != EFramePhase::Collision)
	{
		return;
	}

	IRigidBodyBase* IRI = GetRigidBodyInstance();
	if (IRI == nullptr)
	{
		return;
	}

	const SGameObject* GO = GetGameObject();
	if (GO == nullptr)
	{
		return;
	}

	IRI->SetSimulBeginPosAndRot(
		GO->GetCommittedWorldTransformMat().r[3],
		GO->GetCommittedWorldRotation());
}