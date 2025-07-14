#pragma once
#include <SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h>

#include "SSGAL/Public/GALRenderAsset/GALMaterialAssetWrapperBase.h"

struct CBADefaultPbrMtlParam;
class IMaterialAsset;
class DX12GALRenderDevice;
class DX12PSOWrapper;

class DX12GALDefaultPBRMaterialAsset : public GALMaterialAssetWrapperBase
{
public:
	AllocatedChunkHeader _MtlCBParamChunk;
	CBADefaultPbrMtlParam* _MtlCBSysMemAddr = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS _MtlCBGPUMemAddr;

	AllocatedChunkHeader _MtlTexSRVDescTableChunk;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _MtlTexSRVDescTableCPU = {};
	CD3DX12_GPU_DESCRIPTOR_HANDLE _MtlTexSRVDescTableGPU = {};


public:
	DX12GALDefaultPBRMaterialAsset(IMaterialAsset* ownerMaterial, DX12GALRenderDevice* InOwnerRenderDevice);
	virtual ~DX12GALDefaultPBRMaterialAsset() override;

public:
	virtual void SyncMtlParam() override;
};
