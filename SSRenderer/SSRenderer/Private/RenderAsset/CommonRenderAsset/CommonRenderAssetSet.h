#pragma once
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"

class CommonRenderAssetSet : public ICommonRenderAssetSet
{
public:
	void Release() override;

public:
	virtual void InitializeCommonAssets() override;
	virtual void ReleaseCachedAssets() override;
};
