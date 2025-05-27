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