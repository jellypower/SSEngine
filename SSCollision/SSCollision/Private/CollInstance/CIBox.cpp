#include "pch.h"
#include "CIBox.h"

#include "SSCollision/Private/CollDetect/CollUtils_Private.h"

ECollShapeType CIBox::GetCollShapeType() const
{
	return ECollShapeType::Box;
}


void CIBox::SetWorldTransform(const XMMATRIX& WorldMat, const Quaternion& WorldRot)
{
	_WorldMat = WorldMat;
	_WorldRot = WorldRot;

	CommitTransform();
}

void CIBox::CommitTransform()
{
	_Vertices[0] = { -_Extent.X,-_Extent.Y,-_Extent.Z, 1 };
	_Vertices[1] = { -_Extent.X,-_Extent.Y,+_Extent.Z, 1 };
	_Vertices[2] = { +_Extent.X,-_Extent.Y,+_Extent.Z, 1 };
	_Vertices[3] = { +_Extent.X,-_Extent.Y,-_Extent.Z, 1 };
	_Vertices[4] = { -_Extent.X,+_Extent.Y,-_Extent.Z, 1 };
	_Vertices[5] = { -_Extent.X,+_Extent.Y,+_Extent.Z, 1 };
	_Vertices[6] = { +_Extent.X,+_Extent.Y,+_Extent.Z, 1 };
	_Vertices[7] = { +_Extent.X,+_Extent.Y,-_Extent.Z, 1 };

	for (int i = 0; i < 8; i++)
	{
		_Vertices[i] = XMVector4Transform(_Vertices[i].SimdVec, _WorldMat);
	}
}

Vector4f CIBox::CalcFurthest(const Vector4f& Dir) const
{
	return 	CollUtils::CalcFurthest(Dir, _Vertices, 8);
}

void CIBox::OnEnterTheCollWorld(ICollisionWorld* InCollWorld)
{
	_IncludedCollWorld = InCollWorld;
}

void CIBox::OnExitFromCollWorld()
{
	_IncludedCollWorld = nullptr;
}

ICollisionWorld* CIBox::GetIncludedCollWorld() const
{
	return _IncludedCollWorld;
}

const Vector4f& CIBox::GetExtent() const
{
	return _Extent;
}

Vector4f CIBox::GetVertexPos(int Idx) const
{
	if (Idx < 0 || Idx >= 8)
	{
		SS_ASSERT(false);
		return _WorldMat.r[3];
	}

	return _Vertices[Idx];
}

void CIBox::SetExtent(const Vector4f& InExtent)
{
	_Extent = InExtent;
}

SObjHashCode CIBox::GetGameObjectID() const
{
	return _GameObjectHashCode;
}

void CIBox::SetGameObjectIDXXX(SObjHashCode InHashCode)
{
	_GameObjectHashCode = InHashCode;
}