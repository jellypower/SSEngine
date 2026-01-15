#pragma once
#include "SObjectBase.h"
#include "SObjHashCode.h"

template<typename T>
class SObjHashT
{
public:
	SObjHashT() : _HashCode(nullptr) { }

	SObjHashT(SObjectBase* InSObject)
	{
		_HashCode = InSObject;
	}

public:
	T* Get() const
	{
		SObjectBase* SObject = _HashCode.GetSObject();
		return static_cast<T*>(SObject);
	}

	SObjHashCode GetHashCode() const
	{
		return _HashCode;
	}

private:
	SObjHashCode _HashCode;
};
