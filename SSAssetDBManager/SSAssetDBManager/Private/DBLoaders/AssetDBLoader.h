#pragma once
#include "SSAssetDBManager/Private/DBColumnTypes/Rows_v_0.h"
#include "SSAssetDBManager/Public/IAssetDBLoader.h"

class IRenderAnimAsset;
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
	virtual void ClearInterData() override;

public:
	virtual bool LoadAllAssetDataFromDB() override;
	virtual void CreateAssetInstancesFromInter() override;
	virtual void RelocateCreatedAssetInstancesToAssetManager() override;

public:
	virtual void PushAssetsToSaveToDB(const SS::PooledList<IAssetBase*>& InAssets) override;
	virtual void CreateInterListFromAssetsToSaveToDB(SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo) override;
	virtual void ClearAssetsToSaveToDB() override;
	virtual bool SaveInterAssetsToDB() override;

private:
	void FillEmptyAssetsFromApakFile();

private:
	bool LoadDBInterAllTex();
	bool LoadDBInterAllMtl();
	bool LoadDBInterAllMdls();
	bool LoadDBInterAllMeshes();
	bool LoadDBInterAllMdlcs();
	bool LoadDBInterAllRAnims();

private:
	bool SaveAllInterMeshesToDB();
	bool SaveAllInterMtlsToDB();
	bool SaveAllInterMdlsToDB();
	bool SaveAllInterMdlcsToDB();
	bool SaveAllInterRAnimsToDB();

private:
	sqlite3* _hLoadedDB = nullptr;

	SS::SHasherW _BoundDBNameSpacePath;
	SS::SHasherW _BoundDBSqlFilePath;
	SS::SHasherW _BoundDBNameSpace;
	bool _bIsEngineDefaultAssetDB = false;

	IAssetManagerMutable* _BoundAssetManager = nullptr;
	ICommonRenderAssetSet* _BoundCommonRenderAssets = nullptr;


private:
	SS::PooledList<AssetDBRow_Tex_v_0> _DBInterTextures;
	SS::PooledList<AssetDBRow_Mesh_v_0> _DBInterMeshes;
	SS::PooledList<AssetDBRow_Mtl_DefaultPBR_v_0> _DBInterDefaultMtls;
	SS::PooledList<AssetDBRow_Mdl_v_0> _DBInterMdls;
	SS::PooledList<AssetDBRow_Mdlc_v_0> _DBInterMdlcs;
	SS::PooledList<AssetDBRow_RAnim_v_0> _DBInterRAnims;



private:
	SS::PooledList<IAssetBase*> _AllAssetInstancesSortedByPath;
	SS::PooledList<ITextureAsset*> _CreatedTextures;
	SS::PooledList<IMeshAsset*> _CreatedMeshes;
	SS::PooledList<IMaterialAsset*> _CreatedMaterials;
	SS::PooledList<IModelAsset*> _CreatedMdls;
	SS::PooledList<IModelCombinationAsset*> _CreatedMdlcs;
	SS::PooledList<IRenderAnimAsset*> _CreatedRAnims;

private:
	SS::PooledList<IAssetBase*> _AssetsToSaveToDB;

private:
	SS::PooledList<utf16, SS::InlineAllocator<256>> _StringWorkTable;
	SS::PooledList<byte> _FileDataWorkTable;
};
