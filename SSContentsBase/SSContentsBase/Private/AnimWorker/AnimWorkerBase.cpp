#include "AnimWorkerBase.h"

#include "SSContentsBase/Public/AnimComponents/SAnimatorBaseComponent.h"

AnimWorkerBase::AnimWorkerBase():
	_AnimComponents(512, 512)

{
}

void AnimWorkerBase::AddToWorker(SAnimatorBaseComponent* InAnimator)
{
	if (_AnimComponents.Find(InAnimator->GetHashCode()) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_AnimComponents.Add(InAnimator, InAnimator);
}

void AnimWorkerBase::RemoveFromWorker(SAnimatorBaseComponent* InAnimator)
{
	bool bResult = _AnimComponents.Remove(InAnimator);
	SS_ASSERT(bResult);
}

void AnimWorkerBase::PerFrameUpdateAnimation()
{
	for (SS::pair<SObjHashCode, SAnimatorBaseComponent*> AnimatorItemPair : _AnimComponents)
	{
		SAnimatorBaseComponent* AnimatorItem = AnimatorItemPair.second;
		if (AnimatorItem->IsOnPause())
		{
			continue;
		}

		AnimatorItem->UpdateAnimation();
	}
}
