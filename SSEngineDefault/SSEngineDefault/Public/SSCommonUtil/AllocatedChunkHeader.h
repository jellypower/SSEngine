#pragma once 
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSEngineDefault/Public/SHasher/SHasherW.h"


constexpr int32 NON_DEFAULT_PAGE_IDX = -1;

struct AllocatedChunkHeader
{
	SS::SHasherW	ChunkName;
	void*			PageContent = nullptr;
	int32			DefaultPageIdx = NON_DEFAULT_PAGE_IDX;
	int32			IdxInLinkedList = INVALID_IDX;
	int32			ChunkOffset = 0;
	int32			Size = 0;

	bool operator==(const AllocatedChunkHeader& rhs) const
	{
		return
			ChunkName == rhs.ChunkName &&
			PageContent == rhs.PageContent &&
			DefaultPageIdx == rhs.DefaultPageIdx &&
			IdxInLinkedList == rhs.IdxInLinkedList &&
			ChunkOffset == rhs.ChunkOffset &&
			Size == rhs.Size;
	}
};
