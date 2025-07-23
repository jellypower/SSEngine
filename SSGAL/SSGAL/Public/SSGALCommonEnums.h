#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

enum class EResourceStateType : uint8
{
	None,

	Common,
	CopySrc,
	CopyDest,
	RenderTarget,
	Present,
	DepthWrite,
};
