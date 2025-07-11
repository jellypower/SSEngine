#pragma once
#include "IAssetBase.h"

class GALTextureAssetWrapperBase;

class ITextureAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Texture;

protected:
	GALTextureAssetWrapperBase* _GALTextureAsset = nullptr;

public:
	GALTextureAssetWrapperBase* GetGALTextureAsset() const { return _GALTextureAsset; }

public:
	virtual void ReleaseGALData() = 0;
};