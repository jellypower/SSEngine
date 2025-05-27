#pragma once
#include "GPUAssetInstanceBase.h"

class ShaderAsset;

class GPUShaderAssetInstanceBase : public GPUAssetInstanceBase
{
public:
	GPUShaderAssetInstanceBase(ShaderAsset* ownerShaderAsset);
};
