#include "TestCustomHeapAllocator.h"

TestCustomHeapAllocator::TestCustomHeapAllocator(int32 DefaultPageSize, int32 MinAllocSize, int32 AlignSize, const utf16* AllocatorName)
	: SSCustomMemChunkAllocator(DefaultPageSize, MinAllocSize, AlignSize, AllocatorName)
{
}

void* TestCustomHeapAllocator::AllocPage(int32 PageSize)
{
	return DBG_MALLOC(PageSize);
}

void TestCustomHeapAllocator::FreePage(void* Page)
{
	free(Page);
}
