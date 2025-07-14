#include "pch.h"

#include "DX12GALMaterialAssetWrapper.h"

#include <SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h>

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOPool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOWrapper.h"


DX12GALMaterialAssetWrapper::DX12GALMaterialAssetWrapper(IMaterialAsset* ownerMaterial, DX12GALRenderDevice* InOwnerRenderDevice)
{
	_OwnerMaterialAsset = ownerMaterial;
	_OwnerRenderDevice = InOwnerRenderDevice;

	DX12GALRenderDevice* DX12OwnerRenderDevice = (DX12GALRenderDevice*)_OwnerRenderDevice;

	ID3D12Device5* D3DDevice = DX12OwnerRenderDevice->GetD3DDevice();
	

	SSCustomMemChunkAllocator* ConstantBufferAllocator =
		_OwnerRenderDevice->GetConstantBufferResourceAllocator();
	SSCustomMemChunkAllocator* DescriptorAllocator = 
		_OwnerRenderDevice->GetDescriptorTableAllocator();

}

DX12GALMaterialAssetWrapper::~DX12GALMaterialAssetWrapper()
{
}
