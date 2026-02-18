#pragma once

class IAssetBase;
class ICommonRenderAssetSet;
class IAssetManagerMutable;


class IAssetDBLoader : public INoncopyable
{
public:
	virtual bool StartLoadDB(SS::SHasherW inFilePath) = 0;
	virtual void ClearDB() = 0;

	virtual void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager, ICommonRenderAssetSet* InCommonRenderAssetSet) = 0;
	virtual void ClearAssetManagerToImportAsset() = 0;


public:
	virtual void ClearInterData() = 0;

public:
	virtual bool LoadAllAssetDataFromDB() = 0;
	virtual void CreateAssetInstancesFromInter() = 0;
	virtual void RelocateCreatedAssetInstancesToAssetManager() = 0;

public:
	virtual void PushAssetsToSaveToDB(const SS::PooledList<IAssetBase*>& InAssets) = 0;
	virtual void CreateInterListFromAssetsToSaveToDB() = 0;
	virtual bool SaveInterAssetsToDB() = 0;
	virtual void ClearAssetsToSaveToDB() = 0;
};

