#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"


class DX12GALRenderDevice;

class DX12DescriptorHeapCustomAllocator : public SSCustomMemChunkAllocator
{
public:
	DX12DescriptorHeapCustomAllocator(DX12GALRenderDevice* InRenderDevice, int32 InEachPageSize, int32 MinAllocSize, const utf16* AllocatorName = nullptr);

public:
	int32 GetSRVDescriptorHandleIncrementSize() const { return _SRVDescriptorHandleIncrementSize; }

protected:
	void* AllocPage(int32 PageSize) override;
	void FreePage(void* Page) override;

private:
	DX12GALRenderDevice* _RenderDevice = nullptr;

	int32 _SRVDescriptorHandleIncrementSize = 0;
};
