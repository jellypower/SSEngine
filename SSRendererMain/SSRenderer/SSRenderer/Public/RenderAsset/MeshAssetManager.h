#pragma once
#include "AssetManagerBase.h"

class MeshAsset;

class MeshAssetManager : public AssetManagerBase
{
public:
	MeshAssetManager(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity);
	virtual ~MeshAssetManager();


	virtual void ReleaseAllAssets() override;
};

