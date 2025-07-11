#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class ITextureAsset;
class GALRenderDevice;

class GALTextureAssetWrapperBase : public INoncopyable
{
protected:
	ITextureAsset* _OwnerTexture = nullptr;
	GALRenderDevice* _OwnerRenderDevice = nullptr;
};

