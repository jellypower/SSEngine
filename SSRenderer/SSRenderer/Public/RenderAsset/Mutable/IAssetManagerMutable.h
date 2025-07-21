#pragma once
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"

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

	virtual ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;
	virtual IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt) = 0;
	virtual IMaterialAssetMutable* CreateEmptyMaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) = 0;


	/// <param name="fileName">임포트할 에셋의 원본 "파일" 이름</param>
	/// <param name="nodeName">임포트할 단일 에셋의 이름. (suffix 미포함)</param>
	virtual SS::SHasherW GenerateAssetName(const SS::StringW& fileName, const SS::StringW& nodeName, EAssetType InAssetType) const = 0;
};
