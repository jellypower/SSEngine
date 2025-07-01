#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelAsset.h"

ModelAsset::ModelAsset(SS::SHasherW inAssetName, SS::SHasherW inAssetPath)
	: SSAssetBase(EAssetType::Model, inAssetName, inAssetPath)
{
}

ModelAsset::ModelAsset()
	: SSAssetBase(EAssetType::Model)
{
}

void ModelAsset::SetMesh(SS::SHasherW inMeshAssetName)
{
	_meshAssetName = inMeshAssetName;
}

void ModelAsset::SetMaterial(SS::SHasherW inMaterialAssetName, int32 materialIdx)
{
	if (materialIdx > SUBMESH_COUNT_MAX)
	{
		DEBUG_BREAK();
		return;
	}

	if (_submeshCnt < materialIdx + 1)
	{
		_submeshCnt = materialIdx + 1;
	}

	_materialAssetNames[materialIdx] = inMaterialAssetName;
}
