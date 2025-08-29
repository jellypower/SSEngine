#include "RISkinnedMesh.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"

RISkinnedMesh::RISkinnedMesh():
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

	_ModelRef->AddAssetReference(ThisAssetRef);
}

void RISkinnedMesh::OnExitFromRenderWorldXXX()
{
	_IncludedRenderWorld = nullptr;

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;

	_ModelRef->RemoveAssetReference(ThisAssetRef);
}

IRenderWorld* RISkinnedMesh::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}

IModelAsset* RISkinnedMesh::GetModelAsset() const
{
	return _ModelRef;
}

void RISkinnedMesh::SetModelAsset(IModelAsset* InAsset)
{
	_ModelRef = InAsset;
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
