#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SSkinnedMeshRenderComponent.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRISkinnedMesh.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"

void SSkinnedMeshRenderComponent::ConstructRenderInstance()
{
	IRISkinnedMesh* NewStaticMeshRI = g_Renderer->CreateRISkinnedMesh();
	_RenderInstance = NewStaticMeshRI;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();

	SS_ASSERT(_ModelAssetName.IsEmpty() == false);
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);


	SGameObject* Parent = GetParent();
	NewStaticMeshRI->SetModelAsset(FoundModelRef);
	NewStaticMeshRI->SetGameObjectIDXXX(Parent->GetHashCode());
}

void SSkinnedMeshRenderComponent::DestructRenderInstance()
{
	if (_RenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstance->ReleaseGALMetaData();
	delete _RenderInstance;
}
