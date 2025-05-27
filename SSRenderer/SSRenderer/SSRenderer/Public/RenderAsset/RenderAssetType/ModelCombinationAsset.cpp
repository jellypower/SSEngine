#include "ModelCombinationAsset.h"

AssetPlacementReference::AssetPlacementReference(AssetPlacementReference&& rhs)
{
	Transform = rhs.Transform;
	PlacementName = rhs.PlacementName;
	AssetName = rhs.AssetName;
	MeshType = rhs.MeshType;
	ParentIdx = rhs.ParentIdx;

	ChildIndices = SS::move(rhs.ChildIndices);
}

AssetPlacementReference::AssetPlacementReference()
{
}

ModelCombinationAsset::ModelCombinationAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ChildCnt)
	: SSAssetBase(EAssetType::ModelCombination, InAssetName, InAssetPath)
{
	_childs.Reserve(ChildCnt);
}

ModelCombinationAsset::ModelCombinationAsset()
	: SSAssetBase(EAssetType::ModelCombination)
{
}

void ModelCombinationAsset::AddNewChild(const AssetPlacementReference& newReference)
{
	_childs.PushBack(SS::move(newReference));
}
