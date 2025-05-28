#include "HasherPoolBase.h"

#include <string.h>

#include "SSEngineDefault/Public/SSDebugLogger.h"

HasherPoolBase::HasherPoolBase(int32 InBucketCnt)
{
	_HasherBucketCnt = InBucketCnt;
	_HasherBucket = (HasherPoolNode**)DBG_MALLOC(sizeof(HasherPoolNode*) * _HasherBucketCnt);
	memset(_HasherBucket, 0, sizeof(HasherPoolNode*) * _HasherBucketCnt);
}

HasherPoolBase::~HasherPoolBase()
{

}

uint64 HasherPoolBase::FindHasherValue(const utf16* InLoweredStr, uint32 InStrLen, uint32 InHashedValue) const
{
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


	while (CurHasherPoolNode != nullptr)
	{
		if (wcsncmp(CurHasherPoolNode->_str, InLoweredStr, InStrLen) == 0)
		{
			return HashX;
		}

		CurHasherPoolNode = CurHasherPoolNode->_next;
		CurNodeCnt++;
	}

	return 0;
}

uint64 HasherPoolBase::AddHasherValue(const utf16* InStr, uint32 InStrLen, uint32 InHashedValue)
{
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
	HasherPoolNode* NewNode = (HasherPoolNode*)DBG_MALLOC(sizeof(HasherPoolNode) + InStrSpaceSize);
	NewNode->_next = nullptr;
	NewNode->_strLen = InStrLen;
	wcscpy_s(NewNode->_str, InStrLen + 1, InStr);

	if (CurHasherPoolNode == nullptr)
	{
		_HasherBucket[BucketIdx] = NewNode;
		return HashX;
	}


	CurNodeCnt++;
	while (CurHasherPoolNode->_next != nullptr)
	{
		CurNodeCnt++;
		CurHasherPoolNode = CurHasherPoolNode->_next;
	}

	CurHasherPoolNode->_next = NewNode;
	return HashX;
}
