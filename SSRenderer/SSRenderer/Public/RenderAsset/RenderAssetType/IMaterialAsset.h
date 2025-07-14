#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"

struct MtlDataBase;
class GALMaterialAssetWrapperBase;




class IMaterialAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Material;

protected:
	MtlDataBase* _MtlData = nullptr;
	GALMaterialAssetWrapperBase* _GALMaterialAsset = nullptr;

public:
	const MtlDataBase* GetMtlData() const { return _MtlData; }
	const GALMaterialAssetWrapperBase* GetGALMaterialAsset() const { return _GALMaterialAsset; }

	virtual void ReleaseGALData() = 0;
};
