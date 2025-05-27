#include "ShaderAssetManager.h"

#include "RenderAssetType/ShaderAsset.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"
#include "SSRenderer/Private/SSRendererGlobalVariablePrivate.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

ShaderAssetManager::ShaderAssetManager()
	: _shaderMap(ASSETMANAGER_DEFAULT_HASHMAP_CAPACITY)
{
}

ShaderAssetManager::~ShaderAssetManager()
{
}

ShaderAsset* ShaderAssetManager::FindShaderAsset(SS::SHasherW InShaderName) const
{
	ShaderAsset* const* ppShaderAsset = _shaderMap.Find(InShaderName);

	if(ppShaderAsset == nullptr)
	{
		return nullptr;
	}

	return *ppShaderAsset;
}

void ShaderAssetManager::InstantiateAllShaders()
{
	// TODO: 지금은 임의대로 Shader리스트를 관리하지만 나중에는 별도의 파일로 뺴서 관리하자
	ShaderAsset* newShader = nullptr;

	struct ShaderAssetConstructorDesc
	{
		SS::SHasherW InAssetName;
		SS::SHasherW InAssetPath;
		const char* entryPoint;
		EShaderType InShaderType;
	};

	ShaderAssetConstructorDesc DescSet[] =
	{
		{L"TempVertexShader", L"Resource/Shader/tempShader.hlsl", "VSMain", EShaderType::VertexShader},
		{L"TempPixelShader", L"Resource/Shader/tempShader.hlsl", "PSMain", EShaderType::PixelShader},
		{L"LambertShaderVS", L"Resource/Shader/LambertShader.hlsl", "VS", EShaderType::VertexShader},
		{L"LambertShaderPS", L"Resource/Shader/LambertShader.hlsl", "PS", EShaderType::PixelShader},
	};

	for (const ShaderAssetConstructorDesc& DescItem : DescSet)
	{
		ShaderAsset* newShader = DBG_NEW ShaderAsset(DescItem.InAssetName, DescItem.InAssetPath, DescItem.entryPoint, DescItem.InShaderType);
		_shaderMap.Add(DescItem.InAssetName, newShader);

		SSRendererModule::Private::g_Renderer->_GALRenderDevice->InstantiateShaderGPUAsset(newShader);
	}
}

void ShaderAssetManager::ReleaseAllShaders()
{
	for (SS::pair<SS::SHasherW, ShaderAsset*>& shaderItem : _shaderMap)
	{
		ShaderAsset* shaderAsset = shaderItem.second;
		shaderAsset->ReleaseGPUInstance();
		delete shaderAsset;
	}

	_shaderMap.Clear();
}
