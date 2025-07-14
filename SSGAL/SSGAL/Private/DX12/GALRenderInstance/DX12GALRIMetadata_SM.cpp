#include "pch.h"

#include "DX12GALRIMetadata_SM.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "SSGAL/Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBAModelBuffer.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBARenderEnvParam.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"


DX12GALRIMetadata_SM::DX12GALRIMetadata_SM(GALRenderDevice* InRenderDevice, const IRIMesh* InOwnerRenderInstance)
{
	_OwnerRenderDevice = (DX12GALRenderDevice*)InRenderDevice;
	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
	_SubMeshCntCache = InOwnerRenderInstance->GetModelAsset()->GetSubMeshCnt();
	SS::SHasherW ModelName = InOwnerRenderInstance->GetModelAsset()->GetAssetName();

	_OwnerRenderInstance = InOwnerRenderInstance;



	// Constant Buffer
	{
		_ModelCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBAModelBuffer), ModelName);
		DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)_ModelCBChunk.PageContent;
		_ModelCBSysMemAddr = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + _ModelCBChunk.ChunkOffset);
		_ModelCBGPUMemAddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + _ModelCBChunk.ChunkOffset;


		// TODO: RenderWorld에서 받아와서 사용하기. -> 렌더인스턴스별로 RenderEnv파라미터 생성해주지 않아도 됨.
		// TODO: 렌더 Env에서 받아올 때 섀도우맵도 같이 받아와서 사용하도록 수정하기
		_RenderEnvCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBARenderEnvParam), ModelName);
		DX12ConstantBufferResourcePage* RenderEnvCBPage = (DX12ConstantBufferResourcePage*)_RenderEnvCBChunk.PageContent;
		_RenderEnvCBSysMemAddr = reinterpret_cast<CBARenderEnvParam*>(RenderEnvCBPage->ResourceSysMem + _RenderEnvCBChunk.ChunkOffset);
		_RenderEnvCBGPUMemAddr = RenderEnvCBPage->D3D12Resource->GetGPUVirtualAddress() + _RenderEnvCBChunk.ChunkOffset;
	}
}

DX12GALRIMetadata_SM::~DX12GALRIMetadata_SM()
{
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();

	ConstantBufferAllocator->ReleaseChunk(_ModelCBChunk);
	ConstantBufferAllocator->ReleaseChunk(_RenderEnvCBChunk);
}

ERenderInstanceType DX12GALRIMetadata_SM::GetMetadataRenderInstanceType()
{
	return ERenderInstanceType::StaticMesh;
}
