#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"

class IMaterialAssetMutable : public IMaterialAsset
{
public:
	virtual void NotifyMtlDataModified() = 0;

public:
	MtlDataBase* GetMutableMtlData() const { return _MtlData; }

	void InjectRawDataXXX(MtlDataBase* InMtlData)
	{
		_MtlData = InMtlData;
		NotifyMtlDataModified();
	}
	void InjectGALMeshAsset(GALMaterialAssetWrapperBase* InGALMtlAsset) { _GALMaterialAsset = InGALMtlAsset; }
};
