#include "RIStaticMesh.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"

SObjHashCode RIStaticMesh::GetGameObjectID() const
{
	return _GameObjectHashCode;
}

ERenderInstanceType RIStaticMesh::GetRIType() const
{
	return ERenderInstanceType::StaticMesh;
}

GALRIMetadata* RIStaticMesh::GetGALMetadata() const
{
	return _MetaData;
}

void RIStaticMesh::ReleaseGALMetaData()
{
	if (_MetaData != nullptr)
	{
		delete _MetaData;
		_MetaData = nullptr;
	}
}

const XMMATRIX& RIStaticMesh::GetWorldTransformMatrix() const
{
	return _WorldTransformMatrix;
}

const XMMATRIX& RIStaticMesh::GetWorldRotationMatrix() const
{
	return _WorldRotationMatrix;
}

void RIStaticMesh::InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover)
{
	if (_MetaData != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	if (MetadataToHandover->GetMetadataRenderInstanceType() != ERenderInstanceType::StaticMesh)
	{
		SS_INTERRUPT();
		return;
	}

	_MetaData = MetadataToHandover;
}

void RIStaticMesh::SetWorldTransformMatrix(const XMMATRIX& InMatrix)
{
	_WorldTransformMatrix = InMatrix;
}

void RIStaticMesh::SetWorldRotation(const Quaternion& InRotation)
{
	_WorldRotationMatrix = InRotation.AsMatrix();
}

void RIStaticMesh::SetGameObjectIDXXX(SObjHashCode InHashCode)
{
	if (_GameObjectHashCode != nullptr)
	{
		SS_INTERRUPT();
	}

	_GameObjectHashCode = InHashCode;
}

IMeshAsset* RIStaticMesh::GetMeshAsset() const
{
	return _MeshRef;
}

IMaterialAsset* RIStaticMesh::GetMaterialAsset(int MtlIdx) const
{
	if (_MtlRef.GetSize() <= MtlIdx)
	{
		return nullptr;
	}

	return _MtlRef[MtlIdx];
}


void RIStaticMesh::SetMeshAsset(IMeshAsset* InAsset)
{
	if (_IncludedRenderWorld == nullptr)
	{
		_MeshRef = InAsset;
		const int32 NewSubMeshCnt = _MeshRef->GetSubMeshCnt();
		_MtlRef.SetSizeDirectly(NewSubMeshCnt);
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

void RIStaticMesh::SetMaterialAsset(IMaterialAsset* InAsset, int32 MtlIdx)
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

void RIStaticMesh::OnEnterTheRenderWorldXXX(IRenderWorld* InRenderWorld)
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

void RIStaticMesh::OnExitFromRenderWorldXXX()
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

IRenderWorld* RIStaticMesh::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}