#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"

class ITextureAsset;

class MaterialAsset : public IMaterialAssetMutable
{
private:
	SS::PooledList<ITextureAsset*, SS::InlineAllocator<8>> _ReferencingTextureCache;

public:
	MaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime);
	void Release() override;

public:
	EAssetType GetAssetType() const override;

public:
	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	virtual void BindAssetManager(IAssetManager* InAssetManager) override;

public:
	virtual void ReleaseGALData() override;
	virtual void ApplyMtlDataModify() override;
};
