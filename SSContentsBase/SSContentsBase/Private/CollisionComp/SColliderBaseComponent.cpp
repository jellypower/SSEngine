#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/SColliderBaseComponent.h"

#include <SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h>

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSContentsBase/Public/CollisionComp/RigidBodyComponent/SRigidBodyBaseComponent.h"


void SColliderBaseComponent::PostConstructHierarchy()
{
	ConstructCollInstance();
}

void SColliderBaseComponent::OnEnterTheWorld()
{

}

void SColliderBaseComponent::OnExitTheWorld()
{

}

void SColliderBaseComponent::PreDestructHierarchy()
{
	DestructCollInstance();
}

void SColliderBaseComponent::OnGameObjectTransformCommited(EFramePhase CommitPhase)
{
	if (CommitPhase != EFramePhase::Collision)
	{
		return;
	}

	SGameObject* GO = GetGameObject();
	if (_OwnerRigidBody == nullptr)
	{
		GetCollInstance()->SyncColliderLclTransform_ByContent(GO->GetTransform());
		return;
	}

	SGameObject* RigidGO = _OwnerRigidBody->GetGameObject();
	if (RigidGO == GO)
	{
		Transform LclTransform = RigidGO->GetTransform();
		LclTransform.Position = Vector4f::Zero;
		LclTransform.Rotation = Quaternion();
		GetCollInstance()->SyncColliderLclTransform_ByContent(LclTransform);
	}
	else
	{
		GetCollInstance()->SyncColliderLclTransform_ByContent(GO->GetTransform());
	}
}

void SColliderBaseComponent::SetOffset(const Vector4f& InOffset)
{
	_Offset = InOffset;

	ICollInstanceBase* CI = GetCollInstance();
	if (CI != nullptr)
	{
		CI->SetOffset(InOffset);
	}
}


void SColliderBaseComponent::BindRigidBodyComponent(SRigidBodyBaseComponent* InOwner)
{
	_OwnerRigidBody = InOwner;
}