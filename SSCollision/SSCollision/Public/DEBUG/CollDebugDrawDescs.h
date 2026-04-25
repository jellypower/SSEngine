#pragma once


enum class ECollDebugDraw_MeshType
{
	Point,
	Sphere,
	Box
};

struct CollDebugDrawDesc_Base
{
	Vector4f Color;
	bool bUseDepth = false;
	float Time = 0;
};

struct CDDD_Line : public CollDebugDrawDesc_Base
{
	Vector4f Start;
	Vector4f End;
};

struct CDDD_Mesh : public CollDebugDrawDesc_Base
{
	ECollDebugDraw_MeshType Type;
	XMMATRIX WMatrix;
	XMMATRIX RotMatrix;
	Vector4f Color;
};