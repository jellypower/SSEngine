#pragma once
#include "ISpatialAccelerationStructure.h"

struct SAPElement
{
	int32 ProxyIdx;
	bool bStart;
};

struct CIProxy
{
	ICollInstanceBase* Coll;
	int32 SAPListStartIdx;
	int32 SAPListEndIdx;
};


class SASSweepAndPrune : public ISpatialAccelerationStructure
{
private:
	SS::PooledList<SAPElement> _SAPList;
	SS::PooledList<CIProxy> _CIProxies;
	SS::PooledList<ICollInstanceBase*> _EnterPendingItems;
	SS::PooledList<ICollInstanceBase*> _ExitPendingItems;

public:
	SASSweepAndPrune();

public:
	ESASType GetSASType() const override;

	bool IsAnyInstanceExists() const override;
	void AddCollInstance(ICollInstanceBase* InCollInstance) override;
	void RemoveCollInstance(ICollInstanceBase* InCollInstance) override;
	void FlushPendingInstances() override;

	void QueryCollidableWith(SS::PooledList<ICollInstanceBase*>& OutList, ICollInstanceBase* CollTarget) const override;


public:
	void UpdateSAPStructure();

private:
	void RemoveFromSAPList(ICollInstanceBase* ICIToRemove);
	void AddToSAPList(ICollInstanceBase* ICIToAdd);

	float ExtractSAPElementAxisValue(SAPElement InElement) const;
};
