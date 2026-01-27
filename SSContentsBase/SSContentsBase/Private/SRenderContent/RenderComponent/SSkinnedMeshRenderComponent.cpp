#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SSkinnedMeshRenderComponent.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRISkinnedMesh.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"


constexpr int32 TEMP_BONE_BASIC_CAPACITY = 200;

bool SSkinnedMeshRenderComponent::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SSkinnedMeshRenderComponent::PerFrame()
{
	SGameObject* SkinnedMesh = GetGameObject();
	if (SkinnedMesh != nullptr)
	{
		// 스켈레톤에 따라서 스킨드 메시의 렌더인스턴스를 업데이트 하는 과정은 게임오브젝트의 트랜스폼 커밋이 진행된 이후에 해야함
		// 그래서 여기서 트랜스폼 커밋이 필요하다고 마킹해주고 실제 렌더 인스턴스 업데이트는 UpdateRenderInstanceBonePose 에서 진행
		SkinnedMesh->MarkTransformCommitNeeded();
	}
}

void SSkinnedMeshRenderComponent::OnGameObjectTransformCommited()
{
	// noop
}

void SSkinnedMeshRenderComponent::OnChildrenGameObjectTransformCommitted()
{
	__super::OnChildrenGameObjectTransformCommitted();

	_RenderInstance->SetWorldTransformMatrix(XMMatrixIdentity());
	_RenderInstance->SetWorldRotation(Quaternion());

	UpdateRenderInstanceBonePose();
}

void SSkinnedMeshRenderComponent::PostConstructHierarchy()
{
	__super::PostConstructHierarchy();


	SGameObject* GameObject = GetGameObject();
	SGameObject* BoneAncestor = GameObject->GetStrongBindAncestor();
	ReconstructBoneBinding(BoneAncestor);
}

void SSkinnedMeshRenderComponent::ConstructRenderInstance()
{
	IRISkinnedMesh* NewSkinnedMeshRI = g_Renderer->CreateRISkinnedMesh();
	_RenderInstance = NewSkinnedMeshRI;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();

	SS_ASSERT(_ModelAssetName.IsEmpty() == false);
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);


	NewSkinnedMeshRI->SetModelAsset(FoundModelRef);
	NewSkinnedMeshRI->SetGameObjectIDXXX(GetHashCode());
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


void SSkinnedMeshRenderComponent::ReconstructBoneBinding(SGameObject* RootBoneGameObject)
{
	if (_ModelAssetName.IsEmpty())
	{
		SS_ASSERT(false);
		return;
	}

	_RootBone = RootBoneGameObject;


	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);
	IMeshAsset* BoundMesh = FoundModelRef->GetMeshAsset();

	if (BoundMesh->GetMeshType() != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}

	const MeshRawDataSkinned* SkinnedRawMesh = (MeshRawDataSkinned*)BoundMesh->GetMeshRawData();
	int32 NewBoneCnt = SkinnedRawMesh->_BoneHeader._BoneCnt;

	SS::PooledList<SGameObject*> ScrapedDecendants(200);
	ScrapedDecendants.PushBack(RootBoneGameObject);
	RootBoneGameObject->ScrapAllDescendants(ScrapedDecendants);

	_BoneBindings.Clear();
	_BoneBindings.Reserve(200);
	const SS::PooledList<BonePlacement>& OriginalBones = SkinnedRawMesh->_BonePlacements;
	for (int32 i = 0; i < NewBoneCnt; i++)
	{
		SGameObject* MatchingObject = nullptr;

		for (SGameObject* Item : ScrapedDecendants)
		{
			if (OriginalBones[i].BoneName == Item->GetObjectName())
			{
				MatchingObject = Item;
				break;
			}
		}

		_BoneBindings.PushBack(MatchingObject);
	}

	int32 a = 0;
}

void SSkinnedMeshRenderComponent::UpdateRenderInstanceBonePose()
{
	if (_RenderInstance == nullptr)
	{
		return;
	}

	IRISkinnedMesh* SkinnedRenderInstance = static_cast<IRISkinnedMesh*>(_RenderInstance);


	int32 BoneCnt = _BoneBindings.GetSize();
	for (int32 i = 0; i < BoneCnt; i++)
	{
		SGameObject* BoneObjectItem = _BoneBindings[i].Get();

		if (BoneObjectItem == nullptr)
		{
			continue;
		}

		
		SkinnedRenderInstance->UpdateSkeletonPose(i,
			BoneObjectItem->GetCommittedWorldTransformMat(),
			BoneObjectItem->GetCommittedWorldRotation().AsMatrix());
	}
}
