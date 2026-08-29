#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelCombinationAssetMutable.h"





class ModelCombinationAsset : public IModelCombinationAssetMutable
{
public:
	ModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt, time_t LastUpdateTime);
	void Release() override;

public:
	virtual EAssetType GetAssetType() const override;

public:
	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	virtual void BindAssetManager(IAssetManager* InAssetManager) override;

public:
	virtual void ReserveChilds(int32 Capacity) override;
	virtual void ClearChilds() override;
	virtual void AddNewChild(const AssetPlacementReference& newReference) override;

private:

};
