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