#pragma once
#include "SObjHashCode.h"


class SObjectBase : public INoncopyable
{
public:
	SObjHashCode _HashCode = nullptr;
	SS::SHasherW _ObjectName;

public:
	virtual ~SObjectBase();


public:
	SObjHashCode GetHashCode() const { return _HashCode; }
	SS::SHasherW GetObjectName() const { return _ObjectName; }

	virtual void PostConstruct() { }
	virtual void PreDestruct() { }

};

