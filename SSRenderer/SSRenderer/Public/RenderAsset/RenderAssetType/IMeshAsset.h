#pragma once
#pragma once
#include "IAssetBase.h"

struct MeshRawDataBase;
class GALMeshAssetWrapperBase;



class IMeshAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Mesh;

protected:
	const MeshRawDataBase* _MeshRawData = nullptr;
	GALMeshAssetWrapperBase* _GALMeshAsset = nullptr;


public:
	const MeshRawDataBase* GetMeshRawData() const { return _MeshRawData; }
	const GALMeshAssetWrapperBase* GetGALMeshAsset() const { return _GALMeshAsset; }

public:
	virtual int32 GetSubMeshCnt() const = 0;

	virtual void ReleaseSystemData() = 0;
	virtual void ReleaseGALData() = 0;
};

