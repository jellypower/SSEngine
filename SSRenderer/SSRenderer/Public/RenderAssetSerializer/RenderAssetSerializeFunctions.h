#pragma once
#include "SSRenderer/ModuleExportKeyword.h"

#include "SSEngineDefault/Public/SHasher/SHasherW.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"


struct RenderAnimRawData;
class IModelCombinationAssetMutable;
class IApakFileReader;
class IMeshAsset;
enum class EAssetType;
class IAssetBase;
class IModelCombinationAsset;
class MeshRawDataDefault;
struct MeshRawDataBase;




SSRENDERER_MODULE int64 AppendDataFromMeshAsset(
	SS::PooledList<byte>& Data,
	const MeshRawDataBase* MeshData);

SSRENDERER_MODULE int64 AppendDataFromMdlcAsset(
	SS::PooledList<byte>& Data,
	const IModelCombinationAsset* MdlcData,
	SS::SHasherW MdlcChildNameSpaceReplaced,
	SS::SHasherW MdlcChildNameSpaceToReplace);

SSRENDERER_MODULE int64 AppendDataFromRenderAnim(
	SS::PooledList<byte>& Data,
	const RenderAnimRawData* AnimData);


SSRENDERER_MODULE int64 FillMeshAssetHeaaderOnly(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int64 Offset = 0);

SSRENDERER_MODULE int64 FillMeshRawDataFromData(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int64 Offset = 0);

SSRENDERER_MODULE int64 FillRenderAnimFromData(
	RenderAnimRawData*& OutAnimData,
	const SS::PooledList<byte>& Data,
	int64 Offset = 0);



SSRENDERER_MODULE int64 AppendApakDataFromAssetList(
	SS::PooledList<byte>& Data,
	const SS::PooledList<IAssetBase*>& AssetListToSerailize,
	SS::SHasherW MdlcChildNameSpaceReplaced,
	SS::SHasherW MdlcChildNameSpaceToReplace);



SSRENDERER_MODULE int64 CreateAssetsFromApakData(
	SS::PooledList<IAssetBase*>& CreatedAssetList,
	const SS::PooledList<byte>& Data,
	SS::SHasherW ApakAssetPath,
	SS::SHasherW AssetNamespace,
	time_t LastUpdateTime,
	int Offset = 0);

SSRENDERER_MODULE int64 CreateMeshAssetFromData(
	IMeshAsset*& OutMeshAsset,
	SS::SHasherW AssetName,
	SS::SHasherW AssetPath,
	SS::SHasherW AssetNamespace,
	time_t LastUpdateTime,
	const SS::PooledList<byte>& Data,
	int Offset = 0);

SSRENDERER_MODULE int64 FillEmptyMdlcAssetFromData(
	IModelCombinationAssetMutable* MdlcAssetToFill,
	const SS::PooledList<byte>& Data,
	int64 Offset = 0);

SSRENDERER_MODULE EAssetType ExtractAssetTypeFromName(SS::SHasherW InAssetName);

SSRENDERER_MODULE IApakFileReader* CreateApakFileAccessor(SS::SHasherW SystemPath, SS::SHasherW DBNameSpace);
SSRENDERER_MODULE IApakFileReader* CreateApakFileAccessorFromNameSpace(SS::SHasherW DBNameSpace, SS::SHasherW RelativePath);

