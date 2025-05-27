#include "ModelAssetManager.h"

#include "RenderAssetType/ModelAsset.h"

ModelAssetManager::ModelAssetManager(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity)
	: AssetManagerBase(AssetHashMapCapacity, AssetHashMapBucketCapacity)
{
}

ModelAssetManager::~ModelAssetManager()
{
}