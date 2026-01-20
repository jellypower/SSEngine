#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/SSCommonUtil/SSTransientMemAllocator.h"

SSTransientMemAllocator::SSTransientMemAllocator(
	int32 InEachPageSize,
	int32 MinAllocSize,
	int32 AlignSize,
	const utf16* AllocatorName
)
{
	_EachPageSize =		InEachPageSize;
	_MinAllocSize =		MinAllocSize;
	_AlignSize =		AlignSize;
	_AllocatorName = AllocatorName;
}

void SSTransientMemAllocator::ReserveDefaultPage(int64 NewDefaultPageCnt)
{
	if (NewDefaultPageCnt <= _PageContents.GetSize())
	{
		return;
	}

	if (NewDefaultPageCnt > _PageContents.GetCapacity())
	{
		_PageContents.Reserve(NewDefaultPageCnt * 2); // List의 공간은 미리 reserve해둔다.
	}

	int64 NewlyAllocatedPageCnt = NewDefaultPageCnt - _PageContents.GetSize();
	for (int32 i = 0; i < NewlyAllocatedPageCnt; i++)
	{
		void* NewPageContent = AllocPage();
		_PageContents.PushBack(NewPageContent);
	}
}

void SSTransientMemAllocator::ReleaseDefaultPages()
{
	for (void* PageContentItem : _PageContents)
	{
		FreePage(PageContentItem);
	}

	_PageContents.Clear();
}

void SSTransientMemAllocator::ResetAllChunksXXX()
{
	_TransientPageIdxCursor = 0;
	_TransientPageMemOffsetCursor = 0;
}

TransientChunkHeader SSTransientMemAllocator::AllocChunk(int32 NeededSize)
{
	if (NeededSize <= 0)
	{
		SS_INTERRUPT();
	}

	if (NeededSize < _MinAllocSize)
	{
		NeededSize = _MinAllocSize;
	}

	{
		int32 Unit = NeededSize / _AlignSize;
		int32 Mod = NeededSize % _AlignSize;

		int32 NewUnit = Unit +
			(Mod == 0 ? 0 : 1);

		NeededSize = _AlignSize * NewUnit;
	}

	if (_EachPageSize < NeededSize)
	{
		SS_INTERRUPT();
	}


	int AvailableMemSpace = _EachPageSize - _TransientPageMemOffsetCursor;


	if (NeededSize > AvailableMemSpace)
	{
		_TransientPageIdxCursor++;
		_TransientPageMemOffsetCursor = 0;
	}

	if (_PageContents.GetSize() <= _TransientPageIdxCursor)
	{
		ReserveDefaultPage(_TransientPageIdxCursor + 1); // increase "_PageContents"
	}


	TransientChunkHeader NewChunk;
	NewChunk.PageContent = _PageContents[_TransientPageIdxCursor];
	NewChunk.ChunkOffset = _TransientPageMemOffsetCursor;
	NewChunk.Size = NeededSize;

	_TransientPageMemOffsetCursor += NeededSize;

	return NewChunk;
}
