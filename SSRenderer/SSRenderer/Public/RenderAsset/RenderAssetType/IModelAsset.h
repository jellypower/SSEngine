#pragma once
#include "IAssetBase.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"


class IMaterialAsset;
class IMeshAsset;

class IModelAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Model;

public:
	virtual IMeshAsset* GetMeshAsset() const = 0;
	virtual IMaterialAsset* GetMaterialAsset(int32 materialIdx) const = 0;
	virtual SS::SHasherW GetMeshAssetName() const = 0;
	virtual SS::SHasherW GetMaterialAssetName(int32 materialIdx) const = 0;
	virtual int32 GetSubMeshCnt() const = 0;
};
