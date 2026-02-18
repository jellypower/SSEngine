#include "pch.h"

#include "AssetDBLoader.h"

#include "SSAssetDBManager/Private/DBColumnTypes/DBRowUtils.h"

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"


#include "SSRenderer/Public/RenderAssetSerializer/IApakFileReader.h"
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"

#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelCombinationAssetMutable.h"



#include "DBQueries.h"

#include <algorithm>


AssetDBLoader::AssetDBLoader()
{
	_DBInterTextures.Reserve(128);
	_DBInterMeshes.Reserve(128);
	_DBInterDefaultMtls.Reserve(128);
	_DBInterMdls.Reserve(128);
	_DBInterMdlcs.Reserve(128);
}

bool AssetDBLoader::StartLoadDB(SS::SHasherW InNameSpace)
{
	if (_BoundDBNameSpace.IsEmpty() == false)
	{
		SS_ASSERT(false);
		return false;
	}

	_BoundDBNameSpace = InNameSpace;

	SS::StringW FilePathConstructor = L"Resource/AssetDB/";
	FilePathConstructor += InNameSpace.C_Str();
	FilePathConstructor += L"/";
	_BoundDBNameSpacePath = FilePathConstructor.C_Str();

	FilePathConstructor += InNameSpace.C_Str();
	FilePathConstructor += L".sqlite";
	_BoundDBSqlFilePath = FilePathConstructor.C_Str();


	const int Result = sqlite3_open16(_BoundDBSqlFilePath.C_Str(), &_hLoadedDB);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot open file.");

		_BoundDBNameSpacePath = SS::SHasherW();
		_BoundDBSqlFilePath = SS::SHasherW();
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

	ClearInterData();

	_BoundDBNameSpacePath = SS::SHasherW();
	_BoundDBSqlFilePath = SS::SHasherW();
	_BoundDBNameSpace = SS::SHasherW();
	_bIsEngineDefaultAssetDB = false;
}

bool AssetDBLoader::LoadAllAssetDataFromDB()
{
	bool bResult = LoadDBInterAllTex();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = LoadDBInterAllMeshes();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = LoadDBInterAllMtl();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = LoadDBInterAllMdls();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = LoadDBInterAllMdlcs();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	return true;
}


bool SortByAssetPath(const IAssetBase* lhs, const IAssetBase* rhs)
{
	return lhs->GetAssetPath().GetDirectValue() < rhs->GetAssetPath().GetDirectValue();
}

