#pragma once
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"
#include "SSEngineDefault/Public/SSVector.h"


struct alignas(16) SSDefaultVertex {
	Vector4f Pos;
	Vector4f Normal;
	Vector4f Tangent;
	Vector2f Uv[DEFAULT_VERTEX_UV_TYPE_CNT_MAX];
};

struct alignas(16) SSSkinnedVertex : SSDefaultVertex
{
	uint32 BoneIdx[VERTEX_SKINNING_BONE_COUNT_MAX] = { 0, };
	float Weight[VERTEX_SKINNING_BONE_COUNT_MAX] = { 0, };
};