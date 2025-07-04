#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"


class IModelAssetMutable : public IModelAsset
{
public:
	virtual void SetMesh(IMeshAsset* InMeshAsset) = 0;
	virtual void SetMaterial(IMaterialAsset* InMaterialAsset, int32 materialIdx) = 0;
};