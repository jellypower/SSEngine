#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSRenderer/Public/RenderInstance/IRICubeMap.h"

void SCubeMapRenderComponent::SetCubeMapTextureAssetName(SS::SHasherW InTexName)
{
	_TextureAssetName = InTexName;
}

void SCubeMapRenderComponent::ConstructRenderInstance()
{
	IRICubeMap* NewCubeMapRI = g_Renderer->CreateRICubeMap();
	_RenderInstance = NewCubeMapRI;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();

	SS_ASSERT(_TextureAssetName.IsEmpty() == false);
	ITextureAsset* FoundTexture = AssetManager->FindAssetByName<ITextureAsset>(_TextureAssetName);

	NewCubeMapRI->SetCubemapTexture(FoundTexture);
}

void SCubeMapRenderComponent::DestructRenderInstance()
{
	if (_RenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstance->ReleaseGALMetaData();
	delete _RenderInstance;
}
