#pragma once
#include "SSAssetDBManager/Private/DBColumnTypes/Rows_v_0.h"
#include "SSAssetDBManager/Public/IAssetDBLoader.h"

class IModelCombinationAsset;
class IModelAsset;
class IMeshAsset;
class IMaterialAsset;
class ITextureAsset;

class AssetDBLoader : public IAssetDBLoader
{
public:
	AssetDBLoader();

public:
	bool StartLoadDB(SS::SHasherW InNameSpace) override;
	void ClearDB() override;


	void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager, ICommonRenderAssetSet* InCommonRenderAssetSet) override;
	void ClearAssetManagerToImportAsset() override;

public:
	virtual void ClearLoadedAssetData() override;

public:
	virtual bool LoadAllAssetDataFromDB() override;
	virtual void CreateLoadedAssetInstances() override;
	virtual void RelocateCreatedAssetInstancesToAssetManager() override;

public:
	virtual void PushAssetsToSaveToDB(const SS::PooledList<IAssetBase*>& InAssets) override;
	virtual void LoadAssetListFromAssetsToSaveToDB() override;
	virtual void ClearAssetsToSaveToDB() override;
	virtual bool SaveLoadedAssetsToDB() override;

private:
	void FillEmptyAssetsFromApakFile();

private:
	bool LoadAllLoadedTex();
	bool LoadAllLoadedMtl();
	bool LoadAllLoadedMdls();
	bool LoadAllLoadedMeshes();
	bool LoadAllLoadedMdlcs();

private:
	bool SaveAllLoadedMeshesToDB();
	bool SaveAllLoadedMdlsToDB();
	bool SaveAllLoadedMdlcsToDB();

private:
	sqlite3* _hLoadedDB = nullptr;

	SS::SHasherW _BoundDBNameSpacePath;
	SS::SHasherW _BoundDBSqlFilePath;
	SS::SHasherW _BoundDBNameSpace;
	bool _bIsEngineDefaultAssetDB = false;

	IAssetManagerMutable* _BoundAssetManager = nullptr;
	ICommonRenderAssetSet* _BoundCommonRenderAssets = nullptr;


private:
	SS::PooledList<AssetDBRow_Tex_v_0> _LoadedTextures;
	SS::PooledList<AssetDBRow_Mesh_v_0> _LoadedMeshes;
	SS::PooledList<AssetDBRow_Mtl_DefaultPBR_v_0> _LoadedDefaultMtls;
	SS::PooledList<AssetDBRow_Mdl_v_0> _LoadedMdls;
	SS::PooledList<AssetDBRow_Mdlc_v_0> _LoadedMdlcs;



private:
	SS::PooledList<IAssetBase*> _AllAssetInstancesSortedByPath;
	SS::PooledList<ITextureAsset*> _CreatedTextures;
	SS::PooledList<IMeshAsset*> _CreatedMeshes;
	SS::PooledList<IMaterialAsset*> _CreatedMaterials;
	SS::PooledList<IModelAsset*> _CreatedMdls;
	SS::PooledList<IModelCombinationAsset*> _CreatedMdlcs;

private:
	SS::PooledList<IAssetBase*> _AssetsToSaveToDB;

private:
	SS::PooledList<utf16, SS::InlineAllocator<256>> _StringWorkTable;
	SS::PooledList<byte> _FileDataWorkTable;
};
