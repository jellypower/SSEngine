#include "CommonRenderAssetSet.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

void CommonRenderAssetSet::CacheCommonRenderAssets()
{
	IAssetManager* AssetManager = g_Renderer->GetAssetManager();

	_TexEMPTY = AssetManager->FindAssetByName<ITextureAsset>(L"EMPTY.tex");
	_TexWHITE = AssetManager->FindAssetByName<ITextureAsset>(L"WHITE.tex");
	_TexBLACK = AssetManager->FindAssetByName<ITextureAsset>(L"BLACK.tex");
	_TexEMPTYNORMAL = AssetManager->FindAssetByName<ITextureAsset>(L"EMPTYNORMAL.tex");
	_EmptyPBRMaterial = AssetManager->FindAssetByName<IMaterialAsset>(L"EMPTY.mtl");
}

void CommonRenderAssetSet::AddRefCachedAssets()
{
	AssetInstanceReferencer Referencer;
	Referencer.Type = EAssetInstanceReferenceType::AssetName;
	Referencer.AssetName = L"__COMMON_ASSET_REFERENCER__";

	_TexEMPTY->AddAssetReference(Referencer);
	_TexWHITE->AddAssetReference(Referencer);
	_TexBLACK->AddAssetReference(Referencer);
	_TexEMPTYNORMAL->AddAssetReference(Referencer);
	_EmptyPBRMaterial->AddAssetReference(Referencer);
}

void CommonRenderAssetSet::ReleaseCachedAssets()
{
	AssetInstanceReferencer Referencer;
	Referencer.Type = EAssetInstanceReferenceType::AssetName;
	Referencer.AssetName = L"__COMMON_ASSET_REFERENCER__";

	_TexEMPTY->RemoveAssetReference(Referencer);
	_TexWHITE->RemoveAssetReference(Referencer);
	_TexBLACK->RemoveAssetReference(Referencer);
	_TexEMPTYNORMAL->RemoveAssetReference(Referencer);
	_EmptyPBRMaterial->RemoveAssetReference(Referencer);
}
