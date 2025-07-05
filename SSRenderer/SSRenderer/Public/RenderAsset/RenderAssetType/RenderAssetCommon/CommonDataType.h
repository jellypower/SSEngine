#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

enum class EAssetType : int32
{
	None = 0,

	Mesh,
	Material,
	Model,
	ModelCombination,
	Texture,
	Skeleton,
	SkeletonAnim,

	Count
};
