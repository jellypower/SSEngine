#pragma once


enum class ERigidBodyType
{
	None,

	Static,
	Dynamic,
	CharacterMovement,
};


class ICollisionWorld;
class ICollInstanceBase;

class IRigidBodyBase : public INoncopyable
{
public:
	virtual ERigidBodyType GetRigidBodyType() const = 0;

	virtual bool IsTransformModifiedOnThisTick() const = 0;

	// Transform
	virtual void SetSimulBeginPosAndRot(const Vector4f& InPos, const Quaternion& InRot) = 0;
	virtual const Vector4f& GetSimulBeginPos() const = 0;
	virtual const Quaternion& GetSimulBeginRot() const = 0;
	virtual void OnBeginSimulation() = 0;
	virtual void OnEndSimulation() = 0;

	virtual ICollInstanceBase* GetCollInstance() const = 0;
	virtual void BindCollisionInstance(ICollInstanceBase* BoundCI) = 0;
	virtual void DetachCollInstance(ICollInstanceBase* BoundCI) = 0;

	virtual SObjHashCode GetGameObjectID() const = 0;
	virtual ICollisionWorld* GetIncludedCollWorld() const = 0;
	virtual void OnEnterTheCollWorld(ICollisionWorld* InRenderWorld) = 0;
	virtual void OnExitFromCollWorld() = 0;
};
