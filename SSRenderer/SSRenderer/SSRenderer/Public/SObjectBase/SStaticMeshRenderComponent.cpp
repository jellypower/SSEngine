#include "SStaticMeshRenderComponent.h"

#include "SSContentsBase/SGameObject.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"
#include "SSRenderer/Private/SSRendererGlobalVariablePrivate.h"
#include "SSRenderer/Public/RenderAsset/ModelAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/SSAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderAssetCommon/CommonDataType.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderInstance/BasicRenderInstance.h"

void SStaticMeshRenderComponent::ConstructRenderInstance()
{
	_RenderInstance = DBG_NEW BasicRenderInstance();

	ModelAssetManager* ModelAssetManager = SSRendererModule::Private::g_Renderer->GetModelAssetManager();

	SS_ASSERT(_ModelAssetName.IsEmpty() == false);
	SSAssetBase* FoundModelRef = ModelAssetManager->FindAssetByName(_ModelAssetName);
	if (FoundModelRef == nullptr || FoundModelRef->GetAssetType() != EAssetType::Model)
	{
		SS_ASSERT_MSG(false, L"Invalid Asset");
		return;
	}

	_RenderInstance->_Type = ERenderInstanceType::StaticMesh;
	_RenderInstance->_ModelRef = (ModelAsset*)FoundModelRef;
	SGameObject* Parent = GetParent();
	_RenderInstance->_GameObjectHashCode = Parent->GetHashCode();
}

void SStaticMeshRenderComponent::DestructRenderInstance()
{
	if (_RenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstance->ReleaseGPURenderInstance();
	delete _RenderInstance;
}
