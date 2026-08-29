#pragma once

class ITextureAsset;
class GALRenderDevice;

class GALTextureAssetWrapperBase : public ISSUnknown
{
protected:
	ITextureAsset* _OwnerTexture = nullptr;
	GALRenderDevice* _OwnerRenderDevice = nullptr;
};

