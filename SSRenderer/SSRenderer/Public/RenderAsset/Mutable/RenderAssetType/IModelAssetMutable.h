#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"


class IModelAssetMutable : public IModelAsset
{
public:
	virtual void SetMesh(SS::SHasherW InMeshAssetName) = 0;
	virtual void SetMaterial(SS::SHasherW InMaterialAssetName, int32 materialIdx) = 0;
};