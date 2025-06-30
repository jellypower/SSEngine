#pragma once
#include <d3d12.h>

#include "SSGAL/Public/GPURenderAsset/GPUShaderAssetInstanceBase.h"

class DX12GPUShaderAssetInstance : public GPUShaderAssetInstanceBase
{
public:
	DX12GPUShaderAssetInstance(ShaderAsset* ownerShader);
	virtual ~DX12GPUShaderAssetInstance();

	virtual bool IsValid() const override;

	ID3DBlob* GetCompiledShader() const { return _compiledShader; }

private:
	ID3DBlob* _compiledShader = nullptr;
};
