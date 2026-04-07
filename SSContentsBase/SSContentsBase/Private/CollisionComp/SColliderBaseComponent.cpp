#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/SColliderBaseComponent.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"


void SColliderBaseComponent::PostConstructHierarchy()
{
	ConstructCollInstance();
}

void SColliderBaseComponent::OnEnterTheWorld()
{
	SWorld* IncludedWorld = GetIncludedWorld();
	ICollisionWorld* CollWorld = IncludedWorld->GetCollWorld();

	CollWorld->AddToWorld(GetCollInstance());
}

void SColliderBaseComponent::OnExitTheWorld()
{
	SWorld* IncludedWorld = GetIncludedWorld();
	ICollisionWorld* CollWorld = IncludedWorld->GetCollWorld();

	ICollInstanceBase* CollInstance = GetCollInstance();
	SObjHashCode GOID = CollInstance->GetGameObjectID();
	CollWorld->RemoveFromWorld(GOID);
}

void SColliderBaseComponent::PreDestructHierarchy()
{
	DestructCollInstance();
}

void SColliderBaseComponent::OnGameObjectTransformCommited()
{
	SGameObject* Owner = GetGameObject();
	GetCollInstance()->SetWorldTransform(
		Owner->GetCommittedWorldTransformMat(),
		Owner->GetCommittedWorldRotation());
}
