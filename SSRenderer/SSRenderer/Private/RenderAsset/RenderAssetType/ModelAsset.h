#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"


class ModelAsset : public IModelAssetMutable
{
public:
	ModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath);

public:
	EAssetType GetAssetType() const override;

	virtual IMeshAsset* GetMeshAsset() const override;
	virtual IMaterialAsset* GetMaterialAsset(int32 materialIdx) const override;
	virtual SS::SHasherW GetMeshAssetName() const override;
	virtual SS::SHasherW GetMaterialAssetName(int32 materialIdx) const override;
	virtual int32 GetSubMeshCnt() const override;


public:
	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	virtual void BindAssetManager(IAssetManager* InAssetManager) override;

public:
	virtual void SetMesh(SS::SHasherW InMeshAssetName) override;
	virtual void SetMaterial(SS::SHasherW InMaterialAssetName, int32 InMaterialIdx) override;

private:
	SS::SHasherW _MeshAssetName;
	SS::SHasherW _MaterialAssetNames[SUBMESH_COUNT_MAX];

	int32 _SubMeshCntCache = 0;
	IMeshAsset* _MeshAssetCache = nullptr;
	IMaterialAsset* _MaterialAssetCache[SUBMESH_COUNT_MAX] = { nullptr, };
};
