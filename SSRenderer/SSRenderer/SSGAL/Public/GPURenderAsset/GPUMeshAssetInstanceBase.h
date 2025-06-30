#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class MeshAsset;

class GPUMeshAssetInstanceBase : public INoncopyable
{
protected:
	MeshAsset* _OwnerMeshAsset = nullptr;
};

