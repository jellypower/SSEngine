#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

struct HasherPoolNode
{
	HasherPoolNode* _next = nullptr;

	union {
		struct {
			uint32 _HashedValue; // 해쉬 상위 32비트
			uint32 _CurNodeCnt; // 해쉬 하위 32비트
		};
		uint64 _hashX; // 해쉬 64비트 전체값
	};

	uint64 _strLen = 0; // strlen을 uint32가 아니라 uint64로 두는 이유는 16바이트 얼라인을 맞추기 위해서입니다.
	utf16 _str[0];
};

class IHasherPool : INoncopyable
{
public:
	virtual const HasherPoolNode* FindOrAddHasherValue(const utf16* InLoweredStr, uint32 InStrLen, uint32 InHashedValue) = 0;

protected:
	int32 _HasherBucketCnt;
	HasherPoolNode** _HasherBucket;
};
