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

const HasherPoolNode* HasherPoolBase::FindOrAddHasherValue(const utf16* InStr, uint32 InStrLen, uint32 InHashedValue)
{
	if (InStr == nullptr || InStrLen == 0)
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
	if (CurHasherPoolNode == nullptr) // 해당 해시 버킷에 노드가 들어간 적이 없는 경우
	{
		HasherPoolNode* NewNode = (HasherPoolNode*)DBG_MALLOC(sizeof(HasherPoolNode) + InStrSpaceSize);
		NewNode->_next = nullptr;
		NewNode->_hashX = HashX;
		NewNode->_strLen = InStrLen;
		wcscpy(NewNode->_str, InStr);
		_HasherBucket[BucketIdx] = NewNode;

		return NewNode;
	}


	while (true)
	{
		if (wcscmp(CurHasherPoolNode->_str, InStr) == 0) // 원하는 노드를 찾은 경우
		{
			return CurHasherPoolNode;
		}

		if (CurHasherPoolNode->_next == nullptr) // 원하는 노드가 없는 경우 새로 만들기
		{
			HasherPoolNode* NewNode = (HasherPoolNode*)DBG_MALLOC(sizeof(HasherPoolNode) + InStrSpaceSize);
			NewNode->_next = nullptr;
			NewNode->_hashX = HashX;
			NewNode->_strLen = InStrLen;
			wcscpy(NewNode->_str, InStr);
			CurHasherPoolNode->_next = NewNode;
			CurNodeCnt++;

			return NewNode;
		}

		CurHasherPoolNode = CurHasherPoolNode->_next;
		CurNodeCnt++;
	}
}
