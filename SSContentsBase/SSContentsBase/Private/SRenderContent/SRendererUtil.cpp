#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"

#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SSkinnedMeshRenderComponent.h"

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"

#include "SSContentsBase/Public/SRenderContent/RenderComponent/SStaticMeshRenderComponent.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelCombinationAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

SGameObject* SRendererUtil::InstantiateModelObjTree(SS::SHasherW MdlcAssetName)
{
	const IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	const IModelCombinationAsset* MdlcAsset = AssetManager->FindAssetByName<IModelCombinationAsset>(MdlcAssetName);
	if (MdlcAsset == nullptr)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	SGameObject* NewGameObj = NewSObject<SGameObject>(MdlcAssetName);

	if (MdlcAsset->GetChildCnt() == 2) // 단일 모델이면 -> GetChildCnt중 1개는 루트오브젝트, 1개는 실제 인스턴스
	{
		const AssetPlacementReference& AssetPlacement = MdlcAsset->GetChildAt(1);

		if (AssetPlacement.MeshType == EMeshType::Rigid)
		{
			SMeshRenderComponentBase* NewRenderComponent = NewGameObj->CreateComponent<SStaticMeshRenderComponent>(MdlcAsset->GetAssetName());
			NewRenderComponent->SetModelAsset(AssetPlacement.AssetName);
			NewRenderComponent->PostConstructHierarchy();
		}
		else
		{
			SS_ASSERT(false); // 일어나면 안되는 상황
		}

		return NewGameObj;
	}
	else
	{
		NewGameObj->SetStrongBindAncestor(NewGameObj);
		InstantiateModelObjTree_Recursion(MdlcAsset, MDLC_PLACEMENTREF_ROOT_IDX, NewGameObj, NewGameObj);
		SGameObjectConstructor::FinishConstructHierarchy(NewGameObj);
		return NewGameObj;
	}
}

SGameObject* SRendererUtil::InstantiateModel(SS::SHasherW ModelAssetName, SS::SHasherW ObjectNameOverride)
{
	const IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	const IModelAsset* lModelAsset = AssetManager->FindAssetByName<IModelAsset>(ModelAssetName);
	if (lModelAsset == nullptr)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	SGameObject* NewGameObj = nullptr;
	if (ObjectNameOverride.IsEmpty())
	{
		NewGameObj = NewSObject<SGameObject>(ModelAssetName);
	}
	else
	{
		NewGameObj = NewSObject<SGameObject>(ObjectNameOverride);
	}

	SStaticMeshRenderComponent* NewStaticMeshComp = NewGameObj->CreateComponent<SStaticMeshRenderComponent>(lModelAsset->GetAssetName());
	NewStaticMeshComp->SetModelAsset(lModelAsset->GetAssetName());
	NewStaticMeshComp->PostConstructHierarchy();

	return NewGameObj;
}

void SRendererUtil::InstantiateModelObjTree_Recursion(const IModelCombinationAsset* MdlcAsset, int32 CurAssetIdx,
                                                      SGameObject* ParentObject, SGameObject* StrongBindAncestor)
{
	const AssetPlacementReference& ThisAssetPlacement = MdlcAsset->GetChildAt(CurAssetIdx);

	for (int32 ChildIdx : ThisAssetPlacement.ChildIndices)
	{
		const AssetPlacementReference& ChildAssetPlacement = MdlcAsset->GetChildAt(ChildIdx);
		SGameObject* NewChildObj = NewSObject<SGameObject>(ChildAssetPlacement.PlacementName);
		NewChildObj->SetParent(ParentObject);
		NewChildObj->SetStrongBindAncestor(StrongBindAncestor);
		NewChildObj->SetTransform(ChildAssetPlacement.Transform);

		if(ChildAssetPlacement.AssetName.IsEmpty() == false)
		{
			if (ChildAssetPlacement.MeshType == EMeshType::Rigid)
			{
				SMeshRenderComponentBase* NewRenderComp = NewChildObj->CreateComponent<SStaticMeshRenderComponent>(ChildAssetPlacement.PlacementName);
				NewRenderComp->SetModelAsset(ChildAssetPlacement.AssetName);
			}
			else if (ChildAssetPlacement.MeshType == EMeshType::Skinned)
			{
				SMeshRenderComponentBase* NewRenderComp = NewChildObj->CreateComponent<SSkinnedMeshRenderComponent>(ChildAssetPlacement.PlacementName);
				NewRenderComp->SetModelAsset(ChildAssetPlacement.AssetName);
			}
			else
			{
				SS_ASSERT(false);
			}
		}
		else
		{
//			SRenderComponentBase* NewRenderComp = NewChildObj->CreateComponent<SStaticMeshRenderComponent>(ChildAssetPlacement.PlacementName);
//			NewRenderComp->SetModelAsset("directionmesh/direction.mdl");
		}
		
		InstantiateModelObjTree_Recursion(MdlcAsset, ChildIdx, NewChildObj, StrongBindAncestor);
	}

}
