#include "pch.h"
#include "CollDebug_Private.h"

#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/DEBUG/CollDebugDrawDescs.h"


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
	Vector4f Min = CollInstance->GetBBMin();
	Vector4f Max = CollInstance->GetBBMax();


	Transform DrawTransform;
	DrawTransform.Position = CollInstance->GetWorldPos();
	DrawTransform.Scale = Max - Min;

	
	CDDD_Mesh Desc;
	Desc.WMatrix = DrawTransform.AsMatrix();
	Desc.RotMatrix = XMMatrixIdentity();
	Desc.Color = Color;
	Desc.bUseDepth = bUseDepth;
	Desc.Time = Time;
	Desc.Type = ECollDebugDraw_MeshType::Box;
	InWorldToDraw->AddDrawDebugMesh(Desc);
}

void CollDebug_Private::DrawPoint(ICollisionWorld* InWorldToDraw, const Vector4f& Pos, const Vector4f& Color,
	bool bUseDepth, float Scale, float Time)
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