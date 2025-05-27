#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class SSAssetBase;

class AssetManagerBase : public INoncopyable
{
protected:
	SS::HashMap<SS::SHasherW, SSAssetBase*> _assetHashMap;

public:
	AssetManagerBase(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity);
	virtual ~AssetManagerBase();

	virtual void AddToAssetPool(SSAssetBase* newAsset);
	virtual void ReleaseAllAssets();
	virtual SS::SHasherW GenerateAssetName(const SS::StringW& fileName, const SS::StringW& nodeName, const utf16* suffix) const;

	virtual SSAssetBase* FindAssetByName(SS::SHasherW InModelAssetName) const;

};
