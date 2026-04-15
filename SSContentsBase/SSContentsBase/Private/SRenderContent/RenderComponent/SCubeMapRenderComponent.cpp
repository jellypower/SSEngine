#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SCubeMapRenderComponent.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSRenderer/Public/RenderInstance/IRICubeMap.h"

SS::SHasherW SCubeMapRenderComponent::GetCubeMapTextureAssetName() const
{
	return _TextureAssetName;
}

void SCubeMapRenderComponent::SetCubeMapTextureAssetName(SS::SHasherW InTexName)
{
	if (_RenderInstance != nullptr)
	{
		IAssetManager* AssetManager = g_Renderer->GetAssetManager();
		ITextureAsset* FoundTexture = AssetManager->FindAssetByName<ITextureAsset>(InTexName);
		if (FoundTexture == nullptr)
		{
			SS_ASSERT(false);
			_TextureAssetName = SS::SHasherW::GetEmpty();
			return;
		}

		IRICubeMap* RICubeMap = static_cast<IRICubeMap*>(_RenderInstance);
		RICubeMap->SetCubemapTexture(FoundTexture);
	}

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
	NewCubeMapRI->SetGameObjectIDXXX(GetHashCode());
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
