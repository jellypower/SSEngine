#include "pch.h"
#include "CIBox.h"

#include "SSCollision/Private/CollDetect/CollDebug_Private.h"
#include "SSCollision/Private/SpatialSystem/ISpatialAccelerationStructure.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"


ECollShapeType CIBox::GetCollShapeType() const
{
	return ECollShapeType::Box;
}


void CIBox::CollProcess_MoveObjecet(const Vector4f& MoveDelta)
{
	_WorldMat.r[3] += MoveDelta.SimdVec;
}

void CIBox::CollProcess_RotateObjecet(const Quaternion& RotDelta)
{
	SS_ASSERT(false); // TODO: Impl
}


void CIBox::SyncWorldTransform_ByContent(const XMMATRIX& WorldMat, const Quaternion& WorldRot)
{
	_WorldMat =
	{
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		_Offset.SimdVec
	};
	_WorldMat = _WorldMat * WorldMat;
	_WorldRot = WorldRot;

	XMMATRIX WorldMatAbs;
	WorldMatAbs.r[0] = XMVectorAbs(_WorldMat.r[0]);
	WorldMatAbs.r[1] = XMVectorAbs(_WorldMat.r[1]);
	WorldMatAbs.r[2] = XMVectorAbs(_WorldMat.r[2]);
	WorldMatAbs.r[3] = g_XMZero;
	XMVECTOR RotatedExtent = XMVector3TransformNormal(_Extent.SimdVec, WorldMatAbs);
	XMVECTOR WorldPos = _WorldMat.r[3];

	_BBox.Min = WorldPos - RotatedExtent;
	_BBox.Max = WorldPos + RotatedExtent;


	{
		CollDebug_Private::DrawBoundBox(_IncludedCollWorld, this, {0,0,1,1}, true, 0);


		Transform DebugTransform;
		DebugTransform.Scale = _Extent * 2;
		DebugTransform.Position = _Offset;
		XMMATRIX DebugDrawExtent = DebugTransform.AsMatrix();
		DebugDrawExtent = DebugDrawExtent * WorldMat;
		CollDebug_Private::DrawShape(
			_IncludedCollWorld, ECollDebugDraw_MeshType::Box, DebugDrawExtent, _WorldRot, 
			Vector4f::Zero, true);
	}
}

const Vector4f& CIBox::GetOffset() const
{
	return _Offset;
}

void CIBox::SetOffset(const Vector4f& InOffset)
{
	_Offset = InOffset;
	_Offset.SimdVec = XMVectorSetW(_Offset.SimdVec, 1);
}


Vector4f CIBox::GetWorldPos() const
{
	return _WorldMat.r[3];
}

const XMMATRIX& CIBox::GetWorldTransformMat() const
{
	return _WorldMat;
}

const Quaternion& CIBox::GetWorldRot() const
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

const AABBBox& CIBox::GetBBox() const
{
	return _BBox;
}

void* CIBox::GetInternalHandle() const
{
	return nullptr;
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
