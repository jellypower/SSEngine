#include "pch.h"
#include "CISphere.h"

#include "SSCollision/Private/CollDetect/CollDebug_Private.h"
#include "SSCollision/Private/SpatialSystem/ISpatialAccelerationStructure.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"

float CISphere::GetRadius() const
{
	return _Radius;
}

void CISphere::SetRadius(float InRadius)
{
	_Radius = InRadius;
}

ECollShapeType CISphere::GetCollShapeType() const
{
	return ECollShapeType::Sphere;
}


void CISphere::CollProcess_MoveObjecet(const Vector4f& MoveDelta)
{
	_WorldMat.r[3] += MoveDelta.SimdVec;
}

void CISphere::CollProcess_RotateObjecet(const Quaternion& RotDelta)
{
	SS_ASSERT(false); // TODO: Impl
}

void CISphere::SyncWorldTransform_ByContent(const XMMATRIX& WorldMat, const Quaternion& WorldRot)
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


	XMVECTOR vRadius = { _Radius, _Radius, _Radius, 0 };
	_BBox.Min = _WorldMat.r[3] - vRadius;
	_BBox.Max = _WorldMat.r[3] + vRadius;

	if (_IncludedSAS != nullptr)
	{
		_IncludedSAS->AddUpdateNeededCollInstance(this);
	}

	CollDebug_Private::DrawBoundBox(_IncludedCollWorld, this, Vector4f::Zero, true, 0);
	CollDebug_Private::DrawShape(_IncludedCollWorld, ECollDebugDraw_MeshType::Sphere, _WorldMat, _WorldRot,
		Vector4f::Zero, true);
}

void CISphere::SetOffset(const Vector4f& InOffset)
{
	_Offset = InOffset;
	_Offset.SimdVec = XMVectorSetW(_Offset.SimdVec, 1);
}


Vector4f CISphere::GetWorldPos() const
{
	return _WorldMat.r[3];
}

const XMMATRIX& CISphere::GetWorldTransformMat() const
{
	return _WorldMat;
}

const Quaternion& CISphere::GetWorldRot() const
{
	return _WorldRot;
}


Vector4f CISphere::CalcFurthest(const Vector4f& Dir) const
{
	float SqrLen = Dir.Get3DSqrLength();
	if (SqrLen < 0.0001f)
	{
		return _WorldMat.r[3]; // Dir이 불분명하면 원점 리턴
	}

	float BiggestScale = SS::CalcBiggestScaleAxis(_WorldMat);

	float Len = sqrt(SqrLen);
	Vector4f NewDir = Dir * (BiggestScale * _Radius / Len); // Radius로 벡터 길이 변경

	return NewDir.SimdVec + _WorldMat.r[3];
}

const AABBBox& CISphere::GetBBox() const
{
	return _BBox;
}


SObjHashCode CISphere::GetGameObjectID() const
{
	return _GameObjectHashCode;
}

void CISphere::SetGameObjectIDXXX(SObjHashCode InHashCode)
{
	_GameObjectHashCode = InHashCode;
}

void CISphere::OnEnterTheCollWorld(ICollisionWorld* InRenderWorld)
{
	_IncludedCollWorld = InRenderWorld;
}

void CISphere::OnExitFromCollWorld()
{
	_IncludedCollWorld = nullptr;
}

ICollisionWorld* CISphere::GetIncludedCollWorld() const
{
	return _IncludedCollWorld;
}

void CISphere::OnEnterTheSAS(ISpatialAccelerationStructure* InSAS)
{
	if (_IncludedSAS != nullptr)
	{
		SS_INTERRUPT();
		return;
	}

	_IncludedSAS = InSAS;
}

void CISphere::OnExitTheSAS()
{
	if (_IncludedSAS == nullptr)
	{
		SS_INTERRUPT();
		return;
	}

	_IncludedSAS = nullptr;
}

ISpatialAccelerationStructure* CISphere::GetIncludedSAS() const
{
	return _IncludedSAS;
}

void CISphere::SetSASProxyIdx(int64 InSASProxyIdx)
{
	_SASProxyIdx = InSASProxyIdx;
}

int64 CISphere::GetSASProxyIdx() const
{
	return _SASProxyIdx;
}
