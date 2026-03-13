#include "AnimWorkerBase.h"

#include <SSEngineDefault/Public/RawProfiler/SSFrameInfo.h>

#include "SSContentsBase/Public/AnimComponents/SAnimatorBaseComponent.h"
#include "SSContentsBase/Public/AnimWorker/AnimWorkee/IAnimWorkee.h"

AnimWorkerBase::AnimWorkerBase(SWorld* InWorld) :
	_AnimComponents(512, 512),
	_WorldToAnimate(InWorld)

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

void AnimWorkerBase::BeginUpdateAnimation()
{
	for (SS::pair<SObjHashCode, SAnimatorBaseComponent*> AnimatorItemPair : _AnimComponents)
	{
		SAnimatorBaseComponent* AnimatorItem = AnimatorItemPair.second;
		IAnimWorkee* AnimWorkee = AnimatorItem->GetAnimWorkee();
		if (AnimWorkee == nullptr)
		{
			continue;
		}

		if (AnimWorkee->ShouldUpdateAnim() == false)
		{
			continue;
		}

		// TODO: 나중에 DeltaTime에 TimeScale 적용하기
		float DeltaTime = SSFrameInfo::GetDeltaTime();
		AnimWorkee->UpdateAnimation(DeltaTime);
	}
}

void AnimWorkerBase::EndUpdateAnimation()
{
	uint64 ThisFrameCnt = SSFrameInfo::GetFrameCnt();

	for (SS::pair<SObjHashCode, SAnimatorBaseComponent*> AnimatorItemPair : _AnimComponents)
	{
		SAnimatorBaseComponent* AnimatorItem = AnimatorItemPair.second;
		AnimatorItem->OnAnimWorkerUpdateAnimationEnded();


		IAnimWorkee* AnimWorkee = AnimatorItem->GetAnimWorkee();
		if (AnimWorkee == nullptr)
		{
			continue;
		}


		if (AnimWorkee->GetLastUpdateFrame() != ThisFrameCnt) 
		{
			continue; // 애니메이션이 업데이트 된 적이 없으면 트랜스폼을 적용하지 않습니다.
		}

		AnimatorItem->ApplyAnimWorkeeTransform();
	}
}
