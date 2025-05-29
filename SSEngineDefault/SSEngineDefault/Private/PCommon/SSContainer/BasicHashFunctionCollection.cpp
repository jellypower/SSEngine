#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/SSContainer/BasicHashFunctionCollection.h"

#include "SSEngineDefault/Public/SSContainer/CityHash.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SHasher/SHasherW.h"

uint32 HashValue(int32 inValue)
{
	union
	{
		int32 i32Value;
		uint32 ui32Value;
	};

	i32Value = inValue;

	return ui32Value;
}

uint32 HashValue(int64 inValue)
{
	union
	{
		int64 i64Value;
		struct
		{
			uint32 Hui32Value;
			uint32 Lui32Value;
		};
	};

	i64Value = inValue;

	return Hui32Value ^ Lui32Value;
}

uint32 HashValue(uint64 inValue)
{
	union
	{
		uint64 ui64Value;
		struct
		{
			uint32 Hui32Value;
			uint32 Lui32Value;
		};
	};

	ui64Value = inValue;

	return Hui32Value ^ Lui32Value;
}

uint32 HashValue(const SS::SHasherW& inValue)
{
	uint32 HashedValue = HashValue(inValue.GetDirectValue());
	return HashedValue;
}

uint32 HashValue(const SS::StringW& inValue)
{
	int32 StrLen = inValue.GetStrLen();
	uint32 HashValue = CityHash32(reinterpret_cast<const char*>(inValue.C_Str()), StrLen * (sizeof(utf16) / sizeof(char)));
	return HashValue;
}
