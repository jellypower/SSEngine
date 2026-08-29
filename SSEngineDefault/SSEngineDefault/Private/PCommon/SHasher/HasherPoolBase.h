#pragma once
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"




class HasherPoolBase : public IHasherPool
{
public:
	HasherPoolBase(int32 InBucketCnt);
	void Release() override;

public:
	virtual const HasherPoolNode* FindOrAddHasherValue(const utf16* InStr, uint32 InStrLen, uint32 InHashedValue) override;
};