void AssetDBLoader::CreateAssetInstancesFromInter()
{
	for (const AssetDBRow_Tex_v_0& TexRowItem : _DBInterTextures)
	{
		
		ITextureAssetMutable* NewTex = _BoundAssetManager->CreateEmptyTextureAsset(_BoundDBNameSpace,
			TexRowItem.AssetName, TexRowItem.AssetPath, TexRowItem.TextureType);

		_AllAssetInstancesSortedByPath.PushBack(NewTex);
		_CreatedTextures.PushBack(NewTex);
	}

	for (const AssetDBRow_Mtl_DefaultPBR_v_0& DefaultMtlRowItem : _DBInterDefaultMtls)
	{
		IMaterialAssetMutable* NewMtl = _BoundAssetManager->CreateEmptyMaterialAsset(_BoundDBNameSpace,
			DefaultMtlRowItem.AssetName, DefaultMtlRowItem.AssetPath);

		MtlDataDefaultPBR* NewDefaultPBRMtlData = DBG_NEW MtlDataDefaultPBR;
		NewDefaultPBRMtlData->_BaseColorScale = DefaultMtlRowItem._BaseColorScale;
		NewDefaultPBRMtlData->_EmissiveScale = DefaultMtlRowItem._EmissiveScale;
		NewDefaultPBRMtlData->_NormalTexScale = DefaultMtlRowItem.NormalTexScale;
		NewDefaultPBRMtlData->_Metallic = DefaultMtlRowItem.Metallic;
		NewDefaultPBRMtlData->_Roughness = DefaultMtlRowItem.Roughness;
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::BaseColor] = DefaultMtlRowItem.Textures[(int32)EDefaultPBRMatTexTypes::BaseColor];
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Normal] = DefaultMtlRowItem.Textures[(int32)EDefaultPBRMatTexTypes::Normal];
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Metallic] = DefaultMtlRowItem.Textures[(int32)EDefaultPBRMatTexTypes::Metallic];
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Emissive] = DefaultMtlRowItem.Textures[(int32)EDefaultPBRMatTexTypes::Emissive];
		NewDefaultPBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Occlusion] = DefaultMtlRowItem.Textures[(int32)EDefaultPBRMatTexTypes::Occlusion];

		NewMtl->InjectRawDataXXX(NewDefaultPBRMtlData);

		_AllAssetInstancesSortedByPath.PushBack(NewMtl);
		_CreatedMaterials.PushBack(NewMtl);
	}

	for (const AssetDBRow_Mesh_v_0& MeshRowItem : _DBInterMeshes)
	{
		IMeshAssetMutable* NewAsset = _BoundAssetManager->CreateEmptyMeshAsset(_BoundDBNameSpace,
			MeshRowItem.AssetName, MeshRowItem.AssetPath);

		_AllAssetInstancesSortedByPath.PushBack(NewAsset);
		_CreatedMeshes.PushBack(NewAsset);
	}

	for (const AssetDBRow_Mdl_v_0& MdlRowItem : _DBInterMdls)
	{
		IModelAssetMutable* NewMdl = _BoundAssetManager->CreateEmptyModelAsset(
			_BoundDBNameSpace, MdlRowItem.AssetName, MdlRowItem.AssetPath);

		NewMdl->SetMesh(MdlRowItem.MeshName);

		for (int32 i = 0; i < SUBMESH_COUNT_MAX; i++)
		{
			SS::SHasherW MtlName = MdlRowItem.MtlNames[i];
			if (MtlName.IsEmpty())
			{
				break;
			}

			NewMdl->SetMaterial(MtlName, i);
		}

		_AllAssetInstancesSortedByPath.PushBack(NewMdl);
		_CreatedMdls.PushBack(NewMdl);
	}

	for (const AssetDBRow_Mdlc_v_0& MdlcRowItem : _DBInterMdlcs)
	{
		IModelCombinationAssetMutable* NewAsset = _BoundAssetManager->CreateEmptyModelCombinationAsset(_BoundDBNameSpace,
			MdlcRowItem.AssetName, MdlcRowItem.AssetPath, 0);

		_AllAssetInstancesSortedByPath.PushBack(NewAsset);
		_CreatedMdlcs.PushBack(NewAsset);
	}

	int32 AllAssetCnt = _AllAssetInstancesSortedByPath.GetSize();
	IAssetBase** AllAssetDataRaw = _AllAssetInstancesSortedByPath.GetData();
	std::sort(AllAssetDataRaw, AllAssetDataRaw + AllAssetCnt, SortByAssetPath);

	FillEmptyAssetsFromApakFile();
}

void AssetDBLoader::ClearInterData()
{
	_DBInterTextures.Clear();
	_DBInterMeshes.Clear();
	_DBInterDefaultMtls.Clear();
	_DBInterMdls.Clear();
	_DBInterMdlcs.Clear();
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
	for (ITextureAsset* TexAssetItem : _CreatedTextures)
	{
		_BoundAssetManager->AddToAssetPool(TexAssetItem);
	}

	for (IMeshAsset* MeshAssetItem : _CreatedMeshes)
	{
		_BoundAssetManager->AddToAssetPool(MeshAssetItem);
	}

	for (IMaterialAsset* MtlAssetItem : _CreatedMaterials)
	{
		_BoundAssetManager->AddToAssetPool(MtlAssetItem);
	}

	for (IModelAsset* MdlAssetItem : _CreatedMdls)
	{
		_BoundAssetManager->AddToAssetPool(MdlAssetItem);
	}

	for (IModelCombinationAsset* MdlcAssetITem : _CreatedMdlcs)
	{
		_BoundAssetManager->AddToAssetPool(MdlcAssetITem);
	}

	_CreatedTextures.Clear();
	_CreatedMeshes.Clear();
	_CreatedMaterials.Clear();
	_CreatedMdls.Clear();
	_CreatedMdlcs.Clear();
	_AllAssetInstancesSortedByPath.Clear();
}

void AssetDBLoader::PushAssetsToSaveToDB(const SS::PooledList<IAssetBase*>& InAssets)
{
	for (IAssetBase* AssetItem : InAssets)
	{
		AssetItem->MarkAsUpdated();
		_AssetsToSaveToDB.PushBack(AssetItem);
	}
}

