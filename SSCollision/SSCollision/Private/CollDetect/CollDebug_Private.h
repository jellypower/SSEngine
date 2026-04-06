#pragma once

class ICollisionWorld;



static class CollDebug_Private
{
public:
	static void DrawSimplex(
		ICollisionWorld* InWorldToDraw, 
		const SimplexV4& InSimplex, 
		const Vector4f& Pos,
		const Vector4f& Color = Vector4f::Zero,
		bool bUseDepth = false,
		float Time = 0);
};
