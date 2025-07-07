#include "SRenderContent/Public/RenderComponent/SStaticMeshRenderComponent.h"

#include "SSContentsBase/Public/SGameObject.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSRenderer/Public/RenderInstance/IRIMesh.h"

void SStaticMeshRenderComponent::ConstructRenderInstance()
{
	IRIMesh* NewStaticMeshRI = g_Renderer->CreateRIStaticMesh();
	_RenderInstance = NewStaticMeshRI;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();

	SS_ASSERT(_ModelAssetName.IsEmpty() == false);
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);

	SGameObject* Parent = GetParent();
	NewStaticMeshRI->SetModelAsset(FoundModelRef);
	NewStaticMeshRI->SetGameObjectHashCodeXXX(Parent->GetHashCode());
}

void SStaticMeshRenderComponent::DestructRenderInstance()
{
	if (_RenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstance->ReleaseGALMetaData();
	delete _RenderInstance;
}
