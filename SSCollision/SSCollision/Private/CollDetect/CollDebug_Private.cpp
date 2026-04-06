#include "pch.h"
#include "CollDebug_Private.h"

#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"
#include "SSCollision/Public/DEBUG/CollDebugDrawDescs.h"



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
