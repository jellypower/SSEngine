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
SSENGINEDEFAULT_MODULE int AppendData(
	SS::PooledList<byte>& ToData,
	const void* From,
	int FromDataSize);

/// <returns>
/// return when Written Byte Size.
/// if something went wrong, return 0
/// </returns>
SSENGINEDEFAULT_MODULE int FillMemoryFromData(
	void* Dest,
	int CopySize,
	const SS::PooledList<byte>& FromData,
	int FromOffset);



SSENGINEDEFAULT_MODULE int AppendDataFromStrings(
	SS::PooledList<byte>& Data,
	const SS::PooledList<SS::StringW>& Strings);

SSENGINEDEFAULT_MODULE int FillStringFromData(
	SS::PooledList<SS::StringW>& Strings,
	const SS::PooledList<byte>& Data,
	int Offset = 0);

SSENGINEDEFAULT_MODULE int AppendDataFromHashers(
	SS::PooledList<byte>& Data,
	const SS::PooledList<SS::SHasherW>& Hashers);

SSENGINEDEFAULT_MODULE int FillHashersFromData(
	SS::PooledList<SS::SHasherW>& Hashers,
	const SS::PooledList<byte>& Data,
	int Offset = 0);
