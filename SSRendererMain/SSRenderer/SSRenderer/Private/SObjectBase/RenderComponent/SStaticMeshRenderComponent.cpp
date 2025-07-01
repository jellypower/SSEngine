#include "SSRenderer/Public/SObjectBase/RenderComponent/SStaticMeshRenderComponent.h"

#include "SSContentsBase/SGameObject.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"
#include "SSRenderer/Private/SSRendererGlobalVariablePrivate.h"
#include "SSRenderer/Private/RenderInstance/RIStaticMesh.h"
#include "SSRenderer/Public/RenderAsset/ModelAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/SSAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderAssetCommon/CommonDataType.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"

void SStaticMeshRenderComponent::ConstructRenderInstance()
{

	RIStaticMesh* NewStaticMeshRI = DBG_NEW RIStaticMesh();
	_RenderInstance = NewStaticMeshRI;

	ModelAssetManager* ModelAssetManager = SSRendererModule::Private::g_Renderer->GetModelAssetManager();

	SS_ASSERT(_ModelAssetName.IsEmpty() == false);
	SSAssetBase* FoundModelRef = ModelAssetManager->FindAssetByName(_ModelAssetName);
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
