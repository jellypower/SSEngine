#include "pch.h"


#include "GALPCommonShaderPool.h"

#include "SSGAL/Private/DX12/GALWrapper/DX12GALShaderWrapper.h"
#include "SSGAL/Public/GALWrapper/GALShaderWrapper.h"


constexpr int32 TEMP_SHADERMAP_CAPACITY = 5000;
GALPCommonShaderPool::GALPCommonShaderPool() :
	_ShaderMap(TEMP_SHADERMAP_CAPACITY)
{
}

GALPCommonShaderPool::~GALPCommonShaderPool()
{
	for (SS::pair<SS::SHasherW, GALShaderWrapper*>& Item : _ShaderMap)
	{
		GALShaderWrapper* ShaderItem = Item.second;
		ShaderItem->ReleaseShaderInstance();
		delete ShaderItem;
	}
}


void GALPCommonShaderPool::Initialize()
{
	// TEMP: InitializeShader
	{
		ShaderConstructDesc DescSet[] =
		{
			{L"TempVertexShader", L"Resource/Shader/tempShader.hlsl", "VSMain", EShaderType::VertexShader},
			{L"TempPixelShader", L"Resource/Shader/tempShader.hlsl", "PSMain", EShaderType::PixelShader},
			{L"LambertShaderVS", L"Resource/Shader/LambertShader.hlsl", "VS", EShaderType::VertexShader},
			{L"LambertShaderPS", L"Resource/Shader/LambertShader.hlsl", "PS", EShaderType::PixelShader},
		};

		for (const ShaderConstructDesc& DescItem : DescSet)
		{
			GALShaderWrapper* newShader = CreateShader(DescItem);
			_ShaderMap.Add(DescItem.InAssetName, newShader);

			newShader->CompileShaderInstance();
		}
	}
}



GALShaderWrapper* GALPCommonShaderPool::FindShader(SS::SHasherW InShaderName) const
{
	GALShaderWrapper* const* ppWrapper = _ShaderMap.Find(InShaderName);
	if (ppWrapper == nullptr)
	{
		return nullptr;
	}

	GALShaderWrapper* OutShader = *ppWrapper;

	if (OutShader == nullptr)
	{
		SS_ASSERT(false);
		return nullptr;
	}
	return OutShader;
}
