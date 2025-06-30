#include "DX12GALShaderWrapper.h"

#include <d3dcompiler.h>

void DX12GALShaderWrapper::CompileShaderInstance()
{
	using namespace SS;

#ifdef _DEBUG
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ID3DBlob* errorBlob = nullptr;


	const wchar_t* AssetPathW = GetShaderPath().C_Str();


	const char* shaderTargetName = nullptr;
	switch (GetShaderType())
	{
	case EShaderType::Undefined:
		SS_CLASS_ERR_LOG("Invalid shader type.");
		return;
	case EShaderType::VertexShader:
		shaderTargetName = "vs_5_0";
		break;
	case EShaderType::PixelShader:
		shaderTargetName = "ps_5_0";
		break;
	}

	PooledList<D3D_SHADER_MACRO, InlineAllocator<DEFAULT_SHADER_MACRO_CNT_MAX>> DX12ShaderMacros;
	for (const SS::pair<SS::FixedStringA<SHADER_MACRO_DEFINE_STRLEN_MAX>, SS::FixedStringA<SHADER_MACRO_DEFINE_STRLEN_MAX>>&
		shaderMacroItem : GetShaderMacroSet())
	{
		DX12ShaderMacros.PushBack(D3D_SHADER_MACRO(shaderMacroItem.first.C_Str(), shaderMacroItem.second.C_Str()));
	}
	DX12ShaderMacros.PushBack(D3D_SHADER_MACRO(nullptr, nullptr));



	HRESULT hr = D3DCompileFromFile(
		AssetPathW,
		DX12ShaderMacros.GetData(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		GetEntryPointName().C_Str(),
		shaderTargetName,
		compileFlags,
		0,
		&_compiledShader,
		&errorBlob);

	if (FAILED(hr))
	{
		const char* errorStr = nullptr;
		_compiledShader = nullptr;
		if (errorBlob != nullptr)
		{
			errorStr = (char*)errorBlob->GetBufferPointer();
		}
		SS_CLASS_ERR_LOG("%s\n", errorStr);
		return;
	}

	return;
}

void DX12GALShaderWrapper::ReleaseShaderInstance()
{
	if (_compiledShader != nullptr)
	{
		_compiledShader->Release();
		_compiledShader = nullptr;
	}
}

