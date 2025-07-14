#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"

class ITextureAsset;

class MaterialAsset : public IMaterialAssetMutable
{
private:
	SS::PooledList<ITextureAsset*, SS::InlineAllocator<8>> _ReferencingTextures;

public:
	MaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);
	virtual ~MaterialAsset();

public:
	EAssetType GetAssetType() const override;
	void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	void ReleaseGALData() override;
	void NotifyMtlDataModified() override;


};
