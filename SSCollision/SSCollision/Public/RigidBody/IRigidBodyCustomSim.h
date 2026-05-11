#pragma once
#include "IRigidbodySim.h"

class IRigidBodyCustomSim : public IRigidbodySim
{
public:
	virtual void SimulateMovement(float DeltaTime) = 0;
};
