#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSContainer/PooledLinkedList.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSEngineDefault/Public/SHasher/SHasherW.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"

#include "AllocatedChunkHeader.h"

struct AvailableMemspace
{
	int32 Offset;
	int32 Size;
};

struct PageSet
{
	void* PageContent;
	SS::PooledLinkedList<AvailableMemspace> AvailableChunks;

	PageSet() { }
	PageSet(PageSet&& rhs) : PageContent(rhs.PageContent), AvailableChunks(SS::move(rhs.AvailableChunks)) { }
};


class SSENGINEDEFAULT_MODULE SSCustomMemChunkAllocator : public INoncopyable
{
private:
	SS::PooledList<PageSet> _DefaultPages;
	SS::PooledLinkedList<AllocatedChunkHeader> _AllocatedDefaultPageChunks;

	SS::PooledLinkedList<AllocatedChunkHeader> _LargePageChunks;

	int32 _EachPageSize = 0;
	int32 _MinAllocSize = 0;
	int32 _AlignSize = 0;

	SS::StringW _AllocatorName;

public:
	explicit SSCustomMemChunkAllocator(int32 InEachPageSize, int32 MinAllocSize, int32 AlignSize, const utf16* AllocatorName);

public:
	bool IsAnyChunkInUse() const;

	void ReserveDefaultPage(int64 NewDefaultPageCnt);
	void ReleaseDefaultPages();

	AllocatedChunkHeader AllocChunk(int32 NeededSize, SS::SHasherW ChunkName = SS::SHasherW::GetEmpty());
	void ReleaseChunk(const AllocatedChunkHeader& ChunkToRelease);


protected:
	const SS::StringW& GetAllocatorName() const { return _AllocatorName; }
	int32 GetEachPageSize() const { return _EachPageSize; }

protected:
	virtual void* AllocPage(int32 PageSize) = 0;
	virtual void FreePage(void* Page) = 0;
};

