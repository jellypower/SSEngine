#include "SSRenderer/Public/RenderAsset/ModelAssetManager.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelAsset.h"

ModelAssetManager::ModelAssetManager(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity)
	: AssetManagerBase(AssetHashMapCapacity, AssetHashMapBucketCapacity)
{
}

ModelAssetManager::~ModelAssetManager()
{
}