#include "HasherPoolBase.h"

#define _CRT_SECURE_NO_WARNINGS

#include <cstring>

#include "SSEngineDefault/Public/SSDebugLogger.h"

HasherPoolBase::HasherPoolBase(int32 InBucketCnt)
{
	_HasherBucketCnt = InBucketCnt;
	_HasherBucket = (HasherPoolNode**)DBG_MALLOC(sizeof(HasherPoolNode*) * _HasherBucketCnt);
	memset(_HasherBucket, 0, sizeof(HasherPoolNode*) * _HasherBucketCnt);
}

HasherPoolBase::~HasherPoolBase()
{
	for (uint32 i = 0; i < _HasherBucketCnt; i++)
	{
		HasherPoolNode* CurNode = _HasherBucket[i];
		if (CurNode == nullptr)
		{
			continue;
		}

		while (CurNode != nullptr)
		{
			HasherPoolNode* NextNode = CurNode->_next;
			free(CurNode);
			CurNode = NextNode;
		}
	}

	free(_HasherBucket);
}

const utf16* HasherPoolBase::FindC_Str(uint64 InHashX, uint32* const OutStrLen) const
{
	static constexpr utf16 EMPTY_STR[] = L"EMPTY";
	if (InHashX == 0)
	{
		return EMPTY_STR;
	}

	union {
		struct {
			uint32 HashedValue; // 해쉬 상위 32비트
			uint32 CurNodeCnt; // 해쉬 하위 32비트
		};
		uint64 HashX; // 해쉬 64비트 전체값
	};

	HashX = InHashX;


	HasherPoolNode* CurHashPoolNode = _HasherBucket[HashedValue % _HasherBucketCnt];

	for (uint32 i = 0; i < CurNodeCnt; i++)
	{
		CurHashPoolNode = CurHashPoolNode->_next;
	}

	if (OutStrLen != nullptr)
	{
		*OutStrLen = CurHashPoolNode->_strLen;
	}

	return CurHashPoolNode->_str;
}

uint64 HasherPoolBase::FindOrAddHasherValue(const utf16* InLoweredStr, uint32 InStrLen, uint32 InHashedValue)
{
	if (InLoweredStr == nullptr || InStrLen == 0)
	{
		SS_INTERRUPT();
	}

	union {
		struct {
			uint32 HashedValue; // 해쉬 상위 32비트
			uint32 CurNodeCnt; // 해쉬 하위 32비트
		};
		uint64 HashX; // 해쉬 64비트 전체값
	};

	if (InHashedValue == 0)
	{
		InHashedValue = 1;
	}
	HashedValue = InHashedValue;

	uint32 BucketIdx = HashedValue % _HasherBucketCnt;

	HasherPoolNode* CurHasherPoolNode = _HasherBucket[BucketIdx];
	CurNodeCnt = 0;


	uint64 InStrSpaceSize = sizeof(utf16) * (InStrLen + 1);
	if (CurHasherPoolNode == nullptr)
	{
		HasherPoolNode* NewNode = (HasherPoolNode*)DBG_MALLOC(sizeof(HasherPoolNode) + InStrSpaceSize);
		NewNode->_next = nullptr;
		NewNode->_strLen = InStrLen;
		wcscpy(NewNode->_str, InLoweredStr);
		_HasherBucket[BucketIdx] = NewNode;

		return HashX;
	}


	while (true)
	{
		if (wcscmp(CurHasherPoolNode->_str, InLoweredStr) == 0)
		{
			return HashX;
		}

		if (CurHasherPoolNode->_next == nullptr)
		{
			HasherPoolNode* NewNode = (HasherPoolNode*)DBG_MALLOC(sizeof(HasherPoolNode) + InStrSpaceSize);
			NewNode->_next = nullptr;
			NewNode->_strLen = InStrLen;
			wcscpy(NewNode->_str, InLoweredStr);
			CurHasherPoolNode->_next = NewNode;
			CurNodeCnt++;

			return HashX;
		}

		CurHasherPoolNode = CurHasherPoolNode->_next;
		CurNodeCnt++;
	}
}
