#pragma once

#include "SSEngineDefault/Public/SSContainer/HashMap.h"

#include "SObject/Public/SObjHashCode.h"

#include "SSContentsBase/Public/AnimWorker/IAnimWorker.h"

class SWorld;
class SAnimatorBaseComponent;

class AnimWorkerBase : public IAnimWorker
{
private:
	SS::HashMap<SObjHashCode, SAnimatorBaseComponent*> _AnimComponents;
	SWorld* _WorldToAnimate = nullptr;

public:
	AnimWorkerBase(SWorld* InWorld);
	void Release() override;

public:
	virtual void AddToWorker(SAnimatorBaseComponent* InAnimator) override;
	virtual void RemoveFromWorker(SAnimatorBaseComponent* InAnimator) override;

	virtual void BeginUpdateAnimation(float DeltaTime) override;
	virtual void EndUpdateAnimation() override;

};

