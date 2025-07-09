#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class GALRenderDevice;

class GALCPUReadableTexture : INoncopyable
{
protected:
	GALRenderDevice* _OwnerRenderDevice = nullptr;
	Vector2i32 _ResourceWidthHeight;
};
