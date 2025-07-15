#include "pch.h"
#include "SSDX12Utils.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "Public/SSGALCommonEnums.h"
#include "Public/GALRenderTarget/GALRTCommonEnums.h"

int32 SS::DX12Util::GetColorFormatSizeInByte(ERTColorFormat Format)
{
	switch (Format)
	{
	case ERTColorFormat::D32_FLOAT: return 4;
	case ERTColorFormat::R32G32_SINT: return 8;
	case ERTColorFormat::R8G8B8A8_UNORM: return 4;
	}

	SS_INTERRUPT();
}

DXGI_FORMAT SS::DX12Util::ConvertColorFormat(ERTColorFormat Format)
{
	switch (Format)
	{
	case ERTColorFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
	case ERTColorFormat::D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
	case ERTColorFormat::R32G32_SINT: return DXGI_FORMAT_R32G32_SINT;
	case ERTColorFormat::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	SS_INTERRUPT();
}

D3D12_RESOURCE_STATES SS::DX12Util::ConvertResourceStates(EResourceStateType Type)
{
	switch (Type)
	{
	case EResourceStateType::Common: return D3D12_RESOURCE_STATE_COMMON;
	case EResourceStateType::CopySrc: return D3D12_RESOURCE_STATE_COPY_SOURCE;
	case EResourceStateType::CopyDest: return D3D12_RESOURCE_STATE_COPY_DEST;
	case EResourceStateType::RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
	case EResourceStateType::Present: return D3D12_RESOURCE_STATE_PRESENT;
	case EResourceStateType::DepthWrite: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}

	SS_INTERRUPT();
}