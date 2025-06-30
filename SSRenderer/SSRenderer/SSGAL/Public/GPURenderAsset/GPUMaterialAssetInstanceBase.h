#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class MaterialAsset;

class GPUMaterialAssetInstanceBase : public INoncopyable
{
protected:
	MaterialAsset* _OwnerMaterialAsset = nullptr;
};
