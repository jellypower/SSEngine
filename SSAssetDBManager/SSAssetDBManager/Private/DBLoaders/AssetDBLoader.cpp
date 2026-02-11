#include "pch.h"

#include "AssetDBLoader.h"

#include "SSAssetDBManager/Private/DBColumnTypes/DBRowUtils.h"
#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"

#include "DBQueries.h"

AssetDBLoader::AssetDBLoader()
{
	_LoadedTextures.Reserve(128);
	_LoadedMeshes.Reserve(128);
	_LoadedDefaultMtls.Reserve(128);
	_LoadedMdls.Reserve(128);
	_LoadedMdlcs.Reserve(128);
}

bool AssetDBLoader::StartLoadDB(const utf16* inFilePath)
{
	if (_BoundFilePath.IsEmpty() == false)
	{
		SS_ASSERT(false);
		return false;
	}

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

	return true;
}

void AssetDBLoader::ClearDB()
{
	if (_hLoadedDB)
	{
		sqlite3_close(_hLoadedDB);
		_hLoadedDB = nullptr;
	}

	ClearLoadedAssetData();

	_BoundFilePath = SS::SHasherW();
	_BoundDBNameSpace = SS::SHasherW();
	_bIsEngineDefaultAssetDB = false;
}

bool AssetDBLoader::LoadAllAssetDataFromDB()
{
	bool bResult = LoadAllLoadedTex();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = LoadAllLoadedMeshes();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = LoadAllLoadedMtl();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = LoadAllLoadedMdls();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = LoadAllLoadedMdlcs();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	return true;
}

void AssetDBLoader::CreateLoadedAssetInstances()
{
	for (const AssetDBRow_Tex_v_0& TexColumnItem : _LoadedTextures)
	{
		ITextureAssetMutable* NewTex = _BoundAssetManager->CreateEmptyTextureAsset(_BoundDBNameSpace,
			TexColumnItem.AssetName, TexColumnItem.AssetPath, TexColumnItem.TextureType);

		_GeneratedTextures.PushBack(NewTex);
	}

	SS_ASSERT_MSG(false, L"여기서 계속하기 -> _LoadedMeshes 생성 잘 해내기");
	for (const AssetDBRow_Mesh_v_0& MeshRowItem : _LoadedMeshes)
	{
		IMeshAssetMutable* NewAsset = _BoundAssetManager->CreateEmptyMeshAsset(_BoundDBNameSpace,
			MeshRowItem.AssetName, MeshRowItem.AssetPath);

		_GeneratedMeshes.PushBack(NewAsset);
	}

	for (const AssetDBRow_Mtl_DefaultPBR_v_0& DefaultMtlColumnItem : _LoadedDefaultMtls)
	{
		IMaterialAssetMutable* NewMtl = _BoundAssetManager->CreateEmptyMaterialAsset(_BoundDBNameSpace,
			DefaultMtlColumnItem.AssetName, DefaultMtlColumnItem.AssetPath);

		MtlDataDefaultPBR* NewDefaultPBRMtlData = DBG_NEW MtlDataDefaultPBR;
		NewDefaultPBRMtlData->_BaseColorScale = DefaultMtlColumnItem._BaseColorScale;
		NewDefaultPBRMtlData->_EmissiveScale = DefaultMtlColumnItem._EmissiveScale;
		NewDefaultPBRMtlData->_NormalTexScale = DefaultMtlColumnItem.NormalTexScale;
		NewDefaultPBRMtlData->_Metallic = DefaultMtlColumnItem.Metallic;
		NewDefaultPBRMtlData->_Roughness = DefaultMtlColumnItem.Roughness;
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::BaseColor] = DefaultMtlColumnItem.Textures[(int32)EDefaultPBRMatTexTypes::BaseColor];
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Normal] = DefaultMtlColumnItem.Textures[(int32)EDefaultPBRMatTexTypes::Normal];
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Metallic] = DefaultMtlColumnItem.Textures[(int32)EDefaultPBRMatTexTypes::Metallic];
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Emissive] = DefaultMtlColumnItem.Textures[(int32)EDefaultPBRMatTexTypes::Emissive];
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Occlusion] = DefaultMtlColumnItem.Textures[(int32)EDefaultPBRMatTexTypes::Occlusion];

		NewMtl->InjectRawDataXXX(NewDefaultPBRMtlData);

		_GeneratedMaterials.PushBack(NewMtl);
	}

	for (const AssetDBRow_Mdl_v_0& MdlRowItem : _LoadedMdls)
	{
		IModelAssetMutable* NewMdl = _BoundAssetManager->CreateEmptyModelAsset(
			_BoundDBNameSpace, MdlRowItem.AssetName, MdlRowItem.AssetPath);
	}
}

void AssetDBLoader::ClearLoadedAssetData()
{
	_LoadedTextures.Clear();
	_LoadedMeshes.Clear();
	_LoadedDefaultMtls.Clear();
	_LoadedMdls.Clear();
	_LoadedMdlcs.Clear();
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
	_BoundAssetManager = nullptr;
	_BoundCommonRenderAssets = nullptr;
}

