#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelCombinationAsset.h"

class IModelCombinationAssetMutable : public IModelCombinationAsset
{
public:
	AssetPlacementReference& GetChildAtMutable(int32 Idx) { return _childs[Idx]; }

public:
	virtual void SetHeader(const MdlcAssetHeader& InHeader) { _Header = InHeader; }

	virtual void ReserveChilds(int32 Capacity) = 0;
	virtual void ClearChilds() = 0;
	virtual void AddNewChild(const AssetPlacementReference& newReference) = 0;
};