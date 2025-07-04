#pragma once
#pragma once
#include "IAssetBase.h"

struct MeshRawDataBase;
class GALMeshAssetWrapperBase;



class IMeshAsset : public IAssetBase
{
public:
	GALMeshAssetWrapperBase* _GALMeshAsset = nullptr;

protected:
	const MeshRawDataBase* _MeshRawData = nullptr;


public:
	const MeshRawDataBase* GetMeshRawData() const { return _MeshRawData; }
	void InjectRawDataXXX(MeshRawDataBase* InRawData) { _MeshRawData = InRawData; }

	virtual void ReleaseSystemData() = 0;
	virtual void ReleaseGALData() = 0;
};

