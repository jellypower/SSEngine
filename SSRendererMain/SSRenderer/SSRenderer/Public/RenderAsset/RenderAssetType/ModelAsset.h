#pragma once
#include "SSAssetBase.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

class ModelAsset : public SSAssetBase
{
	friend class SSFBXImporter;
private:
	SS::SHasherW _meshAssetName;

	int32 _submeshCnt = 0;
	SS::SHasherW  _materialAssetNames[SUBMESH_COUNT_MAX];

public:
	ModelAsset(SS::SHasherW inAssetName, SS::SHasherW inAssetPath);
	ModelAsset();

public:
	SS::SHasherW GetMeshAssetName() const { return _meshAssetName; }
	SS::SHasherW GetMaterialAssetName(int32 materialIdx) const { return _materialAssetNames[materialIdx]; }
	int32 GetSubMeshCnt() const { return _submeshCnt; }


	void SetMesh(SS::SHasherW inMeshAssetName);
	void SetMaterial(SS::SHasherW inMaterialAssetName, int32 materialIdx);
};
