#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

class DX12GALRenderDevice;

class DX12ConstantBufferResourceCustomAllocator : public SSCustomMemChunkAllocator
{
public:
	DX12ConstantBufferResourceCustomAllocator(DX12GALRenderDevice* InRenderDevice, int32 InEachPageSize, int32 MinAllocSize, const utf16* AllocatorName);

protected:
	void* AllocPage(int32 PageSize) override;
	void FreePage(void* Page) override;

private:
	DX12GALRenderDevice* _RenderDevice = nullptr;
};
