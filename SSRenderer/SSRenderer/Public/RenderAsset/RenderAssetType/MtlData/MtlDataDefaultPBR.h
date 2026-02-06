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

inline const char* to_string(EDefaultPBRMatTexTypes e)
{
	switch (e)
	{
	case EDefaultPBRMatTexTypes::BaseColor: return "EDefaultPBRMatTexTypes::BaseColor";
	case EDefaultPBRMatTexTypes::Normal: return "EDefaultPBRMatTexTypes::Normal";
	case EDefaultPBRMatTexTypes::Metallic: return "EDefaultPBRMatTexTypes::Metallic";
	case EDefaultPBRMatTexTypes::Emissive: return "EDefaultPBRMatTexTypes::Emissive";
	case EDefaultPBRMatTexTypes::Occlusion: return "EDefaultPBRMatTexTypes::Occlusion";
	default: return "unknown";
	}
}

struct MtlDataDefaultPBR : public MtlDataBase
{
    MtlDataDefaultPBR()
    {
        _Type = EMaterialType::DefaultPBR;
    }

	Vector4f _BaseColorScale;
	Vector4f _EmissiveScale;
	float _NormalTexScale = 1;
	float _Metallic = 0;
	float _Roughness = 0;
	SS::SHasherW _TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Count];

	// Cached
	ITextureAsset* _CachedTextureRefs[(int32)EDefaultPBRMatTexTypes::Count] = {nullptr, };
};