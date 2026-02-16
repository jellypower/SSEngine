#include "SSContentsBase/Public/SRenderContent/RenderComponent/SMeshRenderComponentBase.h"

#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"

void SMeshRenderComponentBase::SetModelAsset(SS::SHasherW ModelAssetName)
{
	_ModelAssetName = ModelAssetName;
}


void SMeshRenderComponentBase::ApplyModelAssetChange()
{
	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);
	dynamic_cast<IRIMesh*>(_RenderInstance)->SetModelAsset(FoundModelRef);
}
