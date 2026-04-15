#pragma once
// #include <d3d12.h>

#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"


class DX12GALRenderTargetBase : public GALRenderTarget
{
public:
	virtual ID3D12Resource* GetCurrentResource() const = 0;

	// virtual ID3D12DescriptorHeap* GetCurrentRTVDescHeap() const = 0; // 필요없음: RenderTarget은 SetDescriptorHeaps함수가 필요 없음, DescTable바인딩 할 때나 쓰기 때문임
	virtual CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const = 0;

	// virtual ID3D12DescriptorHeap* GetCurrentSRVDescHeap() const = 0; // 필요없음: 어차피 CopyDescriptorsSimple써서 사용하기 때문에
	virtual CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentSRV() const = 0;

	virtual void ClearRenderTarget(ID3D12GraphicsCommandList* CmdList, const Vector4f& ClearColor) = 0;
};
