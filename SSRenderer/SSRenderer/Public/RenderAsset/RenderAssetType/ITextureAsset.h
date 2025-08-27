#pragma once
#include "IAssetBase.h"

class GALTextureAssetWrapperBase;

enum class ETextureType
{
	None = -1,

	Texture2D,
	CubeMap
};

class ITextureAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Texture;

protected:
	GALTextureAssetWrapperBase* _GALTextureAsset = nullptr;

public:
	GALTextureAssetWrapperBase* GetGALTextureAsset() const { return _GALTextureAsset; }

public:
	virtual ETextureType GetTextureType() const = 0;

	virtual void ReleaseGALData() = 0;
};