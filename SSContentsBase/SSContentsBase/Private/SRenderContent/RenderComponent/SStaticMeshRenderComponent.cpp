#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SStaticMeshRenderComponent.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSRenderer/Public/RenderInstance/IRIMesh.h"

void SStaticMeshRenderComponent::ConstructRenderInstance()
{
	_RenderInstance = g_Renderer->CreateRIStaticMesh();
	_RenderInstance->SetGameObjectIDXXX(GetHashCode());
	SnycMeshRIWithAssetBindingIfExists();
}

void SStaticMeshRenderComponent::DestructRenderInstance()
{
	if (_RenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstance->ReleaseGALMetaData();
	_RenderInstance->Release();
}
