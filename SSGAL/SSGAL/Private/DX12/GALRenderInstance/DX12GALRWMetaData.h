#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"

#include "Public/GALRenderInstance/GALRWMetaData.h"

class IRenderCamera;
struct CBARenderLightParam;
class DX12GALRenderDevice;
struct CBARenderEnvParam;
class IRenderLight;

class DX12GALRWMetaData : public GALRWMetaData
{
public:
	CBARenderEnvParam* _RenderEnvCBSysMemAddr = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS _RenderEnvCBGPUMemAddr;

private:
	IRenderWorld* _OwnerRenderWorld = nullptr;
	DX12GALRenderDevice* _OwnerRenderDevice = nullptr;

	AllocatedChunkHeader _RenderEnvCBChunk;

	AllocatedChunkHeader _RenderLightParamCBChunk;
	CBARenderLightParam* _RenderLightParamSysMemAddr = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS _RenderLightParamGPUMemAddr;


	AllocatedChunkHeader _WorldLightSettingDescTableChunk;
	ID3D12DescriptorHeap* _LightSettingDescHeap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _LightSettingDescTableCPU = {};
	CD3DX12_GPU_DESCRIPTOR_HANDLE _LightSettingDescTableGPU = {};

public:
	DX12GALRWMetaData(DX12GALRenderDevice* InRenderDevice, IRenderWorld* InOwnerRenderWorld);
	void Release() override;
	

public:
	IRenderWorld* GetOwnerRenderWorld() const override;

public:
	ID3D12DescriptorHeap* GetLightSettingDescHeap() const { return _LightSettingDescHeap; }
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetLightSeetingDescTable() const { return _LightSettingDescTableGPU; }
	D3D12_GPU_VIRTUAL_ADDRESS GetRenderLightParamCB() const { return _RenderLightParamGPUMemAddr; }

	void SyncLights(const SS::PooledList<IRenderLight*>& InLights, const IRenderCamera* MainRenderCamera);

};
