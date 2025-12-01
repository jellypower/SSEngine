#pragma once
#include "SAnimatorBaseComponent.h"


class SSCONTENTBASE_MODULE SSimpleAnimatorTestComponent : public SAnimatorBaseComponent
{
private:
	void UpdateNodesAnimation();

public:
	virtual void UpdateAnimation() override;

};

