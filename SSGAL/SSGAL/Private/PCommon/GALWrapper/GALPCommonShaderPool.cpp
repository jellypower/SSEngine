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
			{L"VS_SMToDefaultPSInput", L"Resource/Shader/VS_SMToDefaultPSInput.hlsl", "Main", EShaderType::VertexShader},
			{L"VS_SKMToDefaultPSInput", L"Resource/Shader/VS_SKMToDefaultPSInput.hlsl", "Main", EShaderType::VertexShader},
			{L"VS_FullScreenQuad", L"Resource/Shader/VS_FullScreenQuad.hlsl", "Main", EShaderType::VertexShader},

			{L"PS_DeferredShadeGBufferOutput", L"Resource/Shader/PS_DeferredShadeGBufferOutput.hlsl", "Main", EShaderType::PixelShader},
			{L"PS_IDDrawer", L"Resource/Shader/PS_IDDrawer.hlsl", "Main", EShaderType::PixelShader},

			{L"PS_DeferredShading", L"Resource/Shader/PS_DeferredShading.hlsl", "Main", EShaderType::PixelShader},

			{L"PS_SkyMap", L"Resource/Shader/PS_SkyMap.hlsl", "Main", EShaderType::PixelShader},

			{L"PS_DrawSimpleColor", L"Resource/Shader/PS_DrawSimpleColor.hlsl", "Main", EShaderType::PixelShader},
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
