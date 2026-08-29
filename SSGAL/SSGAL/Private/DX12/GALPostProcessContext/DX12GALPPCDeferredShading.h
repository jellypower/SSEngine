#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"
#include "Public/GALPostProcessContext/GALPPCDeferredShading.h"
#include "Public/GALWrapper/PipelineDesc.h"

struct CBAPPCDeferredShading;
class DX12GALRenderTargetBase;
class DX12GALRenderDevice;

class DX12GALPPCDeferredShading : public GALPPCDeferredShading
{
public:
	DX12GALPPCDeferredShading(DX12GALRenderDevice* InOwnerDevice);
	void Release() override;

public:
	virtual void SyncGALPPCParam() override;
	virtual void ExecutePostProcess(GALRenderDeviceContext* Executor) override;

	virtual void SetRTNormal(GALRenderTarget* InRenderTarget) override;
	virtual void SetRTAlbedo(GALRenderTarget* InRenderTarget) override;
	virtual void SetRTWorldPos(GALRenderTarget* InRenderTarget) override;
	virtual void SetRTMetallicRoughness(GALRenderTarget* InRenderTarget) override;
	virtual void SetRTEmissive(GALRenderTarget* InRenderTarget) override;


	ID3D12DescriptorHeap* GetGBufferSRVDescHeap() const { return _GBuffersSRVDescHeap; }
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGBufferSRVGPUDescTable() const { return _GBuffersSRVDescTableGPU; }

protected:
	GALRenderDevice* GetOwner() const override;


private:
	PipelineDesc _PsoDescToExecute;

	DX12GALRenderDevice* _OwnerDevice = nullptr;

	DX12GALRenderTargetBase* _RTNormal = nullptr;
	DX12GALRenderTargetBase* _RTAlbedo = nullptr;
	DX12GALRenderTargetBase* _RTWorldPos = nullptr;
	DX12GALRenderTargetBase* _RTMetallicRoughness = nullptr;
	DX12GALRenderTargetBase* _RTEmissive = nullptr;


	AllocatedChunkHeader _GBuffersSRVDescTableChunk;
	ID3D12DescriptorHeap* _GBuffersSRVDescHeap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _GBuffersSRVDescTableCPU = {};
	CD3DX12_GPU_DESCRIPTOR_HANDLE _GBuffersSRVDescTableGPU = {};


	SS::PooledList<ID3D12DescriptorHeap*, SS::InlineAllocator<5>> _UniqueDescHeapWorkTable;
};
