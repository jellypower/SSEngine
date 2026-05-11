#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/SColliderBaseComponent.h"

#include <SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h>

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
	SGameObject* Owner = GetGameObject();

	if (Owner->IsTransformCommitReserved())
	{
		GetCollInstance()->SyncColliderTransform_ByContent(Owner->GetTransform());
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
