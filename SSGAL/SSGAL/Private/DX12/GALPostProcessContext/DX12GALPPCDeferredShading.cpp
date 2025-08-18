#include "pch.h"
#include "DX12GALPPCDeferredShading.h"

#include "SSEngineDefault/Public/SSContainer/ContainerUtil/ContainerUtil.h"
#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"


#include "Public/GALRenderTarget/GALRenderTarget.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "Private/DX12/GALRenderInstance/DX12GALRWMetaData.h"
#include "Private/DX12/GALRenderTarget/DX12GALRenderTargetBase.h"
#include "Private/PCommon/GALWrapper/PSOWrapper.h"

DX12GALPPCDeferredShading::DX12GALPPCDeferredShading(DX12GALRenderDevice* InOwnerDevice)
{
	_OwnerDevice = InOwnerDevice;
	_PsoDescToExecute = ConstructPSOToDeferredShading();


	ID3D12Device5* D3DDevice = InOwnerDevice->GetD3DDevice();


	// Alloc DescriptorTable
	{
		SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerDevice->GetDescriptorTableAllocator();
		int32 DescriptorIncrementalSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		constexpr int32 GBUFFER_CNT = 5;

		_GBuffersSRVDescTableChunk = DescriptorTableAllocator->AllocChunk(GBUFFER_CNT, "DX12GALPPCDeferredShading");
		_GBuffersSRVDescHeap = (ID3D12DescriptorHeap*)_GBuffersSRVDescTableChunk.PageContent;

		_GBuffersSRVDescTableCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_GBuffersSRVDescHeap->GetCPUDescriptorHandleForHeapStart(),
			_GBuffersSRVDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);

		_GBuffersSRVDescTableGPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			_GBuffersSRVDescHeap->GetGPUDescriptorHandleForHeapStart(),
			_GBuffersSRVDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);
	}
}

DX12GALPPCDeferredShading::~DX12GALPPCDeferredShading()
{
	SSCustomMemChunkAllocator* DesciptorHandleAllocator = _OwnerDevice->GetDescriptorTableAllocator();

	DesciptorHandleAllocator->ReleaseChunk(_GBuffersSRVDescTableChunk);
}

const PipelineDesc& DX12GALPPCDeferredShading::GetPSODescToExecute()
{
	return _PsoDescToExecute;
}

