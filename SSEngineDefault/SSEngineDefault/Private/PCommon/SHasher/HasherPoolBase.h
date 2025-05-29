#pragma once
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"




class HasherPoolBase : public IHasherPool
{
public:
	HasherPoolBase(int32 InBucketCnt);
	virtual ~HasherPoolBase();

public:
	virtual const utf16* FindC_Str(uint64 InHashX, uint32* const OutStrLen = nullptr) const override;
	virtual uint64 FindOrAddHasherValue(const utf16* InLoweredStr, uint32 InStrLen, uint32 InHashedValue) override;
};
