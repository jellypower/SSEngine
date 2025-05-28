#include "SRendererUtil.h"

#include "SRenderComponentBase.h"
#include "SStaticMeshRenderComponent.h"
#include "SSContentsBase/SGameObjectConstructor.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSRenderer/Private/SSRendererGlobalVariablePrivate.h"
#include "SSRenderer/Public/RenderAsset/ModelAssetManager.h"
#include "SSRenderer/Public/RenderAsset/ModelCombinationAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelCombinationAsset.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"

SGameObject* SRendererUtil::InstantiateModelObjTree(SS::SHasherW MdlcAssetName)
{
	const ModelCombinationAssetManager* MdlcAssetManager = SSRendererModule::Private::g_Renderer->GetModelCombinationAssetManager();
	const ModelCombinationAsset* MdlcAsset = MdlcAssetManager->FindMdlcByName(MdlcAssetName);
	if (MdlcAsset == nullptr)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	SGameObject* NewGameObj = NewSObject<SGameObject>(MdlcAssetName);

	if (MdlcAsset->GetChildCnt() == 2) // 단일 모델이면
	{
		const AssetPlacementReference& AssetPlacement = MdlcAsset->GetChildAt(1);

		switch (AssetPlacement.MeshType)
		{
		case EMeshType::None:
		{

		}
		break;
		case EMeshType::Rigid:
		{
			SRenderComponentBase* NewRenderComponent = NewGameObj->CreateComponent<SStaticMeshRenderComponent>(MdlcAsset->GetAssetName());
			NewRenderComponent->SetModelAsset(AssetPlacement.AssetName);
			NewRenderComponent->PostConstructHierarchy();
		}
		break;
		case EMeshType::Skinned:
		{
			SS_ASSERT(false); // TODO: Skinning 구현하기 2024/12/31
		}
		break;
		}

		return NewGameObj;
	}
	else
	{
		InstantiateModelObjTree_Recursion(MdlcAsset, MDLC_PLACEMENTREF_ROOT_IDX, NewGameObj);
		SGameObjectConstructor::FinishConstructHierarchy(NewGameObj);
		return NewGameObj;
	}
}

SGameObject* SRendererUtil::InstantiateModel(SS::SHasherW ModelAssetName)
{
	const ModelAssetManager* MdlAssetManager = SSRendererModule::Private::g_Renderer->GetModelAssetManager();
	const ModelAsset* lModelAsset = (ModelAsset*)MdlAssetManager->FindAssetByName(ModelAssetName);
	if (lModelAsset == nullptr)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	SGameObject* NewGameObj = NewSObject<SGameObject>(ModelAssetName);
	SStaticMeshRenderComponent* NewStaticMeshComp = NewGameObj->CreateComponent<SStaticMeshRenderComponent>(lModelAsset->GetAssetName());
	NewStaticMeshComp->SetModelAsset(lModelAsset->GetAssetName());
	NewStaticMeshComp->PostConstructHierarchy();

	return NewGameObj;
}

void SRendererUtil::InstantiateModelObjTree_Recursion(const ModelCombinationAsset* MdlcAsset, int32 CurAssetIdx,
                                                      SGameObject* CurGameObject)
{
	const AssetPlacementReference& ThisAssetPlacement = MdlcAsset->GetChildAt(CurAssetIdx);

	for (int32 ChildIdx : ThisAssetPlacement.ChildIndices)
	{
		const AssetPlacementReference& ChildAssetPlacement = MdlcAsset->GetChildAt(ChildIdx);
		SGameObject* NewChildObj = NewSObject<SGameObject>(ChildAssetPlacement.PlacementName);
		NewChildObj->SetParent(CurGameObject);
		NewChildObj->SetTransform(ChildAssetPlacement.Transform);
		if (ChildAssetPlacement.AssetName.IsEmpty() == false)
		{
			SRenderComponentBase* NewRenderComp = NewChildObj->CreateComponent<SStaticMeshRenderComponent>(ChildAssetPlacement.PlacementName);
			NewRenderComp->SetModelAsset(ChildAssetPlacement.AssetName);
		}
		InstantiateModelObjTree_Recursion(MdlcAsset, ChildIdx, NewChildObj);
	}

}
