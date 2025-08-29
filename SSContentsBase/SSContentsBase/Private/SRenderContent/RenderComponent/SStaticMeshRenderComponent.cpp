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
	IRIMesh* NewStaticMeshRI = g_Renderer->CreateRIStaticMesh();
	_RenderInstance = NewStaticMeshRI;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();

	SS_ASSERT(_ModelAssetName.IsEmpty() == false);
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);

	NewStaticMeshRI->SetModelAsset(FoundModelRef);
	NewStaticMeshRI->SetGameObjectIDXXX(GetHashCode());
}

void SStaticMeshRenderComponent::OnEnterTheWorld()
{
	__super::OnEnterTheWorld();

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = GetHashCode();

	IRIMesh* RIMesh = static_cast<IRIMesh*>(_RenderInstance);
	SS_ASSERT(_RenderInstance->GetRIType() == ERenderInstanceType::StaticMesh);
	IModelAsset* Model = RIMesh->GetModelAsset();
	Model->AddAssetReference(ThisAssetRef);
}

void SStaticMeshRenderComponent::OnExitTheWorld()
{
	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = GetHashCode();

	IRIMesh* RIMesh = static_cast<IRIMesh*>(_RenderInstance);
	SS_ASSERT(_RenderInstance->GetRIType() == ERenderInstanceType::StaticMesh);
	IModelAsset* Model = RIMesh->GetModelAsset();
	Model->RemoveAssetReference(ThisAssetRef);

	__super::OnExitTheWorld();
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
