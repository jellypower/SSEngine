#pragma once
#include "AssetManagerBase.h"

class ModelAsset;

class ModelAssetManager : public AssetManagerBase
{
public:
	ModelAssetManager(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity);
	virtual ~ModelAssetManager();
};
