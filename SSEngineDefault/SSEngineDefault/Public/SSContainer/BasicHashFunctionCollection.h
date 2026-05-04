#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"


namespace SS
{
	class SHasherW;
	class StringW;
}

SSENGINEDEFAULT_MODULE uint32 HashValue(int32 inValue);
SSENGINEDEFAULT_MODULE uint32 HashValue(int64 inValue);
SSENGINEDEFAULT_MODULE uint32 HashValue(uint64 inValue);
SSENGINEDEFAULT_MODULE uint32 HashValue(const SS::SHasherW& inValue);
SSENGINEDEFAULT_MODULE uint32 HashValue(const SS::StringW& inValue);


inline uint32 HashValue(void* InValue)
{
	union 
	{
		uint64 i64Value;
		struct
		{
			uint32 Hui32Value;
			uint32 Lui32Value;
		};
	};

	i64Value = reinterpret_cast<size_t>(InValue);

	return Hui32Value ^ Lui32Value;
}