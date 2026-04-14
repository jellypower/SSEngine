#pragma once

enum class ECollDebugDraw_MeshType;
struct CDDD_Line;
class ICollInstanceBase;
class ICollisionWorld;



static class CollDebug_Private
{
public:
	static void DrawLine(
		ICollisionWorld* InWorldToDraw,
		const CDDD_Line& Desc);

	static void DrawSimplex(
		ICollisionWorld* InWorldToDraw, 
		const SimplexV4& InSimplex, 
		const Vector4f& Pos,
		const Vector4f& Color = Vector4f::Zero,
		bool bUseDepth = false,
		float Time = 0);

	static void DrawShape(
		ICollisionWorld* InWorldToDraw,
		ECollDebugDraw_MeshType MeshType,
		const XMMATRIX& TransformMat,
		const Quaternion& Rot,
		const Vector4f& Color = Vector4f::Zero,
		bool bUseDepth = false,
		float Time = 0);

	static void DrawBoundBox(
		ICollisionWorld* InWorldToDraw,
		const ICollInstanceBase* CollInstance,
		const Vector4f& Color = Vector4f::Zero,
		bool bUseDepth = false,
		float Time = 0);


	static void DrawPoint(
		ICollisionWorld* InWorldToDraw,
		const Vector4f& Pos,
		const Vector4f& Color = Vector4f::Zero,
		bool bUseDepth = false,
		float Scale = 0.1,
		float Time = 0);
};

