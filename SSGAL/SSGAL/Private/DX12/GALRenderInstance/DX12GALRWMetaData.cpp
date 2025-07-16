#include "pch.h"
#include "DX12GALRWMetaData.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "SSRenderer/Public/RenderBase/IRenderWorld.h"

#include "Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Public/GALConstantBufferAccessorTypes/CBARenderEnvParam.h"

DX12GALRWMetaData::DX12GALRWMetaData(DX12GALRenderDevice* InRenderDevice, IRenderWorld* InOwnerRenderWorld)
{
	_OwnerRenderDevice = InRenderDevice;
	_OwnerRenderWorld = InOwnerRenderWorld;
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
	SS::SHasherW OwnerRenderWorldName = InOwnerRenderWorld->GetWorldName();


	{
		_RenderEnvCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBARenderEnvParam), OwnerRenderWorldName);
		DX12ConstantBufferResourcePage* RenderEnvCBPage = (DX12ConstantBufferResourcePage*)_RenderEnvCBChunk.PageContent;
		_RenderEnvCBSysMemAddr = reinterpret_cast<CBARenderEnvParam*>(RenderEnvCBPage->ResourceSysMem + _RenderEnvCBChunk.ChunkOffset);
		_RenderEnvCBGPUMemAddr = RenderEnvCBPage->D3D12Resource->GetGPUVirtualAddress() + _RenderEnvCBChunk.ChunkOffset;
	}
}

DX12GALRWMetaData::~DX12GALRWMetaData()
{
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();

	ConstantBufferAllocator->ReleaseChunk(_RenderEnvCBChunk);
}
