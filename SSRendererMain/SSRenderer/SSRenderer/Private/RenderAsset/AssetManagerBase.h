#pragma once
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"


class AssetManagerBase : public IAssetManager
{
private:
	static const char* GetAssetSuffix(EAssetType InAssetType);

protected:
	SS::HashMap<SS::SHasherW, IAssetBase*> _assetHashMap[(int32)EAssetType::Count];

public:
	AssetManagerBase(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity);
	virtual ~AssetManagerBase();

	virtual void AddToAssetPool(IAssetBase* newAsset) override;
	virtual void ReleaseAllAssets() override;
	virtual SS::SHasherW GenerateAssetName(const SS::StringW& fileName, const SS::StringW& nodeName, EAssetType InAssetType) const override;

	virtual IAssetBase* FindAssetByName(SS::SHasherW InModelAssetName, EAssetType InAssetType) const override;

	
};
