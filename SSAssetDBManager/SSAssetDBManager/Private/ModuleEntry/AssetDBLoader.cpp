#include "pch.h"

#include "AssetDBLoader.h"

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"


AssetDBLoader::AssetDBLoader()
{
	_LoadedTextures.Reserve(200);
}

bool AssetDBLoader::StartLoadDB(const utf16* inFilePath)
{
	_BoundFilePath = inFilePath;

	SS::StringW strBoundDBNameSpace;
	ExtractFileNameFromPath(strBoundDBNameSpace, inFilePath);
	_BoundDBNameSpace = strBoundDBNameSpace.C_Str();

	int Result = sqlite3_open16(inFilePath, &_hLoadedDB);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot open file.");

		_BoundFilePath = SS::SHasherW();
		_BoundDBNameSpace = SS::SHasherW();
		_bIsEngineDefaultAssetDB = false;

		sqlite3_close(_hLoadedDB);
		_hLoadedDB = nullptr;

		return false;
	}

	bool bResult = LoadAllTexDB();
	SS_ASSERT(bResult);

	bResult = LoadAllMtlDB();
	SS_ASSERT(bResult);

	return true;
}

void AssetDBLoader::ClearDB()
{
	if (_hLoadedDB)
	{
		sqlite3_close(_hLoadedDB);
		_hLoadedDB = nullptr;
	}

	_BoundFilePath = SS::SHasherW();
	_BoundDBNameSpace = SS::SHasherW();
	_bIsEngineDefaultAssetDB = false;
}

void AssetDBLoader::GenerateImportedAssets()
{
	for (const AssetDBColumn_Tex_v_0& TexColumnItem : _LoadedTextures)
	{
		ITextureAssetMutable* NewTex = _BoundAssetManager->CreateEmptyTextureAsset(_BoundDBNameSpace, 
			TexColumnItem.AssetName, TexColumnItem.AssetPath, TexColumnItem.TextureType);

		_GeneratedTextures.PushBack(NewTex);
	}

	for (const AssetDBColumn_Mtl_DefaultPBR_v_0& DefaultMtlColumnItem : _LoadedDefaultMtls)
	{
		IMaterialAssetMutable* NewMtl = _BoundAssetManager->CreateEmptyMaterialAsset(_BoundDBNameSpace,
			DefaultMtlColumnItem.AssetName, DefaultMtlColumnItem.AssetPath);

		MtlDataDefaultPBR* NewDefaultPBRMtlData = DBG_NEW MtlDataDefaultPBR;
		NewDefaultPBRMtlData->_BaseColorScale = DefaultMtlColumnItem._BaseColorScale;
		NewDefaultPBRMtlData->_EmissiveScale = DefaultMtlColumnItem._EmissiveScale;
		NewDefaultPBRMtlData->_NormalTexScale = DefaultMtlColumnItem.NormalTexScale;
		NewDefaultPBRMtlData->_Metallic = DefaultMtlColumnItem.Metallic;
		NewDefaultPBRMtlData->_Roughness = DefaultMtlColumnItem.Roughness;
		NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::BaseColor]	= DefaultMtlColumnItem.;
		NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Normal]		= DefaultMtlColumnItem.;
		NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Metallic]	= DefaultMtlColumnItem.;
		NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Emissive]	= DefaultMtlColumnItem.;
		NewDefaultPBRMtlData->_Textures[(int32)EDefaultPBRMatTexTypes::Occlusion]	= DefaultMtlColumnItem.;

		NewMtl->InjectRawDataXXX(NewDefaultPBRMtlData);
	}
}

void AssetDBLoader::BindAssetManagerToImportAsset(
	IAssetManagerMutable* InAssetMnanager,
	ICommonRenderAssetSet* InCommonRenderAssetSet)
{
	_BoundAssetManager = InAssetMnanager;
	_BoundCommonRenderAssets = InCommonRenderAssetSet;
}

void AssetDBLoader::ClearAssetManagerToImportAsset()
{

}

void AssetDBLoader::RelocateImportedAssetsToAssetManager()
{
	for (ITextureAsset* TexItem : _GeneratedTextures)
	{
		_BoundAssetManager->AddToAssetPool(TexItem);
	}
	_GeneratedTextures.Clear();
}

