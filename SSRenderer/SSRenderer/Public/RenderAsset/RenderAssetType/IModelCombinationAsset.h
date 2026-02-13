#pragma once
#include "IAssetBase.h"
#include "RAFileOutline/MeshRawDataHeaders.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"


constexpr int32 MDLC_PLACEMENTREF_ROOT_IDX = 0;

struct AssetPlacementReference
{
	Transform Transform;
	SS::SHasherW PlacementName;
	SS::SHasherW AssetName;
	EMeshType MeshType = EMeshType::None;
	int32 ParentIdx = INVALID_IDX;

	bool IsEmptyPlacement() const { return AssetName.IsEmpty(); }
};


class IModelCombinationAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::ModelCombination;

protected:
	SS::PooledList<AssetPlacementReference> _childs;

public:
	int32 GetChildCnt() const { return _childs.GetSize(); }
	const AssetPlacementReference& GetRootPlacementReference() const { return _childs[MDLC_PLACEMENTREF_ROOT_IDX]; }
	const AssetPlacementReference& GetChildAt(int32 Idx) const { return _childs[Idx]; }
};