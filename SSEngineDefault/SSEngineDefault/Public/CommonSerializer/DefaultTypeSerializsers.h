#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSEngineDefault/Public/SHasher/SHasherW.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"



/// <returns>
/// return when Written Byte Size.
/// if something went wrong, return 0
/// </returns>
SSENGINEDEFAULT_MODULE int64 AppendData(
	SS::PooledList<byte>& ToData,
	const void* From,
	int64 FromDataSize);


/// <returns>
/// return when Written Byte Size.
/// if something went wrong, return 0
/// </returns>
SSENGINEDEFAULT_MODULE int64 FillMemoryFromData(
	void* Dest,
	int64 CopySize,
	const SS::PooledList<byte>& FromData,
	int64 FromOffset);



SSENGINEDEFAULT_MODULE int64 AppendDataFromStrings(
	SS::PooledList<byte>& Data,
	const SS::PooledList<SS::StringW>& Strings);

SSENGINEDEFAULT_MODULE int64 FillStringFromData(
	SS::PooledList<SS::StringW>& Strings,
	const SS::PooledList<byte>& Data,
	int64 Offset = 0);

SSENGINEDEFAULT_MODULE int64 AppendDataFromHashers(
	SS::PooledList<byte>& Data,
	const SS::PooledList<SS::SHasherW>& Hashers);

SSENGINEDEFAULT_MODULE int64 FillHashersFromData(
	SS::PooledList<SS::SHasherW>& Hashers,
	const SS::PooledList<byte>& Data,
	int64 Offset = 0);
