#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

#include "SSNativeKeywords.h"

struct SSDeviceInfo
{
	int32 PhysicalCoreCnt = 0;
	int32 LogicalCoreCnt = 0;
};

SSENGINEDEFAULT_MODULE_NATIVE bool QueryDeviceInfo(SSDeviceInfo* OutDeviceInfo);