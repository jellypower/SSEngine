#pragma once
#include "SSAssetDBManager/Public/IAssetDBLoader.h"

class AssetDBLoader : public IAssetDBLoader
{
public:
	bool BindDB(const utf16* inFilePath) override;
	void ClearDB() override;

	void GenerateImportedAssets() override;

	void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager, ICommonRenderAssetSet* InCommonRenderAssetSet) override;
	void ClearAssetManagerToImportAsset() override;

	void RelocateImportedAssetsToAssetManager() override;

private:

};
