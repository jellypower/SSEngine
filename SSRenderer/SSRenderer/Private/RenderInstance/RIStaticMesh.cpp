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
	_GameObjectHashCode = InHashCode;
}

void RIStaticMesh::SetModelAsset(IModelAsset* InAsset)
{
	_ModelRef = InAsset;
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

	_ModelRef->AddAssetReference(ThisAssetRef);
}

void RIStaticMesh::OnExitFromRenderWorldXXX()
{
	_IncludedRenderWorld = nullptr;

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;

	_ModelRef->RemoveAssetReference(ThisAssetRef);
}

IRenderWorld* RIStaticMesh::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}
