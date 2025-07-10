#pragma once
// #include <d3d12.h>

#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"


class DX12GALRenderTargetBase : public GALRenderTarget
{
public:
	virtual ID3D12Resource* GetCurrentResource() const = 0;

	virtual void SetRenderTarget(ID3D12GraphicsCommandList* CmdList) = 0;
	virtual void ClearRenderTarget(ID3D12GraphicsCommandList* CmdList) = 0;
};