void AssetDBLoader::RelocateCreatedAssetInstancesToAssetManager()
{
	for (ITextureAsset* TexItem : _GeneratedTextures)
	{
		_BoundAssetManager->AddToAssetPool(TexItem);
	}
	_GeneratedTextures.Clear();

	for (IMaterialAsset* TexItem : _GeneratedMaterials)
	{
		_BoundAssetManager->AddToAssetPool(TexItem);
	}
	_GeneratedMaterials.Clear();
}

void AssetDBLoader::PushAssetsToSaveToDB(const SS::PooledList<IAssetBase*>& InAssets)
{
	for (IAssetBase* AssetItem : InAssets)
	{
		_AssetsToSaveToDB.PushBack(AssetItem);
	}
}

void AssetDBLoader::LoadAssetListFromAssetsToSaveToDB()
{
	for (const IAssetBase* AssetItem : _AssetsToSaveToDB)
	{
		EAssetType Type = AssetItem->GetAssetType();
		switch (Type)
		{
		case EAssetType::Mesh:
			_LoadedMeshes.PushBack(AssetToDBRow_Mesh_v_0(reinterpret_cast<const IMeshAsset*>(AssetItem))); break;
		case EAssetType::Material:
			_LoadedDefaultMtls.PushBack(AssetToDBRow_Mtl_DefaultPBR_v_0(reinterpret_cast<const IMaterialAsset*>(AssetItem))); break;
		case EAssetType::Model:
			_LoadedMdls.PushBack(AssetToDBRow_Mdl_v_0(reinterpret_cast<const IModelAsset*>(AssetItem))); break;
		case EAssetType::ModelCombination:
			_LoadedMdlcs.PushBack(AssetToDBRow_Mdlc_v_0(reinterpret_cast<const IModelCombinationAsset*>(AssetItem))); break;
		case EAssetType::Texture:
			_LoadedTextures.PushBack(AssetToDBRow_Tex_v_0(reinterpret_cast<const ITextureAsset*>(AssetItem))); break;
		}
	}
}

void AssetDBLoader::ClearAssetsToSaveToDB()
{
	_AssetsToSaveToDB.Clear();
}

bool AssetDBLoader::SaveLoadedAssetsToDB()
{
	bool bResult = SaveAllLoadedMeshesToDB();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = SaveAllLoadedMdlsToDB();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = SaveAllLoadedMdlcsToDB();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	return true;
}



