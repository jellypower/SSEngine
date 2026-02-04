#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelCombinationAssetMutable.h"



class ModelCombinationAsset : public IModelCombinationAssetMutable
{
public:
	ModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt);

public:
	EAssetType GetAssetType() const override;

	void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;

	void AddNewChild(const AssetPlacementReference& newReference) override;
};
