#pragma once
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"


struct HasherPoolNode
{
	HasherPoolNode* _next = nullptr;
	uint64 _strLen = 0; // strlen을 uint32가 아니라 uint64로 두는 이유는 16바이트 얼라인을 맞추기 위해서입니다.
	utf16 _str[0];
};

class HasherPoolBase : public IHasherPool
{
public:
	HasherPoolBase(int32 InBucketCnt);
	virtual ~HasherPoolBase();

public:
	uint64 FindHasherValue(const utf16* InLoweredStr, uint32 InStrLen, uint32 InHashedValue) const override;
	uint64 AddHasherValue(const utf16* InStr, uint32 InStrLen, uint32 InHashedValue) override;


private:
	int32 _HasherBucketCnt;
	HasherPoolNode** _HasherBucket;
};
