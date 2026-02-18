#pragma once
#include "SSRenderer/ModuleExportKeyword.h"

#include "SSEngineDefault/Public/SHasher/SHasherW.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"


class IModelCombinationAssetMutable;
class IApakFileReader;
class IMeshAsset;
enum class EAssetType;
class IAssetBase;
class IModelCombinationAsset;
class MeshRawDataDefault;
struct MeshRawDataBase;




SSRENDERER_MODULE int32 AppendDataFromMeshAsset(
	SS::PooledList<byte>& Data,
	const MeshRawDataBase* MeshData);

SSRENDERER_MODULE int64 AppendDataFromMdlcAsset(
	SS::PooledList<byte>& Data,
	const IModelCombinationAsset* MdlcData);


SSRENDERER_MODULE int32 FillMeshAssetHeaaderOnly(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int Offset = 0);

SSRENDERER_MODULE int32 FillMeshRawDataFromData(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int Offset = 0);


SSRENDERER_MODULE int64 AppendApakDataFromAssetList(
	SS::PooledList<byte>& Data,
	const SS::PooledList<IAssetBase*>& AssetListToSerailize);



SSRENDERER_MODULE int64 CreateAssetsFromApakData(
	SS::PooledList<IAssetBase*>& CreatedAssetList,
	const SS::PooledList<byte>& Data,
	SS::SHasherW ApakAssetPath,
	SS::SHasherW AssetNamespace,
	int Offset = 0);

SSRENDERER_MODULE int32 CreateMeshAssetFromData(
	IMeshAsset*& OutMeshAsset,
	SS::SHasherW AssetName,
	SS::SHasherW AssetPath,
	SS::SHasherW AssetNamespace,
	const SS::PooledList<byte>& Data,
	int Offset = 0);

SSRENDERER_MODULE int64 FillEmptyMdlcAssetFromData(
	IModelCombinationAssetMutable* MdlcAssetToFill,
	const SS::PooledList<byte>& Data,
	int64 Offset = 0);

SSRENDERER_MODULE EAssetType ExtractAssetTypeFromName(SS::SHasherW InAssetName);

SSRENDERER_MODULE IApakFileReader* CreateApakFileAccessor(SS::SHasherW SystemPath, SS::SHasherW DBNameSpace);
SSRENDERER_MODULE IApakFileReader* CreateApakFileAccessorFromNameSpace(SS::SHasherW DBNameSpace, SS::SHasherW RelativePath);

