#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SObject/ModuleExportKeyword.h"

class SObjectBase;

class SOBJECT_MODULE SObjHashCode
{
public:
	static constexpr int64 SOBJ_NATIVEVALUE_NULL = 0;

	friend bool operator!=(SObjHashCode lhs, SObjHashCode rhs);
	friend bool operator==(SObjHashCode lhs, SObjHashCode rhs);
	friend bool operator!=(SObjHashCode lhs, nullptr_t);
	friend bool operator==(SObjHashCode lhs, nullptr_t);
	friend int32 HashValue(SObjHashCode InValue);
	friend SS::StringW ToStringW(SObjHashCode InCode);

private:
	int64 _NativeValue = SOBJ_NATIVEVALUE_NULL;

public:
	SObjHashCode() { }
	SObjHashCode(nullptr_t) { }
	explicit SObjHashCode(int64 InNativeValue) : _NativeValue(InNativeValue) { }
	SObjHashCode(SObjectBase* InObject);

public:
	FORCEINLINE int64 GetNativeValue() const { return _NativeValue; }
	SObjectBase* GetSObject() const;
};


inline bool operator!=(SObjHashCode lhs, SObjHashCode rhs) { return lhs._NativeValue != rhs._NativeValue; }
inline bool operator==(SObjHashCode lhs, SObjHashCode rhs) { return lhs._NativeValue == rhs._NativeValue; }
inline bool operator!=(SObjHashCode lhs, nullptr_t) { return lhs._NativeValue != SObjHashCode::SOBJ_NATIVEVALUE_NULL; }
inline bool operator==(SObjHashCode lhs, nullptr_t) { return lhs._NativeValue == SObjHashCode::SOBJ_NATIVEVALUE_NULL; }


inline int32 HashValue(SObjHashCode InValue)
{
	union {
		struct {
			int32 valueH; // 해쉬 상위 32비트
			int32 valueL; // 해쉬 하위 32비트
		};
		int64 valueX; // 해쉬 64비트 전체값
	};

	valueX = InValue._NativeValue;
	return valueH ^ valueL;
}


inline SS::StringW ToStringW(SObjHashCode InCode)
{
	constexpr int64 BUFFER_CNT = 16;
	utf16 Buffer[BUFFER_CNT];
	errno_t err = _i64tow_s(InCode._NativeValue, Buffer, BUFFER_CNT, 10);
	if (err != 0)
	{
		SS_ASSERT(false);
		return SS::StringW();
	}

	SS::StringW Result = L"SObjHashCode_";
	Result += Buffer;

	return Result;
}