#include "pch.h"
#include "AssetDBLoader.h"

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"


constexpr utf16 ALL_TEXTURE_QUEERY[] = L"SELECT * from Textures";
constexpr int32 ALL_TEXTURE_QUEERY_SIZE = sizeof(ALL_TEXTURE_QUEERY);


AssetDBLoader::AssetDBLoader()
{
	_LoadedTextures.Reserve(200);
}

bool AssetDBLoader::StartLoadDB(const utf16* inFilePath)
{
	sqlite3* db = nullptr;
	const void* __Temp = nullptr;
	sqlite3_stmt* StmtResult = nullptr;
	_BoundFilePath = inFilePath;
	ExtractFileNameFromPath(_BoundFileNameOnly, _BoundFilePath.C_Str());


	int Result = sqlite3_open16(inFilePath, &db);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot open file.");
		goto lb_fail;
	}


	Result = sqlite3_prepare16_v3(
		db,
		ALL_TEXTURE_QUEERY,
		ALL_TEXTURE_QUEERY_SIZE,
		SQLITE_OPEN_READONLY,
		&StmtResult,
		&__Temp);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot compile stmt.");
		goto lb_fail;
	}

	while ( sqlite3_step(StmtResult) == SQLITE_ROW)
	{
		SS::StringW AssetNameStr = _BoundFileNameOnly;
		AssetNameStr += L"/";
		SS::StringW AssetPathStr = _BoundFilePath;
		AssetPathStr += L"/";

		const utf16* db_c_str = nullptr;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 1);
		AssetPathStr += db_c_str;

		ETextureType TexType = (ETextureType)sqlite3_column_int(StmtResult, 2);
		SS_ASSERT(ETextureType::None <= TexType && TexType < ETextureType::Count);

		AssetDBColumn_Tex_v_0 NewColumn;
		NewColumn.AssetName = AssetNameStr.C_Str();
		NewColumn.AssetPath = AssetPathStr.C_Str();
		NewColumn.TextureType = TexType;

		_LoadedTextures.PushBack(NewColumn);
	}

	sqlite3_finalize(StmtResult);



	sqlite3_close(db);
	return true;

lb_fail:
	if (db != nullptr)
	{
		sqlite3_close(db);
	}

	_BoundFilePath.Clear();
	return false;
}

void AssetDBLoader::ClearDB()
{

}

void AssetDBLoader::GenerateImportedAssets()
{

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

}
