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
	SGameObject* GO = GetGameObject();
	if (GO->IsRootInWorld() == false)
	{
		SS_INTERRUPT("RIgid body be must placed in Root.");
		return;
	}

	SWorld* IncludedWorld = GetIncludedWorld();
	ICollisionWorld* CollWorld = IncludedWorld->GetCollWorld();


	SColliderBaseComponent* FoundCollider = GO->FindComponent<SColliderBaseComponent>();
	if (FoundCollider == nullptr)
	{
		int32 ChildCnt = GO->GetChildCnt();
		for (int32 i=0;i<ChildCnt;i++)
		{
			SGameObject* ChildGO = GO->GetChild(i);
			FoundCollider = ChildGO->FindComponent<SColliderBaseComponent>();
			if (FoundCollider != nullptr)
			{
				break;
			}
		}
	}

	if (FoundCollider == nullptr)
	{
		SS_ASSERT(false, L"Cannot work without Collider");
		return;
	}

	BindColliderComponent(FoundCollider);
	CollWorld->AddToWorld(GetRigidBodyInstance());
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

void SRigidBodyBaseComponent::BindColliderComponent(SColliderBaseComponent* InCollider)
{
	if (_ColliderComp != nullptr)
	{
		SS_ASSERT(false, L"TODO: Implement multiple Colliders");
		return;
	}

	_ColliderComp = InCollider;
	_ColliderComp->BindRigidBodyComponent(this);

	ICollInstanceBase* ICI = _ColliderComp->GetCollInstance();
	IRigidBodyBase* IRI = GetRigidBodyInstance();
	if (ICI != nullptr && IRI != nullptr)
	{
		IRI->BindCollisionInstance(ICI);
	}
}
