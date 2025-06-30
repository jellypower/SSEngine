#include "ShaderAssetManager.h"

#include "RenderAssetType/ShaderAsset.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"
#include "SSGAL/Public/GALWrapper/GALShaderPool.h"
#include "SSRenderer/Private/SSRendererGlobalVariablePrivate.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

ShaderAssetManager::ShaderAssetManager(SSRenderer* InOwnerRenderer) :
	_shaderMap(ASSETMANAGER_DEFAULT_HASHMAP_CAPACITY),
	_OwnerRenderer(InOwnerRenderer)

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
	const utf16* ShaderNames[] =
	{
		{L"TempVertexShader"},
		{L"TempPixelShader"},
		{L"LambertShaderVS"},
		{L"LambertShaderPS"},
	};

	GALRenderDevice* GALRenderDevice = _OwnerRenderer->_GALRenderDevice;
	GALShaderPool* ShaderPool = GALRenderDevice->GetShaderPool();
	for (const utf16* u16NewShaderName : ShaderNames)
	{
		SS::SHasherW NewShaderName = u16NewShaderName;

		GALShaderWrapper* ShaderWrapperItem = ShaderPool->FindShader(NewShaderName);

		if (ShaderWrapperItem == nullptr)
		{
			SS_ASSERT(false);
			continue;
		}

		ShaderAsset* newShader = DBG_NEW ShaderAsset(NewShaderName);
		_shaderMap.Add(NewShaderName, newShader);
	}
}

void ShaderAssetManager::ReleaseAllShaders()
{
	for (SS::pair<SS::SHasherW, ShaderAsset*>& shaderItem : _shaderMap)
	{
		ShaderAsset* shaderAsset = shaderItem.second;
		delete shaderAsset;
	}

	_shaderMap.Clear();
}
