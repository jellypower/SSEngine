#pragma once
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"

struct MeshRawDataBase;
class GALMeshAssetWrapperBase;



class MeshAsset : public IMeshAssetMutable
{
public:
	MeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);

public:
	virtual void InjectRawDataXXX(MeshRawDataBase* InRawData) override;

	virtual const MeshRawDataBase* GetMeshRawData() const override;
	virtual EMeshType GetMeshType() const override;
	virtual int32 GetSubMeshCnt() const override;

	virtual EAssetType GetAssetType() const override;

	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;
	virtual void ReleaseSystemData() override;
	virtual void ReleaseGALData() override;

private:
	const MeshRawDataBase* _MeshRawData = nullptr;

	EMeshType _CachedMeshType = EMeshType::None;
};

