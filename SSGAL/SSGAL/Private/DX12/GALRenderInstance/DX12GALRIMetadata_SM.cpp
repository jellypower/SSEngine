#include "pch.h"

#include "DX12GALRIMetadata_SM.h"

#include <SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h>

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "SSGAL/Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALConstantBufferAccessorTypes/CBAModelBuffer.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"


DX12GALRIMetadata_SM::DX12GALRIMetadata_SM(GALRenderDevice* InRenderDevice, const IRIMesh* InOwnerRenderInstance)
{
	_OwnerRenderDevice = (DX12GALRenderDevice*)InRenderDevice;
	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
	_SubMeshCntCache = InOwnerRenderInstance->GetMeshAsset()->GetSubMeshCnt();

	
	SS::SHasherW MeshName = InOwnerRenderInstance->GetMeshAsset()->GetAssetName();

	_OwnerRenderInstance = InOwnerRenderInstance;



	// Constant Buffer
	{
		_ModelCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBAModelBuffer), MeshName);
		DX12ConstantBufferResourcePage* ModelCBPage = (DX12ConstantBufferResourcePage*)_ModelCBChunk.PageContent;
		_ModelCBSysMemAddr = reinterpret_cast<CBAModelBuffer*>(ModelCBPage->ResourceSysMem + _ModelCBChunk.ChunkOffset);
		_ModelCBGPUMemAddr = ModelCBPage->D3D12Resource->GetGPUVirtualAddress() + _ModelCBChunk.ChunkOffset;
	}
}

DX12GALRIMetadata_SM::~DX12GALRIMetadata_SM()
{
	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();

	ConstantBufferAllocator->ReleaseChunk(_ModelCBChunk);
}

ERenderInstanceType DX12GALRIMetadata_SM::GetMetadataRenderInstanceType()
{
	return ERenderInstanceType::StaticMesh;
}
