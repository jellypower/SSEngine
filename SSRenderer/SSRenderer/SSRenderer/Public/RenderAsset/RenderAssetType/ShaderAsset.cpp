#include "ShaderAsset.h"

#include "SSGAL/Public/GPURenderAsset/GPUShaderAssetInstanceBase.h"


ShaderAsset::ShaderAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, const char* entryPoint, EShaderType InShaderType):
	SSAssetBase(EAssetType::Shader, InAssetName, InAssetPath)
{
	_entryPointName = entryPoint;
	_shaderType = InShaderType;
}

void ShaderAsset::ReleaseGPUInstance()
{
	if (_shaderGPUAssetInstance != nullptr)
	{
		delete _shaderGPUAssetInstance;
	}
	_shaderGPUAssetInstance = nullptr;
}

void ShaderAsset::SetShaderMacros(const ShaderMacroSet& inShaderMacro)
{
	_shaderMacro = inShaderMacro;
}

void ShaderAsset::SetShaderMacros(ShaderMacroSet&& inShaderMacro)
{
	_shaderMacro = SS::move(inShaderMacro);
}