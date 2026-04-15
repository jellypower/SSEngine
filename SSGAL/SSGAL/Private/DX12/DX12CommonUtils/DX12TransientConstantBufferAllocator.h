#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/SSTransientMemAllocator.h"


class DX12GALRenderDeviceContext;

class DX12TransientConstantBufferAllocator : public SSTransientMemAllocator
{
public:
	DX12TransientConstantBufferAllocator(
		DX12GALRenderDeviceContext* InRenderDeviceContext,
		int32 InEachPageSize, 
		int32 MinAllocSize, 
		int32 AlignSize,
		const utf16* AllocatorName);

protected:
	void* AllocPage() override;
	void FreePage(void* Page) override;

private:
	DX12GALRenderDeviceContext* _RenderDeviceContext = nullptr;
};
