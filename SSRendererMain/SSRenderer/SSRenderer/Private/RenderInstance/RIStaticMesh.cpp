#include "RIStaticMesh.h"

#include "SSContentsBase/SGameObject.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

int64 RIStaticMesh::GetGameObjectIDNative() const
{
	return _GameObjectHashCode.GetNativeValue();
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
	SGameObject* GameObj = (SGameObject*)_GameObjectHashCode.GetSObject();

	return GameObj->GetWorldTransformMatrix(); // TODO: Transform Commit하는 과정 만들기
}

const XMMATRIX& RIStaticMesh::GetWorldRotationMatrix() const
{
	SGameObject* GameObj = (SGameObject*)_GameObjectHashCode.GetSObject();

	return GameObj->GetWorldRot().AsMatrix(); // TODO: Transform Commit하는 과정 만들기
}
