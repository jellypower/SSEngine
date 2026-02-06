#pragma once
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"

class CommonRenderAssetSet : public ICommonRenderAssetSet
{
public:
	virtual void InitializeCommonAssets() override;
	virtual void TEMP_CacheCommonAssetFromFBX() override;
	virtual void ReleaseCachedAssets() override;
};
