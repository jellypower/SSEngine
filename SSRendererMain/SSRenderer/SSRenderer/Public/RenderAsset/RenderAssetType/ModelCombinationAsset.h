#pragma once
#include "MeshAsset.h"
#include "SSAssetBase.h"
#include "MeshData/IMeshRawData.h"


enum class EMeshType;
constexpr int32 MDLC_PLACEMENTREF_ROOT_IDX = 0;

struct AssetPlacementReference
{
	Transform Transform;
	SS::SHasherW PlacementName;
	SS::SHasherW AssetName;
	EMeshType MeshType = EMeshType::None;
	int32 ParentIdx = INVALID_IDX;
	SS::PooledList<int32> ChildIndices;

	AssetPlacementReference(AssetPlacementReference&& rhs);
	AssetPlacementReference(const AssetPlacementReference& rhs) = default;
	AssetPlacementReference();

	bool IsEmptyPlacement() const { return AssetName.IsEmpty(); }
};

class ModelCombinationAsset : public SSAssetBase
{
	friend class SSFBXImporter;
public:
	ModelCombinationAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ChildCnt);
	ModelCombinationAsset();

	void AddNewChild(const AssetPlacementReference& newReference);
	int32 GetChildCnt() const { return _childs.GetSize(); }
	const AssetPlacementReference& GetRootPlacementReference() const { return _childs[MDLC_PLACEMENTREF_ROOT_IDX]; }
	const AssetPlacementReference& GetChildAt(int32 Idx) const { return _childs[Idx]; }



private:
	SS::PooledList<AssetPlacementReference> _childs;
};
