#pragma once
#include "SSGAL/Public/GPURenderAsset/GALMaterialAssetWrapperBase.h"

class DX12GALRenderDevice;
class DX12PSOWrapper;

class DX12GALMaterialAssetWrapper : public GALMaterialAssetWrapperBase
{
public:
	DX12GALMaterialAssetWrapper(MaterialAsset* ownerMaterial, DX12GALRenderDevice* InOwnerRenderDevice);
	virtual ~DX12GALMaterialAssetWrapper() override;

private:
	DX12PSOWrapper* _PSOWrapper = nullptr;
};
