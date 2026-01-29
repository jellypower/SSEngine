#pragma once
#include "SSEngineDefault/Public/SSContainer/PooledList.h"


int AppendData_Internal(SS::PooledList<byte>& ToData, const void* From, int FromDataSize)
{
	const int32 OriginalSize = ToData.GetSize();
	ToData.SetSizeDirectly(OriginalSize + FromDataSize);

	byte* CopyTargetRaw = ToData.GetData() + OriginalSize;

	errno_t Result = memcpy_s(
		CopyTargetRaw, FromDataSize,
		From, FromDataSize);

	if (Result != 0)
	{
		SS_ASSERT(false);
		return 0;
	}

	return FromDataSize;
}