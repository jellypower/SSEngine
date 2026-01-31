#pragma once
#include "SSRenderer/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"


class MeshRawDataDefault;
struct MeshRawDataBase;


SSRENDERER_MODULE int AppendDataFromMeshAsset(
	SS::PooledList<byte>& Data,
	const MeshRawDataBase* MeshData);



SSRENDERER_MODULE int FillMeshAssetFromData(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int Offset = 0);

