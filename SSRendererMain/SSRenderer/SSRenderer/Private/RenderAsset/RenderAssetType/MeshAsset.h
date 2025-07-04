#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/IMeshAssetMutable.h"

struct MeshRawDataBase;
class GALMeshAssetWrapperBase;



class MeshAsset : public IMeshAssetMutable
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

