#pragma once
#include "IAssetBase.h"


class IModelCombinationAsset;
struct RenderAnimRawData;

class IRenderAnimAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::RenderAnim;


public:
	virtual const RenderAnimRawData* GetKeyFrameAnimData() const = 0;
	virtual const IModelCombinationAsset* GetOriginalMdlcAsset() const = 0;
};
