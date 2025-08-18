#include "RISkinnedMesh.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

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

void RISkinnedMesh::SetIncludedRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	SS_ASSERT(InRenderWorld == nullptr || _IncludedRenderWorld == nullptr);
	_IncludedRenderWorld = InRenderWorld;
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

void RISkinnedMesh::UpdateSkeleton()
{
	SS_INTERRUPT(); // TODO: Implementation
}