void AssetDBLoader::CreateInterListFromAssetsToSaveToDB()
{
	for (const IAssetBase* AssetItem : _AssetsToSaveToDB)
	{
		EAssetType Type = AssetItem->GetAssetType();
		switch (Type)
		{
		case EAssetType::Mesh:
			_DBInterMeshes.PushBack(AssetToDBRow_Mesh_v_0(reinterpret_cast<const IMeshAsset*>(AssetItem))); break;
		case EAssetType::Material:
			_DBInterDefaultMtls.PushBack(AssetToDBRow_Mtl_DefaultPBR_v_0(reinterpret_cast<const IMaterialAsset*>(AssetItem))); break;
		case EAssetType::Model:
			_DBInterMdls.PushBack(AssetToDBRow_Mdl_v_0(reinterpret_cast<const IModelAsset*>(AssetItem))); break;
		case EAssetType::ModelCombination:
			_DBInterMdlcs.PushBack(AssetToDBRow_Mdlc_v_0(reinterpret_cast<const IModelCombinationAsset*>(AssetItem))); break;
		case EAssetType::Texture:
			_DBInterTextures.PushBack(AssetToDBRow_Tex_v_0(reinterpret_cast<const ITextureAsset*>(AssetItem))); break;
		}
	}
}

void AssetDBLoader::ClearAssetsToSaveToDB()
{
	_AssetsToSaveToDB.Clear();
}

bool AssetDBLoader::SaveInterAssetsToDB()
{
	bool bResult = SaveAllInterMeshesToDB();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = SaveAllInterMtlsToDB();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = SaveAllInterMdlsToDB();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	bResult = SaveAllInterMdlcsToDB();
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	return true;
}

void AssetDBLoader::FillEmptyAssetsFromApakFile()
{
	IApakFileReader* ApakFileAccessor = nullptr;
	for (IAssetBase* AssetItem : _AllAssetInstancesSortedByPath)
	{
		const EAssetType AssetTypeItem = AssetItem->GetAssetType();
		if (
			AssetTypeItem == EAssetType::Material ||
			AssetTypeItem == EAssetType::Model ||
			AssetTypeItem == EAssetType::Texture
			)
		{
			continue;
		}


		SS::SHasherW PathItem = AssetItem->GetAssetPath();

		if (PathItem.IsEmpty())
		{
			continue;
		}

		if (ApakFileAccessor == nullptr ||
			ApakFileAccessor->GetFilePath() != PathItem)
		{

			if (ApakFileAccessor != nullptr)
			{
				delete ApakFileAccessor;
			}

			ApakFileAccessor = CreateApakFileAccessor(PathItem, _BoundDBNameSpace);
		}

		bool bResult = ApakFileAccessor->SetDataCursorToAsset(AssetItem->GetAssetName());
		if (bResult == false)
		{
			SS_ASSERT(false);
			continue;
		}


		if (AssetTypeItem == EAssetType::Mesh)
		{
			IMeshAssetMutable* MeshAsset = static_cast<IMeshAssetMutable*>(AssetItem);

			MeshRawDataDefault* MeshRawData = nullptr;
			FillMeshRawDataFromData(MeshRawData, ApakFileAccessor->GetCursoredData());

			MeshAsset->InjectRawDataXXX(MeshRawData);
		}
		else if (AssetTypeItem == EAssetType::ModelCombination)
		{
			IModelCombinationAssetMutable* MdlcAsset = static_cast<IModelCombinationAssetMutable*>(AssetItem);
			FillEmptyMdlcAssetFromData(MdlcAsset, ApakFileAccessor->GetCursoredData());
		}
		else
		{
			SS_ASSERT_MSG(false, L"TODO: Impl");
		}
	}

	if (ApakFileAccessor != nullptr)
	{
		delete ApakFileAccessor;
	}
}

