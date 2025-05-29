#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SObject/ModuleExportKeyword.h"

class SObjectBase;

class SOBJECT_MODULE SObjHashCode
{
	static constexpr int64 SOBJ_NATIVEVALUE_NULL = 0;

private:
	int64 _NativeValue = SOBJ_NATIVEVALUE_NULL;

public:
	SObjHashCode(){ }
	SObjHashCode(nullptr_t){ }
	explicit SObjHashCode(int64 InNativeValue): _NativeValue(InNativeValue){ }
	SObjHashCode(SObjectBase* InObject);

	FORCEINLINE int64 GetNativeValue() const { return _NativeValue; }
	FORCEINLINE bool operator!=(const SObjHashCode& rhs) const { return _NativeValue != rhs._NativeValue; }
	FORCEINLINE bool operator==(const SObjHashCode& rhs) const { return _NativeValue == rhs._NativeValue; }
	FORCEINLINE bool operator!=(nullptr_t) const { return _NativeValue != SOBJ_NATIVEVALUE_NULL; }
	FORCEINLINE bool operator==(nullptr_t) const { return _NativeValue == SOBJ_NATIVEVALUE_NULL; }

	SObjectBase* GetSObject() const;
};

FORCEINLINE int32 HashValue(const SObjHashCode& InValue)
{
	union {
		struct {
			int32 valueH; // 해쉬 상위 32비트
			int32 valueL; // 해쉬 하위 32비트
		};
		int64 valueX; // 해쉬 64비트 전체값
	};

	valueX = InValue.GetNativeValue();
	return valueH ^ valueL;
}
