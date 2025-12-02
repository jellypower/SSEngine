#pragma once
#include "SAnimatorBaseComponent.h"


class SSCONTENTBASE_MODULE SSimpleAnimatorTestComponent : public SAnimatorBaseComponent
{
private:
	void UpdateNodesAnimation();

public:
	float GetAnimDuration() const;

	virtual void UpdateAnimation() override;

};

