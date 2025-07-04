#pragma once
#include "RenderAssetType/RenderAssetCommon/CommonDataType.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class IAssetBase;

class IAssetManager : public INoncopyable
{
public:
	virtual void AddToAssetPool(IAssetBase* newAsset) = 0;
	virtual void ReleaseAllAssets() = 0;

	virtual SS::SHasherW GenerateAssetName(const SS::StringW& fileName, const SS::StringW& nodeName, EAssetType InAssetType) const = 0;
	virtual IAssetBase* FindAssetByName(SS::SHasherW InModelAssetName, EAssetType InAssetType) const = 0;
};
