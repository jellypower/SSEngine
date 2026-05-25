#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IMeshAsset;



struct DebugDrawLineDesc
{
	Vector4f Start;
	Vector4f End;
	Vector4f Color;
	bool bUseDepth = false;
};

struct TimedDebugDrawLineDesc
{
	DebugDrawLineDesc LineDesc;
	float Time = 0;
};


struct DebugDrawMeshDesc
{
	IMeshAsset* MeshAsset = nullptr;
	bool bUseDepth = false;
	XMMATRIX WMatrix;
	XMMATRIX RotMatrix;
	Vector4f DrawColor;
};

struct TimedDebugDrawMeshDesc
{
	DebugDrawMeshDesc RenderDesc;
	float Time = 0;
};
