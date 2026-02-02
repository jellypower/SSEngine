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
	Skeleton,
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
	case EAssetType::Skeleton: return "EAssetType::Skeleton";
	case EAssetType::RenderAnim: return "EAssetType::RenderAnim";
	case EAssetType::Count: return "EAssetType::Count";
	default: return "EAssetType::unknown";
	}
}


inline const char* GetAssetSuffix(EAssetType InAssetType)
{
	switch (InAssetType)
	{
	case EAssetType::Mesh: return ".mesh";
	case EAssetType::Material: return ".mtl";
	case EAssetType::Model: return ".mdl";
	case EAssetType::ModelCombination: return ".mdlc";
	case EAssetType::Texture: return ".tex";
	case EAssetType::RenderAnim: return ".ranim";
		break;
	}

	SS_ASSERT(false);
	return nullptr;
}