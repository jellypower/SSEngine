#include "GPUMaterialAssetInstanceBase.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MaterialAsset.h"

GPUMaterialAssetInstanceBase::GPUMaterialAssetInstanceBase(MaterialAsset* ownerMaterial)
{
	_ownerAsset = ownerMaterial;
}
