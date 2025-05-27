#pragma once
#include "AssetManagerBase.h"

class MaterialAsset;




class MaterialAssetManager : public AssetManagerBase
{
public:
	MaterialAssetManager(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity);
	virtual ~MaterialAssetManager();

	MaterialAsset* FindMaterialAssetWithName(SS::SHasherW InAssetName) const;

	virtual void InstantiateAllMaterialAssets();
	virtual void ReleaseAllMaterials();

};

