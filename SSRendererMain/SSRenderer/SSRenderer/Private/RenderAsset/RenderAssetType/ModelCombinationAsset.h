#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelCombinationAsset.h"



class ModelCombinationAsset : public IModelCombinationAsset
{
public:
	ModelCombinationAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt);

public:
	EAssetType GetAssetType() const override;

	void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;

	void AddNewChild(const AssetPlacementReference& newReference) override;
};
