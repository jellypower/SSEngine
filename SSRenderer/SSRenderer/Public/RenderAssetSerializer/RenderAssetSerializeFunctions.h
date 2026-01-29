#pragma once
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

#include "SSRenderer/ModuleExportKeyword.h"

struct MeshRawDataBase;

SSRENDERER_MODULE_NATIVE int FillDataFromMeshAsset(
	SS::PooledList<byte>& Data,
	const MeshRawDataBase* MeshData);

SSRENDERER_MODULE_NATIVE int FillMeshAssetFromData(
	MeshRawDataBase* MeshData,
	const SS::PooledList<byte>& Data,
	int Offset = 0);