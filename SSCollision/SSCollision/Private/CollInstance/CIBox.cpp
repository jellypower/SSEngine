#include "pch.h"
#include "CIBox.h"

#include "SSCollision/Private/CollDetect/CollDebug_Private.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"


ECollShapeType CIBox::GetCollShapeType() const
{
	return ECollShapeType::Box;
}


void CIBox::SetWorldTransform(const XMMATRIX& WorldMat, const Quaternion& WorldRot)
{
	_WorldMat = WorldMat;
	_WorldRot = WorldRot;

	CommitTransform();
	_IncludedCollWorld->AddTransformCommitNeededObj(this);
}

void CIBox::CommitTransform()
{
	XMMATRIX WorldMatAbs;
	WorldMatAbs.r[0] = XMVectorAbs(_WorldMat.r[0]);
	WorldMatAbs.r[1] = XMVectorAbs(_WorldMat.r[1]);
	WorldMatAbs.r[2] = XMVectorAbs(_WorldMat.r[2]);
	WorldMatAbs.r[3] = g_XMZero;
	XMVECTOR RotatedExtent = XMVector3TransformNormal(_Extent.SimdVec, WorldMatAbs);
	XMVECTOR WorldPos = _WorldMat.r[3];

	_BBMin = WorldPos - RotatedExtent;
	_BBMax = WorldPos + RotatedExtent;


	// DEBUG
	{
		CDDD_Line Desc;
		Desc.Start = _WorldMat.r[3];
		Desc.End = _WorldMat.r[3] + RotatedExtent;
		CollDebug_Private::DrawLine(_IncludedCollWorld, Desc);

		CollDebug_Private::DrawBoundBox(_IncludedCollWorld, this, Vector4f::Zero, true, 0);
	}
}

Vector4f CIBox::GetWorldPos() const
{
	return _WorldMat.r[3];
}

const XMMATRIX& CIBox::GetWorldTransformMat() const
{
	return _WorldMat;
}

const Quaternion& CIBox::GetWorldRotTransformMat() const
{
	return _WorldRot;
}

Vector4f CIBox::CalcFurthest(const Vector4f& Dir) const
{
	XMMATRIX WorldToLocal = InverseRigid(_WorldMat);
	Vector4f LocalDir = XMVector3TransformNormal(Dir.SimdVec, WorldToLocal);

	Vector4f FurthestLocal;
	FurthestLocal.X = LocalDir.X > 0 ? _Extent.X : -_Extent.X;
	FurthestLocal.Y = LocalDir.Y > 0 ? _Extent.Y : -_Extent.Y;
	FurthestLocal.Z = LocalDir.Z > 0 ? _Extent.Z : -_Extent.Z;
	FurthestLocal.W = 1;

	Vector4f Point = XMVector3Transform(FurthestLocal.SimdVec, _WorldMat);
	CollDebug_Private::DrawPoint(_IncludedCollWorld, Point, Vector4f::Zero, true);

	return Point;
}

Vector4f CIBox::GetBBMin() const
{
	return _BBMin;
}

Vector4f CIBox::GetBBMax() const
{
	return _BBMax;
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