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
	bool LoadAllTexDB();
	bool LoadAllMtlDB();
	bool LoadAllMdls();

private:
	sqlite3* _hLoadedDB = nullptr;

	SS::SHasherW _BoundFilePath;
	SS::SHasherW _BoundDBNameSpace;
	bool _bIsEngineDefaultAssetDB = false;

	IAssetManagerMutable* _BoundAssetManager = nullptr;
	ICommonRenderAssetSet* _BoundCommonRenderAssets = nullptr;


private:
	SS::PooledList<AssetDBColumn_Tex_v_0> _LoadedTextures;
	SS::PooledList<AssetDBColumn_Mtl_DefaultPBR_v_0> _LoadedDefaultMtls;
	SS::PooledList<AssetDBColumn_Mdl_v_0> _LoadedMdls;


private:
	SS::PooledList<ITextureAsset*> _GeneratedTextures;

};
