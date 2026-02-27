#pragma once
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"

class IRenderAnimAssetMutable;
enum class ETextureType : int32;
class IMaterialAssetMutable;
class ITextureAssetMutable;
class IModelCombinationAssetMutable;
class IMaterialAsset;
class IModelAssetMutable;
class IMeshAssetMutable;

class IAssetManagerMutable : public IAssetManager
{
public:
	virtual void AddToAssetPool(IAssetBase* newAsset) = 0;
	virtual void ReleaseAllAssets() = 0;
};