bool AssetDBLoader::LoadDBInterAllTex()
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

		SS::StringW AssetPathStr = _BoundDBNameSpacePath.C_Str();
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 1);
		SS_ASSERT(db_c_str);
		AssetPathStr += db_c_str;
		

		time_t UpdateTime = sqlite3_column_int64(StmtResult, 2);

		ETextureType TexType = (ETextureType)sqlite3_column_int(StmtResult, 3);
		SS_ASSERT(ETextureType::None <= TexType && TexType < ETextureType::Count);

		AssetDBRow_Tex_v_0 NewRow;
		NewRow.AssetName = AssetNameStr.C_Str();
		NewRow.AssetPath = AssetPathStr.C_Str();
		NewRow.LastUpdateTime = UpdateTime;
		NewRow.TextureType = TexType;

		_DBInterTextures.PushBack(NewRow);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::LoadDBInterAllMtl()
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
		AssetDBRow_Mtl_DefaultPBR_v_0 NewRow;


		SS::StringW AssetNameStr = _BoundDBNameSpace.C_Str();
		AssetNameStr += L"/";

		const utf16* db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;

		time_t UpdateTime = sqlite3_column_int64(StmtResult, 2);


		NewRow.AssetName = AssetNameStr.C_Str();
		NewRow.LastUpdateTime = UpdateTime;


		NewRow._BaseColorScale.X = sqlite3_column_double(StmtResult, 3);
		NewRow._BaseColorScale.Y = sqlite3_column_double(StmtResult, 4);
		NewRow._BaseColorScale.Z = sqlite3_column_double(StmtResult, 5);
		NewRow._BaseColorScale.W = 1;

		NewRow._EmissiveScale.X = sqlite3_column_double(StmtResult, 6);
		NewRow._EmissiveScale.Y = sqlite3_column_double(StmtResult, 7);
		NewRow._EmissiveScale.Z = sqlite3_column_double(StmtResult, 8);
		NewRow._EmissiveScale.W = 1;

		NewRow.NormalTexScale = sqlite3_column_double(StmtResult, 9);
		NewRow.Metallic = sqlite3_column_double(StmtResult, 10);
		NewRow.Roughness = sqlite3_column_double(StmtResult, 11);


		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 12);
		NewRow.Textures[(int32)EDefaultPBRMatTexTypes::BaseColor] = db_c_str;
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 13);
		NewRow.Textures[(int32)EDefaultPBRMatTexTypes::Normal] = db_c_str;
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 14);
		NewRow.Textures[(int32)EDefaultPBRMatTexTypes::Metallic] = db_c_str;
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 15);
		NewRow.Textures[(int32)EDefaultPBRMatTexTypes::Emissive] = db_c_str;
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 16);
		NewRow.Textures[(int32)EDefaultPBRMatTexTypes::Occlusion] = db_c_str;

		_DBInterDefaultMtls.PushBack(NewRow);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::LoadDBInterAllMdls()
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
		AssetDBRow_Mdl_v_0 NewRow;


		SS::StringW AssetNameStr = _BoundDBNameSpace.C_Str();
		AssetNameStr += L"/";
		const utf16* db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 0);
		AssetNameStr += db_c_str;



		time_t UpdateTime = sqlite3_column_int64(StmtResult, 2);

		NewRow.AssetName = AssetNameStr.C_Str();
		NewRow.LastUpdateTime = UpdateTime;


		//		SS_ASSERT_MSG(false, L"여기서 계속하기 -> MeshName이 비어있으면 안되게 만들기");
		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 3);
		if (db_c_str != nullptr)
		{
			NewRow.MeshName = db_c_str;
		}



		db_c_str = (const utf16*)sqlite3_column_text16(StmtResult, 4);
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
					NewRow.MtlNames[SubmeshIdx++] = _StringWorkTable.GetData();
					_StringWorkTable.Clear();
				}
				else
				{
					_StringWorkTable.PushBack(ThisChar);
				}

			} while (ThisChar != '\0');
		}

		_DBInterMdls.PushBack(NewRow);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::LoadDBInterAllMeshes()
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
		SS::StringW AssetPathStr = _BoundDBNameSpacePath.C_Str();


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


		_DBInterMeshes.PushBack(NewRow);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::LoadDBInterAllMdlcs()
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
		SS::StringW AssetPathStr = _BoundDBNameSpacePath.C_Str();

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


		_DBInterMdlcs.PushBack(NewRow);
	}


	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::SaveAllInterMeshesToDB()
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

	for (const AssetDBRow_Mesh_v_0& RowItem : _DBInterMeshes)
	{
		const utf16* NameSpacePathCutoff = CutOffNameSpacePath(RowItem.AssetPath, _BoundDBNameSpacePath);
		if (NameSpacePathCutoff == nullptr)
		{
			SS_ASSERT_MSG(false, L"Not a valid namespace path. If you want to save Asset path to a namespace, asset original file path must be located in same asset path directory.");
			continue;
		}

		sqlite3_bind_text16(StmtResult, 1, RowItem.AssetName.C_Str(), -1, SQLITE_STATIC);

		sqlite3_bind_text16(StmtResult, 2, NameSpacePathCutoff, -1, SQLITE_STATIC);
		sqlite3_bind_int64(StmtResult, 3, RowItem.LastUpdateTime);


		sqlite3_step(StmtResult);
		sqlite3_reset(StmtResult);
	}

	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::SaveAllInterMtlsToDB()
{
	sqlite3_stmt* StmtResult = nullptr;
	const void* __Temp = nullptr;


	int Result = sqlite3_prepare16_v3(
		_hLoadedDB,
		SAVE_DefaultPBR_Mtl_v_0_QUERY,
		sizeof(SAVE_DefaultPBR_Mtl_v_0_QUERY),
		SQLITE_OPEN_READONLY,
		&StmtResult,
		&__Temp);
	if (Result)
	{
		SS_ASSERT_MSG(false, L"Cannot compile stmt.");
		sqlite3_finalize(StmtResult);
		return false;
	}

	for (const AssetDBRow_Mtl_DefaultPBR_v_0& RowItem : _DBInterDefaultMtls)
	{
//		L"AssetName, AssetPath, LastUpdateTime, "
//		L"BaseColor_r, BaseColor_g, BaseColor_b, "
//		L"Emissive_r, Emissive_g, Emissive_b, "
//		L"NormalTexScale, Metallic, Roughness, "
//		L"BaseColor_Tex_ID, Normal_Tex_ID, Metallic_Tex_ID, Emissive_Tex_ID, Occlusion_Tex_ID) "

		sqlite3_bind_text16(StmtResult, 1, RowItem.AssetName.C_Str(), -1, SQLITE_STATIC);
		sqlite3_bind_text16(StmtResult, 2, nullptr, -1, SQLITE_STATIC); // No Path to DefaultPBRMtls
		sqlite3_bind_int64(StmtResult, 3, RowItem.LastUpdateTime);

		sqlite3_bind_double(StmtResult, 4, RowItem._BaseColorScale.X); // BaseColor
		sqlite3_bind_double(StmtResult, 5, RowItem._BaseColorScale.Y);
		sqlite3_bind_double(StmtResult, 6, RowItem._BaseColorScale.Z);

		sqlite3_bind_double(StmtResult, 7, RowItem._EmissiveScale.X); // Emussuve
		sqlite3_bind_double(StmtResult, 8, RowItem._EmissiveScale.Y);
		sqlite3_bind_double(StmtResult, 9, RowItem._EmissiveScale.Z);

		sqlite3_bind_double(StmtResult, 10, RowItem.NormalTexScale); // NormalTexScale
		sqlite3_bind_double(StmtResult, 11, RowItem.Metallic); // Metallic
		sqlite3_bind_double(StmtResult, 12, RowItem.Roughness); // Roughness

		for (int32 i = 0; i < (int32)EDefaultPBRMatTexTypes::Count; i++)
		{
			sqlite3_bind_text16(StmtResult, 13 + i, RowItem.Textures[i].C_Str(), -1, SQLITE_STATIC);
		}


		sqlite3_step(StmtResult);
		sqlite3_reset(StmtResult);
	}

	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::SaveAllInterMdlsToDB()
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

	for (const AssetDBRow_Mdl_v_0& RowItem : _DBInterMdls)
	{
		sqlite3_bind_text16(StmtResult, 1, RowItem.AssetName.C_Str(), -1, SQLITE_STATIC);
		sqlite3_bind_text16(StmtResult, 2, nullptr, -1, SQLITE_STATIC);
		sqlite3_bind_int64(StmtResult, 3, RowItem.LastUpdateTime);
		sqlite3_bind_text16(StmtResult, 4, RowItem.MeshName.C_Str(), -1, SQLITE_STATIC);

		JoinedMtlNames.Clear();
		for (int32 i = 0; i < SUBMESH_COUNT_MAX; i++)
		{
			SS::SHasherW MtlNameItem = RowItem.MtlNames[i];
			if (MtlNameItem.IsEmpty())
			{
				break;
			}

			JoinedMtlNames += RowItem.MtlNames[i].C_Str();
			JoinedMtlNames += L";";
		}
		sqlite3_bind_text16(StmtResult, 5, JoinedMtlNames.C_Str(), -1, SQLITE_TRANSIENT);


		sqlite3_step(StmtResult);
		sqlite3_reset(StmtResult);
	}

	sqlite3_finalize(StmtResult);
	return true;
}

bool AssetDBLoader::SaveAllInterMdlcsToDB()
{
	return true;
}