bool AssetDBLoader::LoadAllLoadedTex()
{
	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;


	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		ALL_TEXTURE_QUERY,
		sizeof(ALL_TEXTURE_QUERY),
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

		AssetDBRow_Tex_v_0 NewColumn;
		NewColumn.AssetName = AssetNameStr.C_Str();
		NewColumn.AssetPath = AssetPathStr.C_Str();
		NewColumn.LastUpdateTime = UpdateTime;
		NewColumn.TextureType = TexType;

		_LoadedTextures.PushBack(NewColumn);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::LoadAllLoadedMtl()
{
	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;


	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		ALL_MTL_QUERY,
		sizeof(ALL_MTL_QUERY),
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
		AssetDBRow_Mtl_DefaultPBR_v_0 NewColumn;


		SS::StringW AssetNameStr = _BoundDBNameSpace.C_Str();
		AssetNameStr += L"/";
		SS::StringW AssetPathStr = _BoundFilePath.C_Str();
		AssetPathStr += L"/";

		const utf16* db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 1);
		if (db_c_str != nullptr)
		{
			AssetPathStr += db_c_str;
		}

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

bool AssetDBLoader::LoadAllLoadedMdls()
{
	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;


	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		ALL_MDL_QUERY,
		sizeof(ALL_MDL_QUERY),
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
		AssetDBRow_Mdl_v_0 NewColumn;


		SS::StringW AssetNameStr = _BoundDBNameSpace.C_Str();
		AssetNameStr += L"/";
		SS::StringW AssetPathStr = _BoundFilePath.C_Str();
		AssetPathStr += L"/";

		const utf16* db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 1);
		if (db_c_str != nullptr)
		{
			AssetPathStr += db_c_str;
		}

		time_t UpdateTime = sqlite3_column_int64(StmtResult, 2);

		NewColumn.SubMeshCnt = sqlite3_column_int(StmtResult, 3);


		NewColumn.AssetName = AssetNameStr.C_Str();
		NewColumn.AssetPath = AssetPathStr.C_Str();
		NewColumn.LastUpdateTime = UpdateTime;

		SS_ASSERT_MSG(false, L"여기서 계속하기 -> MeshName이 비어있으면 안되게 만들기");
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 4);
		if (db_c_str != nullptr)
		{
			NewColumn.MeshName = db_c_str;
		}



		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 5);
		int32 SubmeshIdx = 0;
		const utf16* StringIndexer = db_c_str;

		if (StringIndexer != nullptr)
		{
			_StringWorkTable.Clear();
			utf16 ThisChar = L'\0';
			do
			{
				ThisChar = *StringIndexer;
				StringIndexer++;

				if (ThisChar == L';')
				{
					_StringWorkTable.PushBack(L'\0');
					NewColumn.MtlNames[SubmeshIdx++] = _StringWorkTable.GetData();
					_StringWorkTable.Clear();
				}

				_StringWorkTable.PushBack(ThisChar);

			} while (ThisChar != '\0');
		}


		SS_ASSERT(NewColumn.SubMeshCnt == SubmeshIdx);
		NewColumn.SubMeshCnt = SubmeshIdx;
		_LoadedMdls.PushBack(NewColumn);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::LoadAllLoadedMeshes()
{
	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;

	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		ALL_MESH_QUERY,
		sizeof(ALL_MESH_QUERY),
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
		AssetDBRow_Mesh_v_0 NewRow;


		SS::StringW AssetNameStr = _BoundDBNameSpace.C_Str();
		AssetNameStr += L"/";
		SS::StringW AssetPathStr = _BoundFilePath.C_Str();
		AssetPathStr += L"/";

		const utf16* db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 1);
		if (db_c_str != nullptr)
		{
			AssetPathStr += db_c_str;
		}

		time_t UpdateTime = sqlite3_column_int64(StmtResult, 2);


		NewRow.AssetName = AssetNameStr.C_Str();
		NewRow.AssetPath = AssetPathStr.C_Str();
		NewRow.LastUpdateTime = UpdateTime;


		_LoadedMeshes.PushBack(NewRow);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::LoadAllLoadedMdlcs()
{
	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;

	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		ALL_MDLC_QUERY,
		sizeof(ALL_MDLC_QUERY),
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
		AssetDBRow_Mdlc_v_0 NewRow;


		SS::StringW AssetNameStr = _BoundDBNameSpace.C_Str();
		AssetNameStr += L"/";
		SS::StringW AssetPathStr = _BoundFilePath.C_Str();
		AssetPathStr += L"/";

		const utf16* db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;

		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 1);
		if (db_c_str != nullptr)
		{
			AssetPathStr += db_c_str;
		}

		time_t UpdateTime = sqlite3_column_int64(StmtResult, 2);


		NewRow.AssetName = AssetNameStr.C_Str();
		NewRow.AssetPath = AssetPathStr.C_Str();
		NewRow.LastUpdateTime = UpdateTime;


		_LoadedMdlcs.PushBack(NewRow);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::SaveAllLoadedMeshesToDB()
{
	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;

	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		SAVE_Mesh_v_0_QUERY,
		sizeof(SAVE_Mesh_v_0_QUERY),
		SQLITE_OPEN_READWRITE,
		&StmtResult,
		&__Temp);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot compile stmt.");
		sqlite3_finalize(StmtResult);
		return false;
	}

	for (const AssetDBRow_Mesh_v_0& RowItem : _LoadedMeshes)
	{
		sqlite3_bind_text16(StmtResult, 1, RowItem.AssetName.C_Str(), -1, SQLITE_STATIC);
		sqlite3_bind_text16(StmtResult, 2, RowItem.AssetPath.C_Str(), -1, SQLITE_STATIC);
		sqlite3_bind_int64(StmtResult, 3, RowItem.LastUpdateTime);


		sqlite3_step(StmtResult);
		sqlite3_reset(StmtResult);
	}

	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::SaveAllLoadedMdlsToDB()
{
	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;

	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		SAVE_Mdl_v_0_QUERY,
		sizeof(SAVE_Mdl_v_0_QUERY),
		SQLITE_OPEN_READWRITE,
		&StmtResult,
		&__Temp);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot compile stmt.");
		sqlite3_finalize(StmtResult);
		return false;
	}

	SS::StringW JoinedMtlNames;

	for (const AssetDBRow_Mdl_v_0& RowItem : _LoadedMdls)
	{
		sqlite3_bind_text16(StmtResult, 1, RowItem.AssetName.C_Str(), -1, SQLITE_STATIC);
		sqlite3_bind_text16(StmtResult, 2, RowItem.AssetPath.C_Str(), -1, SQLITE_STATIC);
		sqlite3_bind_int64(StmtResult, 3, RowItem.LastUpdateTime);

		sqlite3_bind_int64(StmtResult, 4, RowItem.SubMeshCnt);


		sqlite3_bind_text16(StmtResult, 5, RowItem.MeshName.C_Str(), -1, SQLITE_STATIC);

		JoinedMtlNames.Clear();
		for (int32 i = 0; i < RowItem.SubMeshCnt; i++)
		{
			JoinedMtlNames += RowItem.MtlNames[i].C_Str();
			JoinedMtlNames += L";";
		}
		sqlite3_bind_text16(StmtResult, 6, JoinedMtlNames.C_Str(), -1, SQLITE_TRANSIENT);


		sqlite3_step(StmtResult);
		sqlite3_reset(StmtResult);
	}

	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::SaveAllLoadedMdlcsToDB()
{
	return true;
}
