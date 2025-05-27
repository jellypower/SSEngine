#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class ModelCombinationAsset;

class ModelCombinationAssetManager : public INoncopyable
{
private:
	SS::HashMap<SS::SHasherW, ModelCombinationAsset*> _assetHashMap;

public:
	ModelCombinationAssetManager();
	virtual ~ModelCombinationAssetManager();

	ModelCombinationAsset* FindMdlcByName(SS::SHasherW MdlcName) const;

	void AddToAssetPool(ModelCombinationAsset* newModelComb);
	void ReleaseAllAssets();
};