void DX12GALPPCDeferredShading::SyncGALPPCParam()
{
	ID3D12Device5* D3DDevice = _OwnerDevice->GetD3DDevice();
	int32 DescriptorIncrementalSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	CD3DX12_CPU_DESCRIPTOR_HANDLE DescHandleToCopy = _GBuffersSRVDescTableCPU;

	D3DDevice->CopyDescriptorsSimple(1, DescHandleToCopy, _RTNormal->GetCurrentSRV(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DescHandleToCopy.Offset(1, DescriptorIncrementalSize);

	D3DDevice->CopyDescriptorsSimple(1, DescHandleToCopy, _RTAlbedo->GetCurrentSRV(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DescHandleToCopy.Offset(1, DescriptorIncrementalSize);

	D3DDevice->CopyDescriptorsSimple(1, DescHandleToCopy, _RTWorldPos->GetCurrentSRV(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DescHandleToCopy.Offset(1, DescriptorIncrementalSize);

	D3DDevice->CopyDescriptorsSimple(1, DescHandleToCopy, _RTMetallicRoughness->GetCurrentSRV(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DescHandleToCopy.Offset(1, DescriptorIncrementalSize);

	D3DDevice->CopyDescriptorsSimple(1, DescHandleToCopy, _RTEmissive->GetCurrentSRV(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DescHandleToCopy.Offset(1, DescriptorIncrementalSize);
}

void DX12GALPPCDeferredShading::ExecutePostProcess(GALRenderDeviceContext* Executor)
{
	if (Executor->GetTaskPhase() != ERenderDeviceTaskPhase::PostProcess)
	{
		SS_INTERRUPT();
		return;
	}

	DX12GALRenderDeviceContext* DX12Executor = (DX12GALRenderDeviceContext*)Executor;
	ID3D12GraphicsCommandList* CurCommandList = DX12Executor->GetCurrentDrawWorkerCmdList();

	DX12GALRWMetaData* CurGALRWMetaData = (DX12GALRWMetaData*)(Executor->GetCurRenderWorldGALMetaData());
	ID3D12DescriptorHeap* DeferredShadingDescHeap = GetGBufferSRVDescHeap();
	ID3D12DescriptorHeap* RenderLightDescHeap = CurGALRWMetaData->GetLightSettingDescHeap();


	DX12Executor->SetPSOAndRootSignature(_PsoDescToExecute);

	_UniqueDescHeapWorkTable.Clear();
	ListPushBackUnique(_UniqueDescHeapWorkTable, DeferredShadingDescHeap);
	ListPushBackUnique(_UniqueDescHeapWorkTable, RenderLightDescHeap);
	CurCommandList->SetDescriptorHeaps(_UniqueDescHeapWorkTable.GetSize(), _UniqueDescHeapWorkTable.GetData());


	D3D12_GPU_DESCRIPTOR_HANDLE GBufferDescTableHandle = GetGBufferSRVGPUDescTable();
	D3D12_GPU_DESCRIPTOR_HANDLE ShadowMapDescTableHandle = CurGALRWMetaData->GetLightSeetingDescTable();

	CurCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CurCommandList->SetGraphicsRootConstantBufferView(0, CurGALRWMetaData->GetRenderLightParamCB()); // RenderLight
	CurCommandList->SetGraphicsRootConstantBufferView(1, CurGALRWMetaData->_RenderEnvCBGPUMemAddr); // RenderEnvParam
	CurCommandList->SetGraphicsRootDescriptorTable(2, GBufferDescTableHandle); // G-Buffer
	CurCommandList->SetGraphicsRootDescriptorTable(3, ShadowMapDescTableHandle); // ShadowMap
	CurCommandList->DrawInstanced(3, 1, 0, 0);
}

void DX12GALPPCDeferredShading::SetRTNormal(GALRenderTarget* InRenderTarget)
{
	if (InRenderTarget->GetRTColorFormat() != ERTColorFormat::R32G32B32A32_FLOAT)
	{
		SS_INTERRUPT();
		return;
	}

	_RTNormal = static_cast<DX12GALRenderTargetBase*>(InRenderTarget);
}

void DX12GALPPCDeferredShading::SetRTAlbedo(GALRenderTarget* InRenderTarget)
{
	if (InRenderTarget->GetRTColorFormat() != ERTColorFormat::R32G32B32A32_FLOAT)
	{
		SS_INTERRUPT();
		return;
	}

	_RTAlbedo = static_cast<DX12GALRenderTargetBase*>(InRenderTarget);
}

void DX12GALPPCDeferredShading::SetRTWorldPos(GALRenderTarget* InRenderTarget)
{
	if (InRenderTarget->GetRTColorFormat() != ERTColorFormat::R32G32B32A32_FLOAT)
	{
		SS_INTERRUPT();
		return;
	}

	_RTWorldPos = static_cast<DX12GALRenderTargetBase*>(InRenderTarget);
}

void DX12GALPPCDeferredShading::SetRTMetallicRoughness(GALRenderTarget* InRenderTarget)
{
	if (InRenderTarget->GetRTColorFormat() != ERTColorFormat::R32G32_FLOAT)
	{
		SS_INTERRUPT();
		return;
	}

	_RTMetallicRoughness = static_cast<DX12GALRenderTargetBase*>(InRenderTarget);
}

void DX12GALPPCDeferredShading::SetRTEmissive(GALRenderTarget* InRenderTarget)
{
	if (InRenderTarget->GetRTColorFormat() != ERTColorFormat::R32G32B32A32_FLOAT)
	{
		SS_INTERRUPT();
		return;
	}

	_RTEmissive = static_cast<DX12GALRenderTargetBase*>(InRenderTarget);
}

GALRenderDevice* DX12GALPPCDeferredShading::GetOwner() const
{
	return _OwnerDevice;
}
