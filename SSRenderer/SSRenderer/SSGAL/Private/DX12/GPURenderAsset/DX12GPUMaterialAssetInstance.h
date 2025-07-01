#pragma once
#include "SSGAL/Public/GPURenderAsset/GPUMaterialAssetInstanceBase.h"

class DX12GALRenderDevice;
class DX12PSOWrapper;

class DX12GPUMaterialAssetInstance : public GPUMaterialAssetInstanceBase
{
public:
	DX12GPUMaterialAssetInstance(MaterialAsset* ownerMaterial, DX12GALRenderDevice* InOwnerRenderDevice);
	virtual ~DX12GPUMaterialAssetInstance() override;

private:
	DX12PSOWrapper* _PSOWrapper = nullptr;
};
