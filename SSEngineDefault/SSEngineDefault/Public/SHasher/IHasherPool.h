#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

struct HasherPoolNode
{
	HasherPoolNode* _next = nullptr;
	uint64 _strLen = 0; // strlen을 uint32가 아니라 uint64로 두는 이유는 16바이트 얼라인을 맞추기 위해서입니다.
	utf16 _str[0];
};

class IHasherPool : INoncopyable
{
public:
	virtual const utf16* FindC_Str(uint64 InHashX, uint32* const OutStrLen = nullptr) const = 0;
	virtual uint64 FindOrAddHasherValue(const utf16* InLoweredStr, uint32 InStrLen, uint32 InHashedValue) = 0;

protected:
	int32 _HasherBucketCnt;
	HasherPoolNode** _HasherBucket;
};
