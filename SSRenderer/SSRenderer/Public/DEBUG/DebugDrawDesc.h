#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IMeshAsset;

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
