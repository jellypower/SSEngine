#pragma once
#include "GPUAssetInstanceBase.h"

class MeshAsset;

class GPUMeshAssetInstanceBase : public GPUAssetInstanceBase
{
public:
	GPUMeshAssetInstanceBase(MeshAsset* ownerMeshAsset);
	virtual ~GPUMeshAssetInstanceBase();
};

