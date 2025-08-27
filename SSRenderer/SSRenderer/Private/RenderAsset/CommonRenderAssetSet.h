#pragma once
#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"

class CommonRenderAssetSet : public ICommonRenderAssetSet
{
public:
	virtual void InitializeCommonAssets() override;
	virtual void ReleaseCachedAssets() override;
};
