#pragma once
#include "IAssetBase.h"
#include "MeshData/MeshRawDataBase.h"

struct MeshRawDataBase;
class GALMeshAssetWrapperBase;



class IMeshAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Mesh;

protected:
	GALMeshAssetWrapperBase* _GALMeshAsset = nullptr;


public:
	const GALMeshAssetWrapperBase* GetGALMeshAsset() const { return _GALMeshAsset; }

public:
	virtual const MeshRawDataBase* GetMeshRawData() const = 0;
	virtual EMeshType GetMeshType() const = 0;
	virtual int32 GetSubMeshCnt() const = 0;

	virtual void ReleaseSystemData() = 0;
	virtual void ReleaseGALData() = 0;
};

