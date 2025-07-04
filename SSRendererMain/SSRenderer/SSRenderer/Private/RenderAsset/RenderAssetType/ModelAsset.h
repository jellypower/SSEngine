#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"


class ModelAsset : public IModelAssetMutable
{
public:
	ModelAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);

public:
	EAssetType GetAssetType() const override;

	void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;

	int32 GetSubMeshCnt() const override;

	void SetMesh(IMeshAsset* InMeshAsset) override;
	void SetMaterial(IMaterialAsset* InMaterialAsset, int32 InMaterialIdx) override;
};
