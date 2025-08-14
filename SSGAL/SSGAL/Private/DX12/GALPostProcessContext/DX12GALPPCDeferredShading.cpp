#include "pch.h"
#include "DX12GALPPCDeferredShading.h"


#include "Private/DX12/GALRenderAsset/DX12GALTextureAssetWrapper.h"
#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "Public/GALRenderTarget/GALRenderTarget.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderTarget/DX12GALRenderTargetBase.h"

DX12GALPPCDeferredShading::DX12GALPPCDeferredShading(DX12GALRenderDevice* InOwnerDevice)
{
	_OwnerDevice = InOwnerDevice;
	
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

void DX12GALPPCDeferredShading::SetRTResult(GALRenderTarget* InRenderTarget)
{
	if (InRenderTarget->GetRTColorFormat() != ERTColorFormat::R32G32B32A32_FLOAT)
	{
		SS_INTERRUPT();
		return;
	}

	_RTResult = static_cast<DX12GALRenderTargetBase*>(InRenderTarget);
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
