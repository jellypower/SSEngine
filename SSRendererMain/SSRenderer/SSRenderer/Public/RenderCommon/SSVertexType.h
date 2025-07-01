#pragma once
#include "SSEngineDefault/Public/SSVector.h"


struct alignas(16) SSDefaultVertex {
	Vector4f Pos;
	Vector4f Normal;
	Vector4f Tangent;
	Vector2f Uv[2];
};

struct SSSkinnedVertex : SSDefaultVertex
{
	uint32 BoneIdx[4] = { 0, };
	float Weight[4] = { 0, };
};