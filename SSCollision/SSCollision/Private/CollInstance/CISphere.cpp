#include "pch.h"
#include "CISphere.h"

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

void CISphere::SetWorldTransform(const XMMATRIX& WorldMat, const Quaternion& WorldRot)
{
	_WorldMat = WorldMat;
	_WorldRot = WorldRot;
}

void CISphere::CommitTransform()
{
	// noop
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
