#pragma once
#include "IAssetBase.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"


class IMaterialAsset;
class IMeshAsset;

class IModelAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Model;

protected:
	IMeshAsset* _MeshAsset = nullptr;
	IMaterialAsset* _MaterialAssets[SUBMESH_COUNT_MAX] = { nullptr, };


public:
	IMeshAsset* GetMeshAsset() const { return _MeshAsset; }
	IMaterialAsset* GetMaterialAsset(int32 materialIdx) const { return _MaterialAssets[materialIdx]; }

	virtual int32 GetSubMeshCnt() const = 0;
};
