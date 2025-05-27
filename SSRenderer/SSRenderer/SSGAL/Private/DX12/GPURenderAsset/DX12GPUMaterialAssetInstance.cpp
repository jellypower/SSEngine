#include "DX12GPUMaterialAssetInstance.h"

#include "SSGAL/Private/GALInstanceGlobalVariablePrivate.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/GraphicsResourcePool/DX12PSOPool.h"
#include "SSGAL/Private/DX12/GraphicsResourcePool/GraphicsResourceWrapper/DX12PSOWrapper.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MaterialAsset.h"


DX12GPUMaterialAssetInstance::DX12GPUMaterialAssetInstance(MaterialAsset* ownerMaterial):
	 GPUMaterialAssetInstanceBase(ownerMaterial)
{
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)SSGALModule::Private::g_GALRenderDevice)->GetD3DDevice();
	DX12PSOPool* PSOPool = (DX12PSOPool*)SSGALModule::Private::g_GALRenderDevice->GetPSOPool();



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

bool DX12GPUMaterialAssetInstance::IsValid() const
{
	return false;
}