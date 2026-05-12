#pragma once
#include "IRigidBodyBase.h"

class IRigidbodySim : public IRigidBodyBase
{
public:
	virtual const Vector4f& GetSimulEndPos() const = 0;
	virtual Vector4f CalcPosDelta() const = 0;

	virtual const Quaternion& GetSimulEndRot() const = 0;
	virtual Quaternion CalcRotDelta() const = 0;

};
