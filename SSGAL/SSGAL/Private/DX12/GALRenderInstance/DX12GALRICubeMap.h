#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"

#include "Public/GALRenderInstance/GALRIMetadata.h"

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

	D3D12_GPU_VIRTUAL_ADDRESS GetCubemapCBGPUMemAddr() const { return _CubemapCBGPUMemAddr; }

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() override;

public:
	AllocatedChunkHeader _CubemapTextureDescTableChunk;
	ID3D12DescriptorHeap* _CubemapDescHeap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _CubemapDescTableCPU = {};
	CD3DX12_GPU_DESCRIPTOR_HANDLE _CubemapDescTableGPU = {};

	AllocatedChunkHeader _CubemapCBChunk;
	CBAModelBuffer* _CubemapCBSysMemAddr = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS _CubemapCBGPUMemAddr;

protected:
	DX12GALRenderDevice* _OwnerRenderDevice = nullptr;
	IRICubeMap* _OwnerCubeMap = nullptr;
};
