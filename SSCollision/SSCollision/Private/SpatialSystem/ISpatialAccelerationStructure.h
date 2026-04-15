#pragma once

class ICollInstanceBase;


enum class ESASType
{
	None,

	SweepAndPrune
};


class ISpatialAccelerationStructure : public INoncopyable
{
public:
	virtual ESASType GetSASType() const = 0;

	virtual bool IsAnyInstanceExists() const = 0;
	virtual void AddCollInstance(ICollInstanceBase* InCollInstance) = 0;
	virtual void RemoveCollInstance(ICollInstanceBase* InCollInstance) = 0;
	virtual void FlushPendingInstances() = 0;


	virtual void QueryCollidableWith(SS::PooledList<ICollInstanceBase*>& OutList, ICollInstanceBase* CollTarget) const = 0;
};
