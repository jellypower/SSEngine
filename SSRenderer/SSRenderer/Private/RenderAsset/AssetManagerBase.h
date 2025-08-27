#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"

class AssetManagerBase : public IAssetManagerMutable
{
protected:
	SS::HashMap<SS::SHasherW, IAssetBase*> _assetHashMap[(int32)EAssetType::Count];

public:
	AssetManagerBase(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity);
	virtual ~AssetManagerBase();

	virtual void AddToAssetPool(IAssetBase* newAsset) override;
	virtual void ReleaseAllAssets() override;

	virtual ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, ETextureType InType) override;
	virtual IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) override;
	virtual IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) override;
	virtual IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt) override;
	virtual IMaterialAssetMutable* CreateEmptyMaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) override;

	virtual SS::SHasherW GenerateAssetName(const SS::StringW& fileName, const SS::StringW& nodeName, EAssetType InAssetType) const override;

	virtual IAssetBase* FindAssetByName(SS::SHasherW InModelAssetName, EAssetType InAssetType) const override;
	virtual const SS::HashMap<SS::SHasherW, IAssetBase*>& GetAssetMap(EAssetType InAssetType) const override;
	
};