bool AssetDBLoader::LoadAllTexDB()
{
	constexpr utf16 ALL_TEXTURE_QUEERY[] = L"SELECT * from Textures";
	constexpr int32 ALL_TEXTURE_QUEERY_SIZE = sizeof(ALL_TEXTURE_QUEERY);

	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;


	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		ALL_TEXTURE_QUEERY,
		ALL_TEXTURE_QUEERY_SIZE,
		SQLITE_OPEN_READONLY,
		&StmtResult,
		&__Temp);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot compile stmt.");
		sqlite3_finalize(StmtResult);
		return false;
	}

	while (sqlite3_step(StmtResult) == SQLITE_ROW)
	{
		SS::StringW AssetNameStr = _BoundDBNameSpace.C_Str();
		AssetNameStr += L"/";

		const utf16* db_c_str = nullptr;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 1);
		SS::StringW AssetPathStr = db_c_str;

		time_t UpdateTime = sqlite3_column_int64(StmtResult, 2);

		ETextureType TexType = (ETextureType)sqlite3_column_int(StmtResult, 3);
		SS_ASSERT(ETextureType::None <= TexType && TexType < ETextureType::Count);

		AssetDBColumn_Tex_v_0 NewColumn;
		NewColumn.AssetName = AssetNameStr.C_Str();
		NewColumn.AssetPath = AssetPathStr.C_Str();
		NewColumn.LastUpdateTime = UpdateTime;
		NewColumn.TextureType = TexType;

		_LoadedTextures.PushBack(NewColumn);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::LoadAllMtlDB()
{
	constexpr utf16 ALL_MTL_QUERY[] = L"SELECT * from Materials";
	constexpr int32 ALL_MTL_QUERY_SIZE = sizeof(ALL_MTL_QUERY);

	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;


	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		ALL_MTL_QUERY,
		ALL_MTL_QUERY_SIZE,
		SQLITE_OPEN_READONLY,
		&StmtResult,
		&__Temp);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot compile stmt.");
		sqlite3_finalize(StmtResult);
		return false;
	}

	while (sqlite3_step(StmtResult) == SQLITE_ROW)
	{
		AssetDBColumn_Mtl_DefaultPBR_v_0 NewColumn;


		SS::StringW AssetNameStr = _BoundDBNameSpace.C_Str();
		AssetNameStr += L"/";
		SS::StringW AssetPathStr = _BoundFilePath.C_Str();
		AssetPathStr += L"/";

		const utf16* db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 1);
		AssetPathStr += db_c_str;

		time_t UpdateTime = sqlite3_column_int64(StmtResult, 2);


		NewColumn.AssetName = AssetNameStr.C_Str();
		NewColumn.AssetPath = AssetPathStr.C_Str();
		NewColumn.LastUpdateTime = UpdateTime;


		NewColumn._BaseColorScale.X = sqlite3_column_double(StmtResult, 3);
		NewColumn._BaseColorScale.Y = sqlite3_column_double(StmtResult, 4);
		NewColumn._BaseColorScale.Z = sqlite3_column_double(StmtResult, 5);
		NewColumn._BaseColorScale.W = 1;

		NewColumn._EmissiveScale.X = sqlite3_column_double(StmtResult, 6);
		NewColumn._EmissiveScale.Y = sqlite3_column_double(StmtResult, 7);
		NewColumn._EmissiveScale.Z = sqlite3_column_double(StmtResult, 8);
		NewColumn._EmissiveScale.W = 1;

		NewColumn.NormalTexScale = sqlite3_column_double(StmtResult, 9);
		NewColumn.Metallic = sqlite3_column_double(StmtResult, 10);
		NewColumn.Roughness = sqlite3_column_double(StmtResult, 11);


		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 12);
		NewColumn.Textures[(int32)EDefaultPBRMatTexTypes::BaseColor] = db_c_str;
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 13);
		NewColumn.Textures[(int32)EDefaultPBRMatTexTypes::Normal] = db_c_str;
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 14);
		NewColumn.Textures[(int32)EDefaultPBRMatTexTypes::Metallic] = db_c_str;
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 15);
		NewColumn.Textures[(int32)EDefaultPBRMatTexTypes::Emissive] = db_c_str;
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 16);
		NewColumn.Textures[(int32)EDefaultPBRMatTexTypes::Occlusion] = db_c_str;

		_LoadedDefaultMtls.PushBack(NewColumn);
	}


	sqlite3_finalize(StmtResult);
	return true;
}
