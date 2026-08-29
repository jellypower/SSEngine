#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SSkinnedMeshRenderComponent.h"

#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRISkinnedMesh.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"


constexpr int32 TEMP_BONE_BASIC_CAPACITY = 200;

SSkinnedMeshRenderComponent::SSkinnedMeshRenderComponent():
	_ChildsByName(400, 400)
{
}

bool SSkinnedMeshRenderComponent::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SSkinnedMeshRenderComponent::PerFrame(float DeltaTime)
{
	SGameObject* SkinnedMesh = GetGameObject();
	if (SkinnedMesh != nullptr)
	{
		// 스켈레톤에 따라서 스킨드 메시의 렌더인스턴스를 업데이트 하는 과정은 게임오브젝트의 트랜스폼 커밋이 진행된 이후에 해야함
		// 그래서 여기서 트랜스폼 커밋이 필요하다고 마킹해주고 실제 렌더 인스턴스 업데이트는 UpdateRenderInstanceBonePose 에서 진행
		SkinnedMesh->MarkTransformCommitNeeded();
	}
}

void SSkinnedMeshRenderComponent::OnGameObjectTransformCommited(EFramePhase CommitPhase)
{
	// SRenderComponentBase::OnGameObjectTransformCommited 에서
	// 적용한 월드 트랜스폼이 작동 안하도록 하기
}

void SSkinnedMeshRenderComponent::OnChildrenGameObjectTransformCommitted(EFramePhase CommitPhase)
{
	// 기존 트랜스폼 무력화하기
	// TODO: 기존 트랜스폼 무력화하는 코드 필요 없어보임 -> 어차피 Bone의 WorldTransform만 쓰는듯
	_RenderInstance->SetWorldTransformMatrix(XMMatrixIdentity());
	_RenderInstance->SetWorldRotation(Quaternion());

	UpdateRenderInstanceBonePose(); // 대신 전부 
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
	_RenderInstance = g_Renderer->CreateRISkinnedMesh();
	_RenderInstance->SetGameObjectIDXXX(GetHashCode());
	SnycMeshRIWithAssetBindingIfExists();
}

void SSkinnedMeshRenderComponent::DestructRenderInstance()
{
	if (_RenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	if (GetIncludedWorld() != nullptr)
	{
		SS_INTERRUPT(); // Must be removed from world before destruct.
		return;
	}

	_RenderInstance->ReleaseGALMetaData();
	_RenderInstance->Release();
}


void SSkinnedMeshRenderComponent::ReconstructBoneBinding(SGameObject* RootBoneGameObject)
{
	SCOPE_PROFILE(ReconstructBoneBinding);
	if (_CachedMeshAsset == nullptr || _CachedMeshAsset->GetMeshType() != EMeshType::Skinned)
	{
		SS_ASSERT(false);
		return;
	}

	_RootBone = RootBoneGameObject;


	const MeshRawDataSkinned* SkinnedRawMesh = (MeshRawDataSkinned*)_CachedMeshAsset->GetMeshRawData();
	int32 NewBoneCnt = SkinnedRawMesh->_BoneHeader._BoneCnt;

	SS::PooledList<SGameObject*> ScrapedDecendants(400);
	ScrapedDecendants.PushBack(RootBoneGameObject);
	RootBoneGameObject->ScrapAllDescendants(ScrapedDecendants);

	for (SGameObject* ChildItem : ScrapedDecendants)
	{
		_ChildsByName.Add(ChildItem->GetObjectName(), ChildItem);
	}


	_BoneBindings.Clear();
	_BoneBindings.Reserve(200);

	const SS::PooledList<SS::SHasherW>& BoneNames = SkinnedRawMesh->_BoneNames;
	for (int32 i = 0; i < NewBoneCnt; i++)
	{

		SObjHashT<SGameObject>* FoundChild = _ChildsByName.Find(BoneNames[i]);
		if (FoundChild == nullptr)
		{
			SS_ASSERT(false);
			continue;
		}

		_BoneBindings.PushBack(*FoundChild);
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
