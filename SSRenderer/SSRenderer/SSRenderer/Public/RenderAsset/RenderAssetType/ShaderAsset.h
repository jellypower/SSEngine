#pragma once
#include "SSAssetBase.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSRenderer/Public/SSRendererBuildSetings.h"
#include "SSEngineDefault/Public/SSContainer/FixedList.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSEngineDefault/Public/SSContainer/Allocators/InlineAllocator.h"


class GPUShaderAssetInstanceBase;
constexpr int32 DEFAULT_SHADER_MACRO_CNT_MAX = 8;
constexpr int32 SHADER_MACRO_DEFINE_STRLEN_MAX = 32;
constexpr int32 SHADER_INCLUDE_STRLEN_MAX = 260;


enum class EShaderType
{
	Undefined,

	VertexShader,
	PixelShader,
};

typedef SS::PooledList<
	SS::pair<SS::FixedStringA<SHADER_MACRO_DEFINE_STRLEN_MAX>, SS::FixedStringA<SHADER_MACRO_DEFINE_STRLEN_MAX>>,
	SS::InlineAllocator<DEFAULT_SHADER_MACRO_CNT_MAX>> ShaderMacroSet;

class ShaderAsset : public SSAssetBase
{
public:
	ShaderAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, const char* entryPoint, EShaderType InShaderType);

	const GPUShaderAssetInstanceBase* GetGPUInstance() const { return _shaderGPUAssetInstance;  }
	bool IsUsableOnGPU() const { return _shaderGPUAssetInstance != nullptr; }
	void ReleaseGPUInstance();

	const SS::FixedStringA<256>& GetEntryPointName() { return _entryPointName;  }
	const ShaderMacroSet& GetShaderMacros() const { return _shaderMacro; }
	EShaderType GetShaderType() const { return _shaderType; }
	

	void SetShaderMacros(const ShaderMacroSet& inShaderMacro);
	void SetShaderMacros(ShaderMacroSet&& inShaderMacro);

public:
	GPUShaderAssetInstanceBase* _shaderGPUAssetInstance = nullptr;

private:
	SS::FixedStringA<256> _entryPointName;
	ShaderMacroSet _shaderMacro;
	EShaderType _shaderType;

};