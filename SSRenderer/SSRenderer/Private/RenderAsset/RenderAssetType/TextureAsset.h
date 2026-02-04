#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"

class TextureAsset : public ITextureAssetMutable
{
private:
	ETextureType _Type = ETextureType::None;

public:
	TextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, ETextureType InType);

public:
	EAssetType GetAssetType() const override;
	void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	void ReleaseGALData() override;

public:
	virtual ETextureType GetTextureType() const override;
};
