#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"


class IModelCombinationAsset;

class IRenderAnimAssetMutable : public IRenderAnimAsset
{
public:
	virtual RenderAnimRawData* GetMutableRawData() = 0;

	virtual void SetOriginMdlcAsset(IModelCombinationAsset* InMdlcAsset) = 0;
	virtual void InjectRawDataXXX(RenderAnimRawData* InRenderAnimData) = 0;
};
