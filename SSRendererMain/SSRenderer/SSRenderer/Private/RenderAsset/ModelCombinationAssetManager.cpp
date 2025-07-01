#include "SSRenderer/Public/RenderAsset/ModelCombinationAssetManager.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelCombinationAsset.h"

ModelCombinationAssetManager::ModelCombinationAssetManager():
	_assetHashMap(1000, 100)
{
}

ModelCombinationAssetManager::~ModelCombinationAssetManager()
{
}

ModelCombinationAsset* ModelCombinationAssetManager::FindMdlcByName(SS::SHasherW MdlcName) const
{
	ModelCombinationAsset* const* ppFoundMdlcAsset = _assetHashMap.Find(MdlcName);
	if (ppFoundMdlcAsset == nullptr)
	{
		return nullptr;
	}

	ModelCombinationAsset* FoundMdlcAsset = *ppFoundMdlcAsset;
	return FoundMdlcAsset;
}

void ModelCombinationAssetManager::AddToAssetPool(ModelCombinationAsset* newModelComb)
{
	_assetHashMap.Add(newModelComb->GetAssetName(), newModelComb);
}

void ModelCombinationAssetManager::ReleaseAllAssets()
{
	for (SS::pair<SS::SHasherW, ModelCombinationAsset*>& item : _assetHashMap)
	{
		delete item.second;
	}

	_assetHashMap.Clear();
}
