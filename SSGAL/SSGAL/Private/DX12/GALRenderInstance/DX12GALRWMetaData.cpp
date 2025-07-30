#include "pch.h"
#include "DX12GALRWMetaData.h"

#include <SSRenderer/Public/RenderInstance/Light/IRenderLightDirectional.h>

#include "SSRenderer/Public/RenderInstance/Light/IRenderLight.h"

#include "DX12GALRIDirectionalLightShadowMapMetadata.h"
#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "SSRenderer/Public/RenderBase/IRenderWorld.h"

#include "Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderTarget/DX12GALDSVRenderTarget.h"
#include "Public/GALConstantBufferAccessorTypes/CBARenderEnvParam.h"
#include "Public/GALConstantBufferAccessorTypes/CBARenderLightParam.h"
#include "Public/GALRenderInstance/Light/GALRIShadowMapMetadata.h"

DX12GALRWMetaData::DX12GALRWMetaData(DX12GALRenderDevice* InRenderDevice, IRenderWorld* InOwnerRenderWorld)
{
	_OwnerRenderDevice = InRenderDevice;
	_OwnerRenderWorld = InOwnerRenderWorld;

	SS::SHasherW OwnerRenderWorldName = InOwnerRenderWorld->GetWorldName();
	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();

	// Alloc Constant Buffer For RenderEnv
	{
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();

		_RenderEnvCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBARenderEnvParam), OwnerRenderWorldName);
		DX12ConstantBufferResourcePage* RenderEnvCBPage = (DX12ConstantBufferResourcePage*)_RenderEnvCBChunk.PageContent;
		_RenderEnvCBSysMemAddr = reinterpret_cast<CBARenderEnvParam*>(RenderEnvCBPage->ResourceSysMem + _RenderEnvCBChunk.ChunkOffset);
		_RenderEnvCBGPUMemAddr = RenderEnvCBPage->D3D12Resource->GetGPUVirtualAddress() + _RenderEnvCBChunk.ChunkOffset;
	}

	// Alloc Constant Buffer For RenderLight
	{
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();

		_RenderLightParamCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBARenderLightParam), OwnerRenderWorldName);
		DX12ConstantBufferResourcePage* RenderLightParamPage = (DX12ConstantBufferResourcePage*)_RenderLightParamCBChunk.PageContent;
		_RenderLightParamSysMemAddr = (CBARenderLightParam*)(RenderLightParamPage->ResourceSysMem + _RenderLightParamCBChunk.ChunkOffset);
		_RenderLightParamGPUMemAddr = RenderLightParamPage->D3D12Resource->GetGPUVirtualAddress() + _RenderLightParamCBChunk.ChunkOffset;
	}

	// Alloc DescriptorTable
	{
		int32 DescriptorIncrementalSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();

		_WorldLightSettingDescTableChunk = DescriptorTableAllocator->AllocChunk(1, OwnerRenderWorldName);
		_LightSettingDescHeap = (ID3D12DescriptorHeap*)_WorldLightSettingDescTableChunk.PageContent;

		_LightSettingDescTableCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_LightSettingDescHeap->GetCPUDescriptorHandleForHeapStart(),
			_WorldLightSettingDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);

		_LightSettingDescTableGPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			_LightSettingDescHeap->GetGPUDescriptorHandleForHeapStart(),
			_WorldLightSettingDescTableChunk.ChunkOffset,
			DescriptorIncrementalSize);
	}
}

DX12GALRWMetaData::~DX12GALRWMetaData()
{
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
	SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();

	DescriptorTableAllocator->ReleaseChunk(_WorldLightSettingDescTableChunk);
	ConstantBufferAllocator->ReleaseChunk(_RenderLightParamCBChunk);
	ConstantBufferAllocator->ReleaseChunk(_RenderEnvCBChunk);
}

IRenderWorld* DX12GALRWMetaData::GetOwnerRenderWorld() const
{
	return _OwnerRenderWorld;
}

void DX12GALRWMetaData::SyncLights(const SS::PooledList<IRenderLight*>& InLights, const IRenderCamera* MainRenderCamera)
{
	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();


	int32 lShadowMapCnt = 0;
	int32 lDirectionalLightCnt = 0;
	int32 lPointLightCnt = 0;
	for (IRenderLight* LightItem : InLights)
	{
		if (LightItem->GetLightType() == ELightType::Directional)
		{
			IRenderLightDirectional* DirectionalLight = (IRenderLightDirectional*)LightItem;

			_RenderLightParamSysMemAddr->DirectionalLights[lDirectionalLightCnt].Direction
				= DirectionalLight->CalcDirectionalLightDirection();

			_RenderLightParamSysMemAddr->DirectionalLights[lDirectionalLightCnt].Direction
				= DirectionalLight->GetLightColor();


			lDirectionalLightCnt++;
		}
		else
		{
			SS_ASSERT(false);
		}


		if (LightItem->GetLightType() == ELightType::Directional
			&& LightItem->IsShadowMapEnabled() && lShadowMapCnt == 0)
		{
			IRenderLightDirectional* DirectionalLight = (IRenderLightDirectional*)LightItem;
			_RenderLightParamSysMemAddr->ShadowMapVPMat = XMMatrixTranspose(DirectionalLight->CalcShadowMapVPMatrix(MainRenderCamera));

			DX12GALRIDirectionalLightShadowMapMetadata* GALDirectionalLightShadowMapMetaData = static_cast<DX12GALRIDirectionalLightShadowMapMetadata*>(DirectionalLight->GetGALMetadata());
			DX12GALDSVRenderTarget* ShadowMap = (DX12GALDSVRenderTarget*)GALDirectionalLightShadowMapMetaData->GetShadowMap();

			CD3DX12_CPU_DESCRIPTOR_HANDLE Dest = _LightSettingDescTableCPU;
			D3DDevice->CopyDescriptorsSimple(1, Dest, ShadowMap->GetCurrentSRV(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			lShadowMapCnt++;
		}
		else
		{
			SS_ASSERT(false);
		}

	}

	_RenderLightParamSysMemAddr->DirectionalLightCnt = lDirectionalLightCnt;
	_RenderLightParamSysMemAddr->PointLightCnt = lPointLightCnt;
}