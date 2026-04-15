#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

#include "TransientChunkHeader.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

class SSENGINEDEFAULT_MODULE SSTransientMemAllocator : public INoncopyable
{
private:
	// Allocator Setting
	int32 _EachPageSize = 0;
	int32 _MinAllocSize = 0;
	int32 _AlignSize = 0;
	SS::StringW _AllocatorName;

	// Pages
	SS::PooledList<void*> _PageContents;


	// Transient Alloc State
	int32 _TransientPageIdxCursor = 0;
	int32 _TransientPageMemOffsetCursor = 0;

protected:
	explicit SSTransientMemAllocator(
		int32 InEachPageSize,
		int32 MinAllocSize,
		int32 AlignSize,
		const utf16* AllocatorName);

public:
	int32 GetEachPageSize() const { return _EachPageSize; }
	const SS::StringW& GetAllocatorName() const { return _AllocatorName; }

public:
	void ReserveDefaultPage(int64 NewDefaultPageCnt);
	void ReleaseDefaultPages();
	void ResetAllChunksXXX();

public:
	TransientChunkHeader AllocChunk(int32 NeededSize);


protected:
	virtual void* AllocPage() = 0;
	virtual void FreePage(void* Page) = 0;
};
