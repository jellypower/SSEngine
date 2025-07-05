#include "DX12GALRenderTargetBase.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"


D3D12_RESOURCE_STATES DX12GALRenderTargetBase::ConvertResourceStates(EResourceStateType Type)
{
	switch (Type)
	{
	case EResourceStateType::Present: return D3D12_RESOURCE_STATE_PRESENT;
	case EResourceStateType::RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	SS_INTERRUPT();
}

DXGI_FORMAT DX12GALRenderTargetBase::ConvertColorFormat(ERTColorFormat Format)
{
	switch (Format)
	{
	case ERTColorFormat::R32_SINT: return DXGI_FORMAT_R32G32_SINT;
	case ERTColorFormat::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	SS_INTERRUPT();
}
