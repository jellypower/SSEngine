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

	SGameObject* NewGameObjRoot = NewSObject<SGameObject>(MdlcAssetName);

	if (MdlcAsset->GetChildCnt() == 2) // 단일 모델이면 -> GetChildCnt중 1개는 루트오브젝트, 1개는 실제 인스턴스
	{
		const AssetPlacementReference& AssetPlacement = MdlcAsset->GetChildAt(1);

		if (AssetPlacement.MeshType == EMeshType::Rigid)
		{
			SMeshRenderComponentBase* NewRenderComponent = NewGameObjRoot->CreateComponent<SStaticMeshRenderComponent>(MdlcAsset->GetAssetName());
			NewRenderComponent->SetModelAsset(AssetPlacement.AssetName);
			NewRenderComponent->PostConstructHierarchy();
		}
		else
		{
			SS_ASSERT(false); // 일어나면 안되는 상황
		}

		return NewGameObjRoot;
	}
	else
	{
		NewGameObjRoot->SetStrongBindAncestor(NewGameObjRoot);
		int32 ChildCnt = MdlcAsset->GetChildCnt();
		SS::PooledList<SGameObject*> GameObjectsCreation(ChildCnt);

		for (int32 i = 0; i < ChildCnt; i++)
		{
			const AssetPlacementReference& ThisAssetPlacement = MdlcAsset->GetChildAt(i);
			SGameObject* NewChildObj = NewSObject<SGameObject>(ThisAssetPlacement.PlacementName);
			NewChildObj->SetStrongBindAncestor(NewGameObjRoot);
			NewChildObj->SetTransform(ThisAssetPlacement.Transform);

			GameObjectsCreation.PushBack(NewChildObj);


			if (ThisAssetPlacement.AssetName.IsEmpty() == false)
			{
				if (ThisAssetPlacement.MeshType == EMeshType::Rigid)
				{
					SMeshRenderComponentBase* NewRenderComp = NewChildObj->CreateComponent<SStaticMeshRenderComponent>(ThisAssetPlacement.PlacementName);
					NewRenderComp->SetModelAsset(ThisAssetPlacement.AssetName);
				}
				else if (ThisAssetPlacement.MeshType == EMeshType::Skinned)
				{
					SMeshRenderComponentBase* NewRenderComp = NewChildObj->CreateComponent<SSkinnedMeshRenderComponent>(ThisAssetPlacement.PlacementName);
					NewRenderComp->SetModelAsset(ThisAssetPlacement.AssetName);
				}
				else
				{
					SS_ASSERT(false);
				}
			}
		}

		for (int32 i = 0; i < ChildCnt; i++)
		{
			const AssetPlacementReference& ThisAssetPlacement = MdlcAsset->GetChildAt(i);
			int32 ParentIdx = ThisAssetPlacement.ParentIdx;

			SGameObject* ThisGameObject = GameObjectsCreation[i];

			SGameObject* ParentGameObject = NewGameObjRoot; // 부모가 없으면 루트에 바로 등록
			if (ParentIdx != -1)
			{
				ParentGameObject = GameObjectsCreation[ParentIdx]; // 부모를 오버라이드
			}

			ThisGameObject->SetParent(ParentGameObject);
		}


		SGameObjectConstructor::FinishConstructHierarchy(NewGameObjRoot);
		return NewGameObjRoot;
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