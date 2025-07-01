#include "DX12GPUMaterialAssetInstance.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOPool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOWrapper.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MaterialAsset.h"


DX12GPUMaterialAssetInstance::DX12GPUMaterialAssetInstance(MaterialAsset* ownerMaterial, DX12GALRenderDevice* InOwnerRenderDevice)
{
	_OwnerMaterialAsset = ownerMaterial;
	_OwnerRenderDevice = InOwnerRenderDevice;

	DX12GALRenderDevice* DX12OwnerRenderDevice = (DX12GALRenderDevice*)_OwnerRenderDevice;

	ID3D12Device5* D3DDevice = DX12OwnerRenderDevice->GetD3DDevice();
	DX12PSOPool* PSOPool = (DX12PSOPool*)DX12OwnerRenderDevice->GetPSOPool();



	PipelineDesc desc;
	desc.VSName = ownerMaterial->_VSName;
	desc.PSName = ownerMaterial->_PSName;
	desc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT;
	desc.RootSignatureType = ERootSignatureType::SS_DEFAULT_PBR;
	DX12PSOWrapper* PSOWrapper = (DX12PSOWrapper*)PSOPool->FindOrAddPSO(desc);
	if(PSOWrapper == nullptr)
	{
		DEBUG_BREAK();
		return;
	}

}

DX12GPUMaterialAssetInstance::~DX12GPUMaterialAssetInstance()
{
}
