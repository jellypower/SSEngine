#pragma once
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"

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

	virtual IMaterialAsset* CreateEmptyMaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt) = 0;

	virtual SS::SHasherW GenerateAssetName(const SS::StringW& fileName, const SS::StringW& nodeName, EAssetType InAssetType) const = 0;
};
