#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IRenderAnimAssetMutable.h"

class RenderAnimAsset : public IRenderAnimAssetMutable
{
public:
	RenderAnimAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);
	virtual ~RenderAnimAsset();

public:
	EAssetType GetAssetType() const override;

	const RenderAnimRawData* GetKeyFrameAnimData() const override;
	const IModelCombinationAsset* GetOriginalMdlcAsset() const override;

public:
	void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;

	RenderAnimRawData* GetMutableRawData() override;
	void InjectRawDataXXX(RenderAnimRawData* InRenderAnimData) override;
	void SetOriginMdlcAsset(IModelCombinationAsset* InMdlcAsset) override;


private:
	IModelCombinationAsset* _OriginalMdlcAsset = nullptr;
	RenderAnimRawData* _AnimRawData = nullptr;
};
