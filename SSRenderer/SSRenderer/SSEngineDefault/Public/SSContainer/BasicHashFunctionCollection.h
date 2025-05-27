#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"


namespace SS
{
	class SHasherW;
	class StringW;
}

uint32 HashValue(int32 inValue);
uint32 HashValue(int64 inValue);
uint32 HashValue(uint64 inValue);
uint32 HashValue(const SS::SHasherW& inValue);
uint32 HashValue(const SS::StringW& inValue);