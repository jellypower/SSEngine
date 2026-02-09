#pragma once
#include "SSRenderer/ModuleExportKeyword.h"

#include "SSEngineDefault/Public/SHasher/SHasherW.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"


class IMeshAsset;
enum class EAssetType;
class IAssetBase;
class IModelCombinationAsset;
class MeshRawDataDefault;
struct MeshRawDataBase;


SSRENDERER_MODULE int32 AppendDataFromMeshAsset(
	SS::PooledList<byte>& Data,
	const MeshRawDataBase* MeshData);

SSRENDERER_MODULE int32 FillMeshAssetFromData(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int Offset = 0);


SSRENDERER_MODULE int32 AppendApakDataFromAssetList(
	SS::PooledList<byte>& Data,
	const SS::PooledList<IAssetBase*>& AssetListToSerailize);

SSRENDERER_MODULE int32 CreateAssetsFromApakData(
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

SSRENDERER_MODULE EAssetType ExtractAssetTypeFromName(SS::SHasherW InAssetName);