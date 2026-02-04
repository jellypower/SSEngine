#pragma once
#include "SSAssetDBManager/Private/DBColumnTypes/Columns_v_0.h"
#include "SSAssetDBManager/Public/IAssetDBLoader.h"

class ITextureAsset;

class AssetDBLoader : public IAssetDBLoader
{
public:
	AssetDBLoader();

public:
	bool StartLoadDB(const utf16* inFilePath) override;
	void ClearDB() override;

	void GenerateImportedAssets() override;

	void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager, ICommonRenderAssetSet* InCommonRenderAssetSet) override;
	void ClearAssetManagerToImportAsset() override;

	void RelocateImportedAssetsToAssetManager() override;

private:
	SS::StringW _BoundFilePath;
	SS::StringW _BoundFileNameOnly;
	bool _bIsEngineDefaultAssetDB = false;

	IAssetManagerMutable* _BoundAssetManager = nullptr;
	ICommonRenderAssetSet* _BoundCommonRenderAssets = nullptr;


	SS::PooledList<AssetDBColumn_Tex_v_0> _LoadedTextures;
};
