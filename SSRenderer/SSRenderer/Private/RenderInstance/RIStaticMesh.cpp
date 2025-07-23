#include "RIStaticMesh.h"

#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"
#include "SSRenderer/Private/RenderAsset/RenderAssetType/ModelAsset.h"

SObjHashCode RIStaticMesh::GetGameObjectID() const
{
	return _GameObjectHashCode;
}

ERenderInstanceType RIStaticMesh::GetRIType() const
{
	return ERenderInstanceType::StaticMesh;
}

const GALRIMetadata* RIStaticMesh::GetGALMetadata() const
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

IModelAsset* RIStaticMesh::GetModelAsset() const
{
	return _ModelRef;
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
	_GameObjectHashCode = InHashCode;
}

void RIStaticMesh::SetModelAsset(IModelAsset* InAsset)
{
	_ModelRef = InAsset;
}

void RIStaticMesh::SetIncludedRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	SS_ASSERT(InRenderWorld == nullptr || _IncludedRenderWorld == nullptr);
	_IncludedRenderWorld = InRenderWorld;
}

IRenderWorld* RIStaticMesh::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}
