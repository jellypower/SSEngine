#pragma once
#include "SObjHashCode.h"


class SObjectBase : public INoncopyable
{
private:
	SObjHashCode _HashCode = nullptr;
	SS::SHasherW _ObjectName;


public:
	void SetHashCodeXXX(SObjHashCode InHashCode) { _HashCode = InHashCode; }
	void SetObjectNameXXX(SS::SHasherW InObjectName) { _ObjectName = InObjectName; }

public:
	virtual void PostConstruct() { }
	virtual void PreDestruct() { }
};

