#include "pch.h"
#include "CISphere.h"

#include "SSCollision/Private/CollDetect/CollDebug_Private.h"
#include "SSCollision/Private/SpatialSystem/ISpatialAccelerationStructure.h"
#include "SSCollision/Public/CollInstance/CICreationDesc.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"

float CISphere::GetRadius() const
{
	return _Radius;
}

void CISphere::SetRadius(float InRadius)
{
	_Radius = InRadius;
	ApplyLocalTransformChange();
}

Transform CISphere::CalcSphereTransform() const
{
	// Offset: Component's internal Variable
	// ColliderTransform: GameObject's Transform
	
	Transform NewTransform = { _Offset, Quaternion(), {1, 1, 1, 0} };
	NewTransform = NewTransform * _ColliderTransform;
	return NewTransform;
}

void CISphere::ApplyLocalTransformChange()
{
	Transform NewTransform = CalcSphereTransform();

	if (_Shape != nullptr)
	{
		physx::PxTransform PxPose = _Shape->getLocalPose();

		Vector4f PrevPos = PxTransformConvert::Vec3FromPx(PxPose.p);
		Quaternion PrevRot = PxTransformConvert::QuatFromPx(PxPose.q);

		if (XMAlmostEqual(PrevPos.SimdVec, NewTransform.Position.SimdVec) == false ||
			XMAlmostEqual(PrevRot.SimdVec, NewTransform.Rotation.SimdVec) == false)
		{
			PxPose.p = PxTransformConvert::Vec3ToPx(NewTransform.Position);
			PxPose.q = PxTransformConvert::QuatToPx(NewTransform.Rotation);
			_Shape->setLocalPose(PxPose);
		}

		physx::PxGeometryHolder Geom = _Shape->getGeometry();
		if (Geom.getType() != physx::PxGeometryType::eSPHERE)
		{
			SS_INTERRUPT();
			return;
		}

		NewTransform.Scale = XMVectorSetW(NewTransform.Scale.SimdVec, 0);
		float NewRadius = SS::GetBiggest(NewTransform.Scale.SimdVec);
		NewRadius *= _Radius;

		physx::PxSphereGeometry SphereGeom = Geom.sphere();
		float Diff = SphereGeom.radius - NewRadius;
		Diff = Diff < 0 ? -Diff : Diff;
		if (Diff > 0.001f)
		{
			SphereGeom.radius = NewRadius;
			_Shape->setGeometry(SphereGeom);
		}
	}

	if (_OwnerRigidBody != nullptr)
	{
		Transform ParentTransform =
		{
			_OwnerRigidBody->GetSimulBeginPos(),
			_OwnerRigidBody->GetSimulBeginRot(),
			{1, 0, 0, 0}
		};


		Transform DebugTransform = NewTransform;
		DebugTransform.Scale = NewTransform.Scale * 2;
		DebugTransform = DebugTransform * ParentTransform;
		CollDebug_Private::DrawShape(
			GetIncludedCollWorld(), ECollDebugDraw_MeshType::Box, DebugTransform.AsMatrix(), DebugTransform.Rotation,
			{ 0,1,0,1 }, true);
	}
}

CISphere::CISphere(const CI_SPHERE_DESC& Desc, physx::PxShape* InShape)
{
	_ColliderTransform = Desc.InitialLclTransform;
	_Offset = Desc.Offset;
	_GameObjectHashCode = Desc.ComponentID;
	_Radius = Desc.Radius;

	_Shape->userData = this;
	_Shape = InShape;
}

CISphere::~CISphere()
{
	PX_RELEASE(_Shape);
}

ECollShapeType CISphere::GetCollShapeType() const
{
	return ECollShapeType::Sphere;
}

void CISphere::SyncColliderLclTransform_ByContent(const Transform& LocalTransform)
{
	_ColliderTransform = LocalTransform;
	ApplyLocalTransformChange();
}

const Vector4f& CISphere::GetOffset() const
{
	return _Offset;
}

void CISphere::SetOffset(const Vector4f& InOffset)
{
	_Offset = InOffset;
	_Offset.SimdVec = XMVectorSetW(_Offset.SimdVec, 1);
	ApplyLocalTransformChange();
}


Vector4f CISphere::CalcFurthest(const Vector4f& Dir) const
{
	return Vector4f();
}

const AABBBox& CISphere::GetBBox() const
{
	return AABBBox();
}


SObjHashCode CISphere::GetGameObjectID() const
{
	return _GameObjectHashCode;
}

ICollisionWorld* CISphere::GetIncludedCollWorld() const
{
	if (_OwnerRigidBody == nullptr)
	{
		return nullptr;
	}

	return _OwnerRigidBody->GetIncludedCollWorld();
}