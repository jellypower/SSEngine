#pragma once
#include "SSAssetBase.h"
#include "SSGAL/Public/SSGALInlineSettings.h"

struct IMeshRawData;
class GALMeshAssetWrapperBase;



class MeshAsset : public SSAssetBase 
{
public:
	GALMeshAssetWrapperBase* _GALMeshAsset = nullptr;

protected:
	const IMeshRawData* _MeshRawData = nullptr;


public:
	MeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);
	virtual ~MeshAsset();

public:
	const IMeshRawData* GetMeshRawData() const { return _MeshRawData; }


	void InjectRawDataXXX(IMeshRawData* InRawData) { _MeshRawData = InRawData; }

	void ReleaseSystemData();
	void ReleaseGALData();

};

