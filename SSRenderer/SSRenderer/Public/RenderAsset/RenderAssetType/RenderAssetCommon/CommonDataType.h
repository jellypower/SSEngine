#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

enum class EAssetType : int32
{
	None = 0,

	Mesh,
	Material,
	Model,
	ModelCombination,
	Texture,
	RenderAnim,

	Count
};

inline const char* to_string(EAssetType e)
{
	switch (e)
	{
	case EAssetType::None: return "EAssetType::None";
	case EAssetType::Mesh: return "EAssetType::Mesh";
	case EAssetType::Material: return "EAssetType::Material";
	case EAssetType::Model: return "EAssetType::Model";
	case EAssetType::ModelCombination: return "EAssetType::ModelCombination";
	case EAssetType::Texture: return "EAssetType::Texture";
	case EAssetType::RenderAnim: return "EAssetType::RenderAnim";
	case EAssetType::Count: return "EAssetType::Count";
	default: return "EAssetType::unknown";
	}
}


inline constexpr const utf16* GetAssetSuffix(EAssetType InAssetType)
{
	switch (InAssetType)
	{
	case EAssetType::Mesh: return L".mesh";
	case EAssetType::Material: return L".mtl";
	case EAssetType::Model: return L".mdl";
	case EAssetType::ModelCombination: return L".mdlc";
	case EAssetType::Texture: return L".tex";
	case EAssetType::RenderAnim: return L".ranim";
		
	default: return L".unknown";
	}
}