#include "pch.h"
#include "CollDebug_Private.h"

#include "SSCollision/Private/RigidBody/RIUtils_Private.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/DEBUG/CollDebugDrawDescs.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"


void CollDebug_Private::DrawLine(ICollisionWorld* InWorldToDraw, const CDDD_Line& Desc)
{
	InWorldToDraw->AddDrawDebugLine(Desc);
}

void CollDebug_Private::DrawSimplex(
	ICollisionWorld* InWorldToDraw,
	const SimplexV4& InSimplex,
	const Vector4f& Pos, 
	const Vector4f& Color,
	bool bUseDepth, 
	float Time)
{
	int Cnt = InSimplex.GetCnt();


	CDDD_Line LineDrawDesc;
	LineDrawDesc.Color = Color;
	LineDrawDesc.bUseDepth = bUseDepth;
	LineDrawDesc.Time = Time;

	for (int i = Cnt - 1; i >= 0; i--)
	{
		LineDrawDesc.Start = InSimplex[i] + Pos;
		for (int j = i - 1; j >= 0; j--)
		{
			LineDrawDesc.End = InSimplex[j] + Pos;
			InWorldToDraw->AddDrawDebugLine(LineDrawDesc);
		}
	}
}

void CollDebug_Private::DrawShape(
	ICollisionWorld* InWorldToDraw, 
	ECollDebugDraw_MeshType MeshType,
	const XMMATRIX& TransformMat, 
	const Quaternion& Rot, 
	const Vector4f& Color, 
	bool bUseDepth, 
	float Time)
{
	CDDD_Mesh Desc;
	Desc.WMatrix = TransformMat;
	Desc.RotMatrix = Rot.AsMatrix();
	Desc.Color = Color;
	Desc.bUseDepth = bUseDepth;
	Desc.Time = Time;
	Desc.Type = MeshType;
	InWorldToDraw->AddDrawDebugMesh(Desc);
}



void CollDebug_Private::DrawBoundBox(
	ICollisionWorld* InWorldToDraw, 
	const ICollInstanceBase* CollInstance,
	const Vector4f& Color, 
	bool bUseDepth, 
	float Time)
{
//	DrawBoundBox(
//		InWorldToDraw,
//		CollInstance->get(),
//		CollInstance->GetBBox(),
//		Color,
//		bUseDepth,
//		Time);
}

void CollDebug_Private::DrawBoundBox(
	ICollisionWorld* InWorldToDraw, 
	const Vector4f& InPos, 
	const AABBBox& InBox,
	const Vector4f& Color, 
	bool bUseDepth, 
	float Time)
{
	Transform DrawTransform;
	DrawTransform.Position = InPos;
	DrawTransform.Scale = InBox.Max - InBox.Min;


	CDDD_Mesh Desc;
	Desc.WMatrix = DrawTransform.AsMatrix();
	Desc.RotMatrix = XMMatrixIdentity();
	Desc.Color = Color;
	Desc.bUseDepth = bUseDepth;
	Desc.Time = Time;
	Desc.Type = ECollDebugDraw_MeshType::Box;
	InWorldToDraw->AddDrawDebugMesh(Desc);
}

void CollDebug_Private::DrawPoint(
	ICollisionWorld* InWorldToDraw, 
	const Vector4f& Pos, 
	const Vector4f& Color,
	bool bUseDepth, 
	float Scale, 
	float Time)
{
	Transform DrawTransform;
	DrawTransform.Position = Pos;
	DrawTransform.Scale = { Scale, Scale,Scale ,0 };



	CDDD_Mesh Desc;
	Desc.WMatrix = DrawTransform.AsMatrix();
	Desc.RotMatrix = XMMatrixIdentity();
	Desc.Color = Color;
	Desc.bUseDepth = bUseDepth;
	Desc.Time = Time;
	InWorldToDraw->AddDrawDebugMesh(Desc);
}

void CollDebug_Private::DrawPXRigid(IRigidBodyBase* RIToDraw, const Vector4f& Color, bool bUseDepth, float Time)
{
	if (RIToDraw->IsTransformModifiedOnThisTick() == false)
	{
		return;
	}

	physx::PxActor* Actor = ExtractPxActor(RIToDraw);
	physx::PxRigidActor* RigidActor = Actor->is<physx::PxRigidActor>();
	if (RigidActor == nullptr)
	{
		return;
	}

	Transform GlobalTransform = PxTransformConvert::TransformFromPx(RigidActor->getGlobalPose());

	physx::PxShape* Shapes[10] = { nullptr, };
	int32 Cnt = RigidActor->getShapes(Shapes, 10);

	for (int32 i=0;i<Cnt;i++)
	{
		physx::PxShape* ShapeItem = Shapes[i];
		Transform LclTransform = PxTransformConvert::TransformFromPx(ShapeItem->getLocalPose());

		physx::PxGeometryHolder Geom = ShapeItem->getGeometry();

		if (Geom.getType() == physx::PxGeometryType::eBOX)
		{
			const physx::PxBoxGeometry& BoxGeom = Geom.box();

			LclTransform.Scale =
			{ BoxGeom.halfExtents.x ,
				BoxGeom.halfExtents.y ,
				BoxGeom.halfExtents.z,
				0 };

			LclTransform.Scale = LclTransform.Scale * 2;

			LclTransform = LclTransform * GlobalTransform;

			DrawShape(
				RIToDraw->GetIncludedCollWorld(),
				ECollDebugDraw_MeshType::Box,
				LclTransform.AsMatrix(),
				LclTransform.Rotation,
				Color,
				bUseDepth,
				Time
			);
		}
	}

	
}