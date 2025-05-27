#pragma once
#include "SSNativeKeywords.h"

struct SSDeviceInfo
{
	int32 PhysicalCoreCnt = 0;
	int32 LogicalCoreCnt = 0;
};

bool QueryDeviceInfo(SSDeviceInfo* OutDeviceInfo);