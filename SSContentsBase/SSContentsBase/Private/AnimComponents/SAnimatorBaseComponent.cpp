#include "SSContentsBase/Public/AnimComponents/SAnimatorBaseComponent.h"


#include "SSContentsBase/Private/AnimWorker/AnimWorkee/AnimWorkeeSimplePlayer.h"
#include "SSContentsBase/Public/AnimWorker/IAnimWorker.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"




void SAnimatorBaseComponent::ReconstructBoneBinding()
{
	SGameObject* GO = GetGameObject();

	if (GO == nullptr || GO->GetIsHierarchyInitialized() == false)
	{
		return;
	}


	SS::PooledList<SGameObject*> ScrapedDecendants(200);
	GO->ScrapAllDescendants(ScrapedDecendants);
	_BoneBindings.Clear();
	_BoneBindings.Reserve(200);

	SGameObject* MatchingObject = nullptr;

	for (SGameObject* Item : ScrapedDecendants)
	{
		_BoneBindings.PushBack(Item); // nullptr도 가능
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
