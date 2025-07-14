#pragma once
#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"

class CommonRenderAssetSet : public ICommonRenderAssetSet
{
public:
	virtual void CacheCommonRenderAssets() override;
	virtual void AddRefCachedAssets() override;
	virtual void ReleaseCachedAssets() override;
};
