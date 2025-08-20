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
	SGameObject* Object = GetGameObject();
	Object->MarkTransformCommitNeeded();
}

void SSkinnedMeshRenderComponent::OnChildrenGameObjectTransformCommitted()
{
	__super::OnChildrenGameObjectTransformCommitted();
	UpdateRenderInstanceBonePose();
}

void SSkinnedMeshRenderComponent::PostConstructHierarchy()
{
	__super::PostConstructHierarchy();

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);
	IMeshAsset* BoundMesh = FoundModelRef->GetMeshAsset();
	if (BoundMesh->GetMeshType() != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}

	const MeshRawDataSkinned* SkinnedRawMesh = (MeshRawDataSkinned*)BoundMesh->GetMeshRawData();
	SS::SHasherW RootBoneName = SkinnedRawMesh->_BoneOriginalPose[SkinnedRawMesh->_RootBoneIdx].BoneName;


	SGameObject* OutmostGameObject = GetGameObject();
	while (OutmostGameObject->GetParent() != nullptr)
	{
		OutmostGameObject = OutmostGameObject->GetParent();
	}

	SGameObject* SkeletonRootGameObject = OutmostGameObject->FindChildOfName(RootBoneName, true);
	ReconstructBoneBinding(SkeletonRootGameObject);
}

void SSkinnedMeshRenderComponent::ConstructRenderInstance()
{
	IRISkinnedMesh* NewSkinnedMeshRI = g_Renderer->CreateRISkinnedMesh();
	_RenderInstance = NewSkinnedMeshRI;

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();

	SS_ASSERT(_ModelAssetName.IsEmpty() == false);
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);


	SGameObject* Parent = GetGameObject();
	NewSkinnedMeshRI->SetModelAsset(FoundModelRef);
	NewSkinnedMeshRI->SetGameObjectIDXXX(Parent->GetHashCode());


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


	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	IModelAsset* FoundModelRef = AssetManager->FindAssetByName<IModelAsset>(_ModelAssetName);
	IMeshAsset* BoundMesh = FoundModelRef->GetMeshAsset();

	if (BoundMesh->GetMeshType() != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}

	const MeshRawDataSkinned* SkinnedRawMesh = (MeshRawDataSkinned*)BoundMesh->GetMeshRawData();
	int32 NewBoneCnt = SkinnedRawMesh->_BoneOriginalPose.GetSize();

	SS::PooledList<SGameObject*> ScrapedDecendants(200);
	ScrapedDecendants.PushBack(RootBoneGameObject);
	RootBoneGameObject->ScrapAllDescendants(ScrapedDecendants);


	_BoneBindings.Reserve(200);
	const SS::PooledList<BonePlacement>& OriginalBones = SkinnedRawMesh->_BoneOriginalPose;
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

	SGameObject* ThisGameObject = GetGameObject();
	XMMATRIX ThisWorldMatrixInv = ThisGameObject->GetCommittedWorldTransformMat();
	ThisWorldMatrixInv = InverseRigid(ThisWorldMatrixInv);
	Quaternion ThisRotInverse = ThisGameObject->GetCommittedWorldRotation().Inverse();

	int32 BoneCnt = _BoneBindings.GetSize();
	for (int32 i = 0; i < BoneCnt; i++)
	{
		SGameObject* BoneObjectItem = _BoneBindings[i].Get();

		if (BoneObjectItem == nullptr)
		{
			continue;
		}

		XMMATRIX BoneRelativeWMat = BoneObjectItem->GetCommittedWorldTransformMat() * ThisWorldMatrixInv;

		Quaternion Rotation = ThisRotInverse * BoneObjectItem->GetCommittedWorldRotation();
		XMMATRIX BoneRelativeRotMat = Rotation.AsMatrix();
//		XMMATRIX BoneRelativeWMat = BoneObjectItem->GetCommittedWorldTransformMat();
//		XMMATRIX BoneRelativeRotMat = BoneObjectItem->GetCommittedWorldRotation().AsMatrix();

		
		SkinnedRenderInstance->UpdateSkeletonPose(i,
			BoneRelativeWMat,
			BoneRelativeRotMat);
		
//		SkinnedRenderInstance->UpdateSkeletonPose(i,
//			XMMatrixIdentity(),
//			XMMatrixIdentity());
	}
}
