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

public:
	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	virtual void BindAssetManager(IAssetManager* InAssetManager) override;

public:
	void ReleaseGALData() override;

public:
	virtual ETextureType GetTextureType() const override;
};
