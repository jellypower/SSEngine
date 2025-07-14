#pragma once
#include "MtlDataBase.h"

class ITextureAsset;

enum class EDefaultPBRMatTexTypes : int32
{
    None = -1,

    BaseColor = 0,
    Normal = 1,
    Metallic = 2,
    Emissive = 3,
    Occlusion = 4,

    Count = 5
};

struct MtlDataDefaultPBR : public MtlDataBase
{
	Vector4f _BaseColor;
	Vector4f _Emissive;
	float _NormalTexScale = 0;
	float _Metallic = 0;
	float _Roughness = 0;
	ITextureAsset* _Textures[(int32)EDefaultPBRMatTexTypes::Count] = { nullptr, };
};