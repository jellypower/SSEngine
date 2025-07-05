#pragma once
#include "SSGAL/Public/GALRenderAsset/GALMaterialAssetWrapperBase.h"

class IMaterialAsset;
class DX12GALRenderDevice;
class DX12PSOWrapper;

class DX12GALMaterialAssetWrapper : public GALMaterialAssetWrapperBase
{
public:
	DX12GALMaterialAssetWrapper(IMaterialAsset* ownerMaterial, DX12GALRenderDevice* InOwnerRenderDevice);
	virtual ~DX12GALMaterialAssetWrapper() override;

private:
	DX12PSOWrapper* _PSOWrapper = nullptr;
};
