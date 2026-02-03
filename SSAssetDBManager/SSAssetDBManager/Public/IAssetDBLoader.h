#pragma once

class IAssetBase;
class ICommonRenderAssetSet;
class IAssetManagerMutable;


class IAssetDBLoader : public INoncopyable
{
public:
	virtual bool BindDB(const utf16* inFilePath) = 0;
	virtual void ClearDB() = 0;

	virtual void GenerateImportedAssets() = 0;

	virtual void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager, ICommonRenderAssetSet* InCommonRenderAssetSet) = 0;
	virtual void ClearAssetManagerToImportAsset() = 0;

	virtual void RelocateImportedAssetsToAssetManager() = 0;
};

