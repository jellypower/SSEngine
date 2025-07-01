#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class GALRenderDevice;
class MeshAsset;

class GPUMeshAssetInstanceBase : public INoncopyable
{
protected:
	MeshAsset* _OwnerMeshAsset = nullptr;
	GALRenderDevice* _OwnerRenderDevice = nullptr;
};

