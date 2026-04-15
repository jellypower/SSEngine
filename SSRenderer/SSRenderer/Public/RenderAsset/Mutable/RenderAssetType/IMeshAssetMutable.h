#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

class IMeshAssetMutable : public IMeshAsset
{
public:
	virtual void InjectRawDataXXX(MeshRawDataBase* InRawData) = 0;
	void InjectGALMeshAsset(GALMeshAssetWrapperBase* InGALMeshAsset) { _GALMeshAsset = InGALMeshAsset; }
};