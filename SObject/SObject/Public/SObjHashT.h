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


	FORCEINLINE int64 GetNativeValue() const { return _HashCode.GetNativeValue(); }
	FORCEINLINE bool operator!=(const SObjHashT& rhs) const { return _HashCode.GetNativeValue() != rhs._HashCode.GetNativeValue(); }
	FORCEINLINE bool operator==(const SObjHashT& rhs) const { return _HashCode.GetNativeValue() == rhs._HashCode.GetNativeValue(); }
	FORCEINLINE bool operator!=(nullptr_t) const { return _HashCode.GetNativeValue() != SObjHashCode::SOBJ_NATIVEVALUE_NULL; }
	FORCEINLINE bool operator==(nullptr_t) const { return _HashCode.GetNativeValue() == SObjHashCode::SOBJ_NATIVEVALUE_NULL; }

private:
	SObjHashCode _HashCode;
};
