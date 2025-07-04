#include "SRenderContent/Public/RenderComponent/SStaticMeshRenderComponent.h"

#include "SSContentsBase/SGameObject.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"

#include "SSRenderer/Private/RenderInstance/RIStaticMesh.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderAssetCommon/CommonDataType.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"

void SStaticMeshRenderComponent::ConstructRenderInstance()
{

	RIStaticMesh* NewStaticMeshRI = DBG_NEW RIStaticMesh();
	_RenderInstance = NewStaticMeshRI;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();

	SS_ASSERT(_ModelAssetName.IsEmpty() == false);
	IAssetBase* FoundModelRef = AssetManager->FindAssetByName(_ModelAssetName, EAssetType::Model);
	if (FoundModelRef == nullptr || FoundModelRef->GetAssetType() != EAssetType::Model)
	{
		SS_ASSERT_MSG(false, L"Invalid Asset");
		return;
	}


	SGameObject* Parent = GetParent();
	NewStaticMeshRI->_ModelRef = (ModelAsset*)FoundModelRef;
	NewStaticMeshRI->_GameObjectHashCode = Parent->GetHashCode();
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
