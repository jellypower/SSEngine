#include "RISkinnedMesh.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"

RISkinnedMesh::RISkinnedMesh() :
	_SkeletonPose(200)
{
}

SObjHashCode RISkinnedMesh::GetGameObjectID() const
{
	return _GameObjectHashCode;
}

void RISkinnedMesh::SetGameObjectIDXXX(SObjHashCode InHashCode)
{
	_GameObjectHashCode = InHashCode;
}

ERenderInstanceType RISkinnedMesh::GetRIType() const
{
	return ERenderInstanceType::SkinnedMesh;
}

const XMMATRIX& RISkinnedMesh::GetWorldTransformMatrix() const
{
	return _WorldTransformMatrix;
}

const XMMATRIX& RISkinnedMesh::GetWorldRotationMatrix() const
{
	return _WorldRotationMatrix;
}

void RISkinnedMesh::SetWorldTransformMatrix(const XMMATRIX& InMatrix)
{
	_WorldTransformMatrix = InMatrix;
}

void RISkinnedMesh::SetWorldRotation(const Quaternion& InRotation)
{
	_WorldRotationMatrix = InRotation.AsMatrix();
}

void RISkinnedMesh::InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover)
{
	if (_MetaData != nullptr)
	{
		SS_INTERRUPT();
		return;
	}

	if (MetadataToHandover->GetMetadataRenderInstanceType() != ERenderInstanceType::SkinnedMesh)
	{
		SS_INTERRUPT();
		return;
	}

	_MetaData = MetadataToHandover;
}

GALRIMetadata* RISkinnedMesh::GetGALMetadata() const
{
	return _MetaData;
}

void RISkinnedMesh::ReleaseGALMetaData()
{
	if (_MetaData != nullptr)
	{
		delete _MetaData;
		_MetaData = nullptr;
	}
}

void RISkinnedMesh::OnEnterTheRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	if (InRenderWorld == nullptr || _IncludedRenderWorld != nullptr)
	{
		SS_INTERRUPT();
	}

	_IncludedRenderWorld = InRenderWorld;

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;


	if (_MeshRef == nullptr)
	{
		SS_INTERRUPT();
		return;
	}
	_MeshRef->AddAssetReference(ThisAssetRef);

	int32 SubMeshCnt = _MeshRef->GetSubMeshCnt();
	for (int i = 0; i < SubMeshCnt; i++)
	{
		_MtlRef[i]->AddAssetReference(ThisAssetRef);
	}
}

void RISkinnedMesh::OnExitFromRenderWorldXXX()
{
	_IncludedRenderWorld = nullptr;

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;

	if (_MeshRef == nullptr)
	{
		SS_INTERRUPT();
		return;
	}
	_MeshRef->RemoveAssetReference(ThisAssetRef);

	int32 SubMeshCnt = _MeshRef->GetSubMeshCnt();
	for (int i = 0; i < SubMeshCnt; i++)
	{
		_MtlRef[i]->RemoveAssetReference(ThisAssetRef);
	}
}

IRenderWorld* RISkinnedMesh::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}

IMeshAsset* RISkinnedMesh::GetMeshAsset() const
{
	return _MeshRef;
}

IMaterialAsset* RISkinnedMesh::GetMaterialAsset(int MtlIdx) const
{
	if (_MeshRef->GetSubMeshCnt() <= MtlIdx)
	{
		return nullptr;
	}

	return _MtlRef[MtlIdx];
}


void RISkinnedMesh::SetModelAsset(IModelAsset* InAsset)
{
	if (_IncludedRenderWorld == nullptr)
	{
		_MeshRef = InAsset->GetMeshAsset();

		int32 SubMeshCnt = _MeshRef->GetSubMeshCnt();
		_MtlRef.SetSizeDirectly(SubMeshCnt);
		for (int32 i = 0; i < SubMeshCnt; i++)
		{
			_MtlRef[i] = InAsset->GetMaterialAsset(i);
		}

		return;
	}

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;



	const int32 PrevSubMeshCnt = _MeshRef->GetSubMeshCnt();
	_MeshRef->RemoveAssetReference(ThisAssetRef);
	for (int32 i = 0; i < PrevSubMeshCnt; i++)
	{
		_MtlRef[i]->RemoveAssetReference(ThisAssetRef);
	}



	_MeshRef = InAsset->GetMeshAsset();
	const int32 NewSubMeshCnt = _MeshRef->GetSubMeshCnt();
	_MeshRef->AddAssetReference(ThisAssetRef);
	_MtlRef.SetSizeDirectly(NewSubMeshCnt);
	for (int32 i=0;i<NewSubMeshCnt;i++)
	{
		_MtlRef[i] = InAsset->GetMaterialAsset(i);
		_MtlRef[i]->AddAssetReference(ThisAssetRef);
	}
}

void RISkinnedMesh::SetMeshAsset(IMeshAsset* InAsset)
{
	if (_IncludedRenderWorld == nullptr)
	{
		_MeshRef = InAsset;
		return;
	}

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;

	const int32 PrevSubMeshCnt = _MeshRef->GetSubMeshCnt();
	_MeshRef->RemoveAssetReference(ThisAssetRef);
	_MeshRef = InAsset;
	const int32 NewSubMeshCnt = _MeshRef->GetSubMeshCnt();
	_MeshRef->AddAssetReference(ThisAssetRef);


	for (int32 i = NewSubMeshCnt; i < PrevSubMeshCnt; i++)
	{
		_MtlRef[i]->RemoveAssetReference(ThisAssetRef);
	}
	_MtlRef.SetSizeDirectly(NewSubMeshCnt);
}

void RISkinnedMesh::SetMaterialAsset(IMaterialAsset* InAsset, int32 MtlIdx)
{
	if (_MeshRef == nullptr)
	{
		return;
	}

	const int32 SubMeshCnt = _MeshRef->GetSubMeshCnt();
	if (SubMeshCnt <= MtlIdx)
	{
		return;
	}

	if (_IncludedRenderWorld == nullptr)
	{
		_MtlRef[MtlIdx] = InAsset;
		return;
	}

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;

	_MtlRef[MtlIdx]->RemoveAssetReference(ThisAssetRef);
	_MtlRef[MtlIdx] = InAsset;
	InAsset->AddAssetReference(ThisAssetRef);
}

const SS::PooledList<SBASkinningJointMatrix>& RISkinnedMesh::GetSkeletonPose() const
{
	return _SkeletonPose;
}

void RISkinnedMesh::UpdateSkeletonPose(int32 BoneIdx, const XMMATRIX& WMatrix, const XMMATRIX& RotMatrix)
{
	if (BoneIdx >= _SkeletonPose.GetSize())
	{
		_SkeletonPose.Resize(BoneIdx + 1);
	}

	_SkeletonPose[BoneIdx].WMatrix = WMatrix;
	_SkeletonPose[BoneIdx].RotMatrix = RotMatrix;
}
