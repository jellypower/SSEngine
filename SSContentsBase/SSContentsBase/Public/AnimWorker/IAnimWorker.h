#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class SAnimatorBaseComponent;

class IAnimWorker : public INoncopyable
{
	// TODO: 나중에 SWorld_Impl 에 들어가도록 클래스 분리 가능할듯


public:
	virtual void AddToWorker(SAnimatorBaseComponent* InAnimator) = 0;
	virtual void RemoveFromWorker(SAnimatorBaseComponent* InAnimator) = 0;

	virtual void BeginUpdateAnimation(float DeltaTime) = 0;
	virtual void EndUpdateAnimation() = 0;
};
