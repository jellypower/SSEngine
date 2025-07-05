#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"

class GALMaterialAssetWrapperBase;

class MaterialAsset : public IMaterialAsset
{
public:
	MaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);

public:
	virtual EAssetType GetAssetType() const override;
	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
};