#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelCombinationAsset.h"

class IModelCombinationAssetMutable : public IModelCombinationAsset
{
public:
	AssetPlacementReference& GetChildAtMutable(int32 Idx) { return _childs[Idx]; }

public:
	virtual void AddNewChild(const AssetPlacementReference& newReference) = 0;
};