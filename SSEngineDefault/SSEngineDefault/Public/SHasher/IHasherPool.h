#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IHasherPool : INoncopyable
{
public:
	virtual ~IHasherPool() = default;
	virtual uint64 FindHasherValue(const utf16* InStr, uint32 InStrLen, uint32 InHashedValue) const = 0;
	// !!! Calling AddHasherValue function for the same string is prohibited !!!
	virtual uint64 AddHasherValue(const utf16* InStr, uint32 InStrLen, uint32 InHashedValue) = 0;
};
