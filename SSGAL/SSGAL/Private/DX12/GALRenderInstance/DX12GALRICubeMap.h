#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"

#include "Public/GALRenderInstance/GALRIMetadata.h"

struct CBARenderEnvParam;
struct CBAModelBuffer;
class DX12GALRenderDevice;
class IRICubeMap;

class DX12GALRICubeMap : public GALRIMetadata
{
public:
	DX12GALRICubeMap(DX12GALRenderDevice* OwnerRenderDevice, IRICubeMap* InOwnerCubeMap);
	virtual ~DX12GALRICubeMap();

public:
	ID3D12DescriptorHeap* GetCubeMapDescHeap() const { return _CubemapDescHeap; }
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCubemapDescTableCPU() const { return _CubemapDescTableCPU; }
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetCubemapDescTableGPU() const { return _CubemapDescTableGPU; }

	CBARenderEnvParam* GetCubemapCBRenderEnvParamSysmem() const { return _CubemapCBRenderEnvParamSysmem; }
	CBAModelBuffer* GetCubemapCBModelSysmem() const { return _CubemapCBModelSysmem; }

	D3D12_GPU_VIRTUAL_ADDRESS GetCubemapCBModelGPUMem() const { return _CubemapCBModelGPUMem; }
	D3D12_GPU_VIRTUAL_ADDRESS GetCubemapCBRenderEnvParamGPUMem() const { return _CubemapCBRenderEnvParamGPUMem; }

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() override;

private:
	AllocatedChunkHeader _CubemapTextureDescTableChunk;
	ID3D12DescriptorHeap* _CubemapDescHeap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _CubemapDescTableCPU = {};
	CD3DX12_GPU_DESCRIPTOR_HANDLE _CubemapDescTableGPU = {};

	AllocatedChunkHeader _CubemapModelCBChunk;
	CBAModelBuffer* _CubemapCBModelSysmem = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS _CubemapCBModelGPUMem;

	AllocatedChunkHeader _CubemapRenderEnvCBChunk;
	CBARenderEnvParam* _CubemapCBRenderEnvParamSysmem = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS _CubemapCBRenderEnvParamGPUMem;

protected:
	DX12GALRenderDevice* _OwnerRenderDevice = nullptr;
	IRICubeMap* _OwnerCubeMap = nullptr;
};
