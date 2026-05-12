#include "pch.h"
#include "CIBox.h"

#include "SSCollision/Private/CollDetect/CollDebug_Private.h"
#include "SSCollision/Private/SpatialSystem/ISpatialAccelerationStructure.h"
#include "SSCollision/Public/CollInstance/CICreationDesc.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"


CIBox::CIBox(const CI_BOX_DESC& Desc, physx::PxShape* InShape)
{
	_ColliderLclTransform = Desc.InitialLclTransform;
	_Offset = Desc.Offset;
	_GameObjectHashCode = Desc.ComponentID;
	_Extent = Desc.Extent;

	_Shape = InShape;
	_Shape->userData = this;
}

CIBox::~CIBox()
{
	PX_RELEASE(_Shape);
}

ECollShapeType CIBox::GetCollShapeType() const
{
	return ECollShapeType::Box;
}

void CIBox::SyncColliderLclTransform_ByContent(const Transform& LocalTransform)
{
	_ColliderLclTransform = LocalTransform;
	ApplyLocalTransformChange();
}

const Vector4f& CIBox::GetExtent() const
{
	return _Extent;
}

void CIBox::SetExtent(const Vector4f& InExtent)
{
	_Extent = InExtent;
	ApplyLocalTransformChange();
}


const Vector4f& CIBox::GetOffset() const
{
	return _Offset;
}

void CIBox::SetOffset(const Vector4f& InOffset)
{
	_Offset = InOffset;
	_Offset.SimdVec = XMVectorSetW(_Offset.SimdVec, 1);
	ApplyLocalTransformChange();
}

Vector4f CIBox::CalcFurthest(const Vector4f& Dir) const
{
	
//	XMMATRIX WorldToLocal = _WorldTransform.AsInverseMatrix();
	XMMATRIX WorldToLocal = XMMatrixIdentity();
	Vector4f LocalDir = XMVector3TransformNormal(Dir.SimdVec, WorldToLocal);

	Vector4f FurthestLocal;
	FurthestLocal.X = LocalDir.X > 0 ? _Extent.X : -_Extent.X;
	FurthestLocal.Y = LocalDir.Y > 0 ? _Extent.Y : -_Extent.Y;
	FurthestLocal.Z = LocalDir.Z > 0 ? _Extent.Z : -_Extent.Z;
	FurthestLocal.W = 1;

	Vector4f Point = XMVector3Transform(FurthestLocal.SimdVec, WorldToLocal);
	CollDebug_Private::DrawPoint(_IncludedCollWorld, Point, Vector4f::Zero, true);

	return Point;
}

const AABBBox& CIBox::GetBBox() const
{
	return AABBBox();
}


ICollisionWorld* CIBox::GetIncludedCollWorld() const
{
	if (_OwnerRigidBody == nullptr)
	{
		return nullptr;
	}

	return _OwnerRigidBody->GetIncludedCollWorld();
}

Transform CIBox::CalcBoxTransform() const
{
	Transform NewTransform = { _Offset, Quaternion(), _Extent };
	NewTransform = NewTransform * _ColliderLclTransform;
	return NewTransform;
}

void CIBox::ApplyLocalTransformChange()
{

	Transform NewTransform = CalcBoxTransform();

	if (_Shape != nullptr)
	{
		physx::PxTransform PxPose = _Shape->getLocalPose();
		XMVECTOR PrevPos = {PxPose.p.x, PxPose.p.x, PxPose.p.x, 1};
		XMVECTOR PrevRot = { PxPose.q.x , PxPose.q.y , PxPose.q.z , PxPose.q.w };

		if (XMAlmostEqual(PrevPos, NewTransform.Position.SimdVec) == false ||
			XMAlmostEqual(PrevRot, NewTransform.Rotation.SimdVec) == false)
		{
			PxPose.p = {
				NewTransform.Position.X ,
				NewTransform.Position.Y,
				NewTransform.Position.Z };
			PxPose.q = {
				NewTransform.Rotation.X,
				NewTransform.Rotation.Y,
				NewTransform.Rotation.Z,
				NewTransform.Rotation.W };
			_Shape->setLocalPose(PxPose);
		}

		physx::PxGeometryHolder Geom = _Shape->getGeometry();
		if (Geom.getType() != physx::PxGeometryType::eBOX)
		{
			SS_INTERRUPT();
			return;
		}


		physx::PxBoxGeometry BoxGeom = Geom.box();
		XMVECTOR PrevExtent =
		{ BoxGeom.halfExtents.x ,
			BoxGeom.halfExtents.y ,
			BoxGeom.halfExtents.z,
			0};

		if (XMAlmostEqual(PrevExtent, NewTransform.Scale.SimdVec) == false)
		{
			BoxGeom.halfExtents = { NewTransform.Scale.X, NewTransform.Scale.Y, NewTransform.Scale.X };
			_Shape->setGeometry(BoxGeom);
		}
	}

	if (_OwnerRigidBody != nullptr)
	{
		Transform ParentTransform =
		{
			_OwnerRigidBody->GetSimulBeginPos(),
			_OwnerRigidBody->GetSimulBeginRot(),
			{1,1,1,0}
		};

		Transform DebugTransform = NewTransform;
		DebugTransform.Scale = NewTransform.Scale * 2;
		DebugTransform = DebugTransform * ParentTransform;
		CollDebug_Private::DrawShape(
			_IncludedCollWorld, ECollDebugDraw_MeshType::Box, DebugTransform.AsMatrix(), DebugTransform.Rotation,
			{ 0,1,0,1 }, true);
	}
}

SObjHashCode CIBox::GetGameObjectID() const
{
	return _GameObjectHashCode;
}