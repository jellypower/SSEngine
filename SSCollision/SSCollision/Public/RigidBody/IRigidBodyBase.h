#pragma once

class ICollInstanceBase;

class IRigidBodyBase : public INoncopyable
{
public:
	virtual void SimulateTick(float DeltaTime) = 0;
	virtual Vector4f GetSimulatedPos() const = 0;
	virtual Quaternion GetSimulatedRot() const = 0;

	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) = 0;
};
