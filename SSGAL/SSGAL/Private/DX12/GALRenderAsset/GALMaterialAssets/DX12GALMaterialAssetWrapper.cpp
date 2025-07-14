#include "pch.h"

#include "DX12GALMaterialAssetWrapper.h"

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
	DX12PSOPool* PSOPool = (DX12PSOPool*)DX12OwnerRenderDevice->GetPSOPool();



	PipelineDesc desc;
	const MtlDataBase* MtlData = ownerMaterial->GetMtlData();
	EMaterialType MatType = MtlData->_Type;
	desc.PSName = GetPSNameOfMatType(MatType);
	desc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT;
	desc.RootSignatureType = ERootSignatureType::SS_DEFAULT_PBR;
	DX12PSOWrapper* PSOWrapper = (DX12PSOWrapper*)PSOPool->FindOrAddPSO(desc);
	if(PSOWrapper == nullptr)
	{
		DEBUG_BREAK();
		return;
	}

}

DX12GALMaterialAssetWrapper::~DX12GALMaterialAssetWrapper()
{
}
