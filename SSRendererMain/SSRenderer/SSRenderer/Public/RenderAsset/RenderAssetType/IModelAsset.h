#pragma once
#include "IAssetBase.h"
#include "SSGAL/Public/SSGALInlineSettings.h"


class IMaterialAsset;
class IMeshAsset;

class IModelAsset : public IAssetBase
{
protected:
	IMeshAsset* _MeshAsset = nullptr;

	IMaterialAsset* _MaterialAssets[SUBMESH_COUNT_MAX] = { nullptr, };


public:
	IMeshAsset* GetMeshAsset() const { return _MeshAsset; }
	IMaterialAsset* GetMaterialAssetName(int32 materialIdx) const { return _MaterialAssets[materialIdx]; }

	virtual int32 GetSubMeshCnt() const = 0;

	virtual void SetMesh(IMeshAsset* InMeshAsset) = 0;
	virtual void SetMaterial(IMaterialAsset* InMaterialAsset, int32 materialIdx) = 0;
};
