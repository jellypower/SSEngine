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
		IRICubeMap* RICubeMap = static_cast<IRICubeMap*>(_RenderInstance);

		IAssetManager* AssetManager = g_Renderer->GetAssetManager();

		ITextureAsset* FoundTexture = AssetManager->FindAssetByName<ITextureAsset>(InTexName);
		if (FoundTexture == nullptr)
		{
			SS_ASSERT(false);
			_TextureAssetName = SS::SHasherW::GetEmpty();
			return;
		}
		RICubeMap->SetCubemapTexture(FoundTexture);
		RICubeMap->SyncCubeMapTexture();
		_TextureAssetName = InTexName;
	}
	else
	{
		_TextureAssetName = InTexName;
	}
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

void SCubeMapRenderComponent::OnEnterTheWorld()
{
	SS_ASSERT(_RenderInstance->GetRIType() == ERenderInstanceType::CubeMap);
	IRICubeMap* InCubeMap = static_cast<IRICubeMap*>(_RenderInstance);

	AssetInstanceReferencer AssetReferencer;
	AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
	AssetReferencer.ObjHashCode = GetHashCode();

	ITextureAsset* TextureAsset = InCubeMap->GetCubemapTexture();
	SS_ASSERT(TextureAsset->GetTextureType() == ETextureType::CubeMap);
	TextureAsset->AddAssetReference(AssetReferencer);

	__super::OnEnterTheWorld();
}

void SCubeMapRenderComponent::OnExitTheWorld()
{
	__super::OnExitTheWorld();

	SS_ASSERT(_RenderInstance->GetRIType() == ERenderInstanceType::CubeMap);
	IRICubeMap* InCubeMap = static_cast<IRICubeMap*>(_RenderInstance);

	AssetInstanceReferencer AssetReferencer;
	AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
	AssetReferencer.ObjHashCode = GetHashCode();

	ITextureAsset* TextureAsset = InCubeMap->GetCubemapTexture();
	SS_ASSERT(TextureAsset->GetTextureType() == ETextureType::CubeMap);
	TextureAsset->RemoveAssetReference(AssetReferencer);
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
