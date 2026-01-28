#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

struct StringWSerializerContainer
{
	SS::PooledList<byte> Data;
	SS::PooledList<SS::StringW> Strings;
};