#include "RIStaticMesh.h"

#include "SSContentsBase/SGameObject.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

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

ModelAsset* RIStaticMesh::GetModelAsset() const
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
