#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

class TestCustomHeapAllocator : public SSCustomMemChunkAllocator
{
public:
	explicit TestCustomHeapAllocator(int32 DefaultPageSize, int32 MinAllocSize, const utf16* AllocatorName = nullptr);

private:
	virtual void* AllocPage(int32 PageSize) override;
	virtual void FreePage(void* Page) override;
};
