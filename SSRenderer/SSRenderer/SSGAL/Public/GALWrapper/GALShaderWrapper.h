#pragma once

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"

#include "SSEngineDefault/Public/SSContainer/Allocators/InlineAllocator.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSEngineDefault/Public/SSContainer/SSString/FixedStringA.h"
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SHasher/SHasherW.h"



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


class GALShaderWrapper : public INoncopyable
{
public:
	GALShaderWrapper(SS::SHasherW InShaderName, const utf16* InShaderPath, const char* entryPoint, EShaderType InShaderType); // MODL: 분리

	SS::SHasherW GetShaderName() const { return _ShaderName; }
	const SS::StringW& GetShaderPath() const { return _ShaderPath; }
	EShaderType GetShaderType() const { return _shaderType; }
	const ShaderMacroSet& GetShaderMacroSet() const { return _shaderMacro; }
	const SS::FixedStringA<256>& GetEntryPointName() const { return _entryPointName; }

	void SetShaderMacros(const ShaderMacroSet& InShaderMacro) { _shaderMacro = InShaderMacro; }
	void SetShaderMacros(ShaderMacroSet&& inShaderMacro) { _shaderMacro = SS::move(inShaderMacro); }

public:
	virtual void CompileShaderInstance() = 0;
	virtual void ReleaseShaderInstance() = 0;


private:
	SS::SHasherW _ShaderName;
	SS::StringW _ShaderPath;

	SS::FixedStringA<256> _entryPointName;
	ShaderMacroSet _shaderMacro;
	EShaderType _shaderType;
};