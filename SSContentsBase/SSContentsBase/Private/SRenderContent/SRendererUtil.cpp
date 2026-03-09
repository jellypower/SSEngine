#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/SRendererUtil.h"


#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SSkinnedMeshRenderComponent.h"

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"

#include "SSContentsBase/Public/SRenderContent/RenderComponent/SStaticMeshRenderComponent.h"

#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelCombinationAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

SGameObject* SRendererUtil::InstantiateMDLC(SS::SHasherW MdlcAssetName)
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
		SS::PooledList<SGameObject*, SS::InlineAllocator<200>> GameObjectsCreation(ChildCnt);


		{
			SCOPE_PROFILE(InstantiateObjs);
			// 오브젝트들 만들기
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
		}

		{
			SCOPE_PROFILE(ConstructParents);
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

		}


		{
			SCOPE_PROFILE(FinishMdlcConstruct);
			SGameObjectConstructor::FinishConstructHierarchy(NewGameObjRoot);
		}
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

SGameObject* SRendererUtil::InstantiateMesh(SS::SHasherW MeshAssetName, SS::SHasherW ObjectNameOverride)
{
	const IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	const ICommonRenderAssetSet* CommRenderAssets = g_Renderer->GetCommonRenderAssetSet();
	const IMeshAsset* lMeshAsset = AssetManager->FindAssetByName<IMeshAsset>(MeshAssetName);
	if (lMeshAsset == nullptr)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	SGameObject* NewGameObj = nullptr;
	if (ObjectNameOverride.IsEmpty())
	{
		NewGameObj = NewSObject<SGameObject>(MeshAssetName);
	}
	else
	{
		NewGameObj = NewSObject<SGameObject>(ObjectNameOverride);
	}

	SStaticMeshRenderComponent* NewStaticMeshComp = NewGameObj->CreateComponent<SStaticMeshRenderComponent>(L"MeshAssetName");
	NewStaticMeshComp->SetMeshAsset(MeshAssetName);

	static const SS::SHasherW EmptyMtlAssetName = CRAN::EMPTY_PBR_MTL;
	const int32 SubMeshCnt = lMeshAsset->GetSubMeshCnt();
	for (int32 i=0;i<SubMeshCnt;i++)
	{
		NewStaticMeshComp->SetMaterialAsset(EmptyMtlAssetName, i);
	}
	NewStaticMeshComp->PostConstructHierarchy();

	return NewGameObj;
}
