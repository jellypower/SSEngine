#pragma once

class IAssetBase;
class ICommonRenderAssetSet;
class IAssetManagerMutable;


class IAssetDBLoader : public INoncopyable
{
public:
	virtual bool StartLoadDB(const utf16* inFilePath) = 0;
	virtual void ClearDB() = 0;

	virtual void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager, ICommonRenderAssetSet* InCommonRenderAssetSet) = 0;
	virtual void ClearAssetManagerToImportAsset() = 0;


public:
	virtual bool LoadAllAssetDataFromDB() = 0;
	virtual void GenerateLoadedAssets() = 0;
	virtual void ClearLoadedAssetData() = 0;
	virtual void RelocateGeneratedAssetsToAssetManager() = 0;
};

