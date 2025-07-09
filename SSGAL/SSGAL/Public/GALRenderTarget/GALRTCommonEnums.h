#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"


// GAL Render Target Common Enums

enum class ERenderTargetType : uint8
{
	None = 0,
	SwapChain,
	Default,
};

// RenderTargetColorFormat
enum class ERTColorFormat : int32
{
	R32G32_SINT,
	R8G8B8A8_UNORM
};
