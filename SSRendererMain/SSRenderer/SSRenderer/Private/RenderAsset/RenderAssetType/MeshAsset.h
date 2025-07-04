#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

struct MeshRawDataBase;
class GALMeshAssetWrapperBase;



class MeshAsset : public IMeshAsset 
{
public:
	MeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);

public:
	EAssetType GetAssetType() const override;

	void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	void ReleaseSystemData() override;
	void ReleaseGALData() override;
};

