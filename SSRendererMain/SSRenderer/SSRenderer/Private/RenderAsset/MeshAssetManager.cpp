#include "SSRenderer/Public/RenderAsset/MeshAssetManager.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderAssetCommon/CommonDataType.h"

MeshAssetManager::MeshAssetManager(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity)
	: AssetManagerBase(AssetHashMapCapacity, AssetHashMapBucketCapacity)
{
}

MeshAssetManager::~MeshAssetManager()
{
}



void MeshAssetManager::ReleaseAllAssets()
{
	for (SS::pair<SS::SHasherW, SSAssetBase*> item : _assetHashMap)
	{
		if (item.second == nullptr)
		{
			SS_ASSERT_MSG(false, L"에셋이 비어있습니다.");
			continue;
		}

		if (item.second->GetAssetType() != EAssetType::Mesh)
		{
			SS_ASSERT_MSG(false, L"Invalid Asset.");
			continue;
		}

		MeshAsset* MeshAssetItem = (MeshAsset*)item.second;

		MeshAssetItem->ReleaseGALData();
		MeshAssetItem->ReleaseSystemData();
		delete MeshAssetItem;
	}

	_assetHashMap.Clear();
}
