#pragma once


// GAL Render Target Common Enums

enum class ERenderTargetType : uint8
{
	None = 0,
	SwapChain,
	Default,
	Default_UAV,
	DepthStencil
};

// RenderTargetColorFormat
enum class ERTColorFormat : int32
{
	None,

	Unknown,

	D32_FLOAT,
	R32G32_SINT,
	R8G8B8A8_UNORM,
	R32G32B32A32_FLOAT,
	R32G32B32_FLOAT,
	R32G32_FLOAT,
};
