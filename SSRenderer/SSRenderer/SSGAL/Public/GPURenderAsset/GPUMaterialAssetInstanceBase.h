#pragma once
#include "GPUAssetInstanceBase.h"

class MaterialAsset;

class GPUMaterialAssetInstanceBase : public GPUAssetInstanceBase
{
public:
	GPUMaterialAssetInstanceBase(MaterialAsset* ownerMaterial); // MODL: 분리
};
