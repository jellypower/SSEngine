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
	virtual void FinalizePendingInstances() = 0; // 종료시

	virtual ESASType GetSASType() const = 0;

	virtual bool IsAnyInstanceExists() const = 0;
	virtual void AddCollInstance(ICollInstanceBase* InCollInstance) = 0;
	virtual void RemoveCollInstance(ICollInstanceBase* InCollInstance) = 0;
	virtual void AddUpdateNeededCollInstance(ICollInstanceBase* InCollInstance) = 0;


	virtual void QueryCollidableWith(SS::PooledList<ICollInstanceBase*>& OutList, ICollInstanceBase* CollTarget) const = 0;
};
