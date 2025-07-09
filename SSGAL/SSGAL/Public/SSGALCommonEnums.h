#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

enum class EResourceStateType : uint8
{
	None,

	CopySrc,
	RenderTarget,
	Present
};

enum class ERenderInstanceType : int32
{
	None = -1,

	StaticMesh
};