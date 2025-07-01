#include "SSRenderer/Public/RenderAsset/MaterialAssetManager.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderAssetCommon/CommonDataType.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"



MaterialAssetManager::MaterialAssetManager(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity)
	: AssetManagerBase(AssetHashMapCapacity, AssetHashMapBucketCapacity)
{
}

MaterialAssetManager::~MaterialAssetManager()
{
}

MaterialAsset* MaterialAssetManager::FindMaterialAssetWithName(SS::SHasherW InAssetName) const
{
	SSAssetBase* const* ppAsset = _assetHashMap.Find(InAssetName);;

	if(ppAsset == nullptr)
	{
		return nullptr;
	}

	MaterialAsset* OutMaterialAsset = (MaterialAsset*)(*ppAsset);

	return OutMaterialAsset;
}

void MaterialAssetManager::InstantiateAllMaterialAssets()
{

}

void MaterialAssetManager::ReleaseAllMaterials()
{
	for (SS::pair<SS::SHasherW, SSAssetBase*> item : _assetHashMap)
	{
		if (item.second == nullptr)
		{
			SS_ASSERT_MSG(false, L"에셋이 비어있습니다.");
			continue;
		}

		if (item.second->GetAssetType() != EAssetType::Material)
		{
			SS_ASSERT_MSG(false, L"Invalid Asset.");
			continue;
		}

		MaterialAsset* MaterialAssetItem = (MaterialAsset*)item.second;

		delete MaterialAssetItem;
	}

	_assetHashMap.Clear();
}
