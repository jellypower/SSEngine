#pragma once

#include "SSEngineDefault/Public/SSContainer/HashMap.h"

#include "SObject/Public/SObjHashCode.h"

#include "SSContentsBase/Public/AnimWorker/IAnimWorker.h"

class SAnimatorBaseComponent;

class AnimWorkerBase : public IAnimWorker
{
private:
	SS::HashMap<SObjHashCode, SAnimatorBaseComponent*> _AnimComponents;

public:
	AnimWorkerBase();

public:
	virtual void AddToWorker(SAnimatorBaseComponent* InAnimator) override;
	virtual void RemoveFromWorker(SAnimatorBaseComponent* InAnimator) override;
	virtual void PerFrameUpdateAnimation() override;
};

