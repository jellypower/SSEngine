#pragma once


enum class ERigidBodyType
{
	None,

	CharacterMovement,
};


class ICollisionWorld;
class ICollInstanceBase;

class IRigidBodyBase : public INoncopyable
{
public:
	virtual ERigidBodyType GetRigidBodyType() const = 0;

	virtual bool IsMovedOnThisTick() const = 0;
	virtual bool IsRotatedOnThisTick() const = 0;
	virtual Vector4f GetSimulatedPosDelta() const = 0;
	virtual Quaternion GetSimulatedRotDelta() const = 0;

	virtual void SimulateTick(float DeltaTime) = 0;
	virtual void OnEndSimulation() = 0;

	virtual ICollInstanceBase* GetCollInstance() const = 0;
	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) = 0;

	virtual void OnEnterTheCollWorld(ICollisionWorld* InRenderWorld) = 0;
	virtual void OnExitFromCollWorld() = 0;
};
