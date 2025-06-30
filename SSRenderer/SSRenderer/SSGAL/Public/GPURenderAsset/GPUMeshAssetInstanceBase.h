#pragma once
#include "GPUAssetInstanceBase.h"

class MeshAsset;

class GPUMeshAssetInstanceBase : public GPUAssetInstanceBase
{
public:
	GPUMeshAssetInstanceBase(MeshAsset* ownerMeshAsset); // MODL: ºÐ¸®
	virtual ~GPUMeshAssetInstanceBase();
};

