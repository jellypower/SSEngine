#pragma once

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

enum class ETextureType;



struct AssetDBColumn_Tex_v_0
{
	SS::SHasherW AssetName;
	SS::SHasherW AssetPath;
	time_t LastUpdateTime;

	ETextureType TextureType;
};

struct AssetDBColumn_Mtl_DefaultPBR_v_0
{
	SS::SHasherW AssetName;
	SS::SHasherW AssetPath;
	time_t LastUpdateTime;

	Vector4f _BaseColorScale;
	Vector4f _EmissiveScale;
	float NormalTexScale = 1;
	float Metallic = 0;
	float Roughness = 0;
	SS::SHasherW Textures[(int32)EDefaultPBRMatTexTypes::Count];
};

struct AssetDBColumn_Mdl_v_0
{
	SS::SHasherW AssetName;
	SS::SHasherW AssetPath;
	time_t LastUpdateTime;

	int32 SubMeshCnt = 0;
	SS::SHasherW MeshName;
	SS::SHasherW MtlNames[SUBMESH_COUNT_MAX];
};