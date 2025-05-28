#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IHasherPool : INoncopyable
{
public:
	virtual uint64 FindOrAddHasherValue(const utf16* InLoweredStr, uint32 InStrLen, uint32 InHashedValue) = 0;
};
