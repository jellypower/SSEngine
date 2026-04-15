#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"

class ITextureAssetMutable : public ITextureAsset
{
public:
	void InjectGALTextureAsset(GALTextureAssetWrapperBase* InGALTextureAsset) { _GALTextureAsset = InGALTextureAsset; }
};
