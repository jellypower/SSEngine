#pragma once
#include "SSEngineDefault/Public/SSVector.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct alignas(16) DefaultVertex {
	Vector4f Pos;
	Vector4f Normal;
	Vector4f Tangent;
	Vector2f Uv[VERTEX_UV_MAP_COUNT_MAX];
};

struct SkinnedVertex : DefaultVertex
{
	uint32 BoneIdx[VERTEX_SKINNING_BONE_COUNT_MAX] = { 0, };
	float Weight[VERTEX_SKINNING_BONE_COUNT_MAX] = { 0, };
};