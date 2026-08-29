#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IRenderAnimAssetMutable.h"

class RenderAnimAsset : public IRenderAnimAssetMutable
{

public:
	RenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime);
	void Release() override;

public:
	EAssetType GetAssetType() const override;

	const RenderAnimRawData* GetKeyFrameAnimData() const override;
	const IModelCombinationAsset* GetOriginalMdlcAsset() const override;

public:
	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	virtual void BindAssetManager(IAssetManager* InAssetManager) override;

public:
	RenderAnimRawData* GetMutableRawData() override;
	void InjectRawDataXXX(RenderAnimRawData* InRenderAnimData) override;
	void SetOriginMdlcAsset(IModelCombinationAsset* InMdlcAsset) override;


private:
	IModelCombinationAsset* _OriginalMdlcAsset = nullptr;
	RenderAnimRawData* _AnimRawData = nullptr;
};
