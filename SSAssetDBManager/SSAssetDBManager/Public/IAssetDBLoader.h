#pragma once

class IAssetBase;
class ICommonRenderAssetSet;
class IAssetManagerMutable;


class IAssetDBLoader : public INoncopyable
{
public:
	virtual bool StartLoadDB(SS::SHasherW inFilePath) = 0;
	virtual void ClearDB() = 0;

	

public:
	virtual void ClearInterData() = 0;

public:
	virtual bool LoadAllAssetDataFromDB() = 0;
	virtual void GenerateAssetInstancesFromInter() = 0;
	virtual void RelocateCreatedAssets(SS::PooledList<IAssetBase*>& OutAssetList) = 0;

public:
	virtual void PushAssetsToSaveToDB(const SS::PooledList<IAssetBase*>& InAssets) = 0;
	virtual void CreateInterListFromAssetsToSaveToDB(SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo) = 0;
	virtual bool SaveInterAssetsToDB() = 0;
	virtual void ClearAssetsToSaveToDB() = 0;
};

