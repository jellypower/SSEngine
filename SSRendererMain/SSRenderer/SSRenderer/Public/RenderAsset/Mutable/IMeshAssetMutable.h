#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

class IMeshAssetMutable : public IMeshAsset
{
public:
	void InjectRawDataXXX(MeshRawDataBase* InRawData) { _MeshRawData = InRawData; }
	void InjectGALMeshAsset(GALMeshAssetWrapperBase* InGALMeshAsset) { _GALMeshAsset = InGALMeshAsset; }
};