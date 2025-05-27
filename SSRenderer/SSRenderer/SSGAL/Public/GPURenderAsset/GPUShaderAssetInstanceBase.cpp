#include "GPUShaderAssetInstanceBase.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ShaderAsset.h"

GPUShaderAssetInstanceBase::GPUShaderAssetInstanceBase(ShaderAsset* ownerShaderAsset)
{
	_ownerAsset = ownerShaderAsset;
}
