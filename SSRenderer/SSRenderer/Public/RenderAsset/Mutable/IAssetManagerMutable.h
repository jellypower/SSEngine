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

	virtual ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, ETextureType InType) = 0;
	virtual IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt) = 0;
	virtual IMaterialAssetMutable* CreateEmptyMaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IRenderAnimAssetMutable* CreateEmptyRenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
};
