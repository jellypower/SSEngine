#pragma once
#include "SObjectBase.h"
#include "SObjHashCode.h"

template<typename T>
class SObjHashT
{
public:
	friend bool operator!= <> (SObjHashT<T> lhs, SObjHashT<T> rhs);
	friend bool operator== <> (SObjHashT<T> lhs, SObjHashT<T> rhs);
	friend bool operator!= <> (SObjHashT<T> lhs, nullptr_t);
	friend bool operator== <> (SObjHashT<T> lhs, nullptr_t);


private:
	SObjHashCode _HashCode;

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
};


template<typename T> inline bool operator!=(SObjHashT<T> lhs, SObjHashT<T> rhs) { return lhs._HashCode.GetNativeValue() != rhs._HashCode.GetNativeValue(); }
template<typename T> inline bool operator==(SObjHashT<T> lhs, SObjHashT<T> rhs) { return lhs._HashCode.GetNativeValue() == rhs._HashCode.GetNativeValue(); }
template<typename T> inline bool operator!=(SObjHashT<T> lhs, nullptr_t) { return lhs._HashCode.GetNativeValue() != SObjHashCode::SOBJ_NATIVEVALUE_NULL; }
template<typename T> inline bool operator==(SObjHashT<T> lhs, nullptr_t) { return lhs._HashCode.GetNativeValue() == SObjHashCode::SOBJ_NATIVEVALUE_NULL; }