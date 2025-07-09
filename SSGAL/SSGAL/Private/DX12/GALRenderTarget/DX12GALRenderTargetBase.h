#pragma once
#include <d3d12.h>

#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"


class DX12GALRenderTargetBase : public GALRenderTarget
{
public:
	static DXGI_FORMAT ConvertColorFormat(ERTColorFormat Format);

public:
	virtual void SetRenderTarget(ID3D12GraphicsCommandList* CmdList) = 0;
	virtual void ClearRenderTarget(ID3D12GraphicsCommandList* CmdList) = 0;

protected:
	static D3D12_RESOURCE_STATES ConvertResourceStates(EResourceStateType Type);
};
