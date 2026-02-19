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

	virtual ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, ETextureType InType) override;
	virtual IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath) override;
	virtual IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath) override;
	virtual IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt) override;
	virtual IMaterialAssetMutable* CreateEmptyMaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath) override;
	virtual IRenderAnimAssetMutable* CreateEmptyRenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath) override;

public:
	virtual const SS::HashMap<SS::SHasherW, IAssetBase*>& GetAssetMap(EAssetType InAssetType) const override;
	virtual void FindAssetsOfNamespace(SS::PooledList<IAssetBase*>& AssetListToFill, SS::SHasherW Namespace, EAssetType InAssetType) const override;

	virtual IAssetBase* FindAssetByName(SS::SHasherW InAssetName, EAssetType InAssetType) const override;

public:
	virtual bool AddAssetReferencer(SS::SHasherW InAssetName, EAssetType InAssetType, const AssetInstanceReferencer& Referencer) override;
	virtual bool RemoveAssetReferencer(SS::SHasherW InAssetName, EAssetType InAssetType, const AssetInstanceReferencer& Referencer) override;
	
};
