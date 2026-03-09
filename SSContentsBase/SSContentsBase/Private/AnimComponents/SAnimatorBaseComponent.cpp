#include "SSContentsBase/Public/AnimComponents/SAnimatorBaseComponent.h"

#include <SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h>


#include "SSContentsBase/Private/AnimWorker/AnimWorkee/AnimWorkeeSimplePlayer.h"
#include "SSContentsBase/Public/AnimWorker/IAnimWorker.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"




SAnimatorBaseComponent::SAnimatorBaseComponent() 
{
}

void SAnimatorBaseComponent::ReconstructBoneBinding()
{
	SCOPE_PROFILE(SAnimatorBaseComponent::ReconstructBoneBinding);
	SGameObject* GO = GetGameObject();

	if (GO == nullptr || GO->GetIsHierarchyInitialized() == false)
	{
		return;
	}


	SS::PooledList<SGameObject*> ScrapedDecendants(400);
	GO->ScrapAllDescendants(ScrapedDecendants);

	_BoneBindings.Reserve(ScrapedDecendants.GetSize());
	
	for (SGameObject* Item : ScrapedDecendants)
	{
		_BoneBindings.PushBack(Item);
	}


	int32 a = 0;
}

void SAnimatorBaseComponent::OnEnterTheWorld()
{
	SGameObject* OwnerGameObject = GetGameObject();
	SWorld* IncludedWorld = OwnerGameObject->GetIncludedWorldRef();
	IAnimWorker* Worker = IncludedWorld->GetAnimWorker();

	Worker->AddToWorker(this);
}

void SAnimatorBaseComponent::PostConstructHierarchy()
{
	ReconstructBoneBinding();
}



void SAnimatorBaseComponent::ApplyAnimWorkeeTransform()
{
	IAnimWorkee* AnimWorkee = GetAnimWorkee();
	if (AnimWorkee == nullptr)
	{
		return;
	}

	const PoseSlot& ResultPose = AnimWorkee->GetResultPose();

	int32 BoneCnt = _BoneBindings.GetSize();
	for (int32 i = 0; i < BoneCnt; i++)
	{
		SGameObject* GOItem = _BoneBindings[i].Get();
		if (GOItem == nullptr)
		{
			continue;
		}

		GOItem->SetTransform(ResultPose.BoneTransforms[i]);
	}
}

void SAnimatorBaseComponent::OnExitTheWorld()
{
	SGameObject* OwnerGameObject = GetGameObject();
	SWorld* IncludedWorld = OwnerGameObject->GetIncludedWorldRef();
	IAnimWorker* Worker = IncludedWorld->GetAnimWorker();

	Worker->RemoveFromWorker(this);
}
