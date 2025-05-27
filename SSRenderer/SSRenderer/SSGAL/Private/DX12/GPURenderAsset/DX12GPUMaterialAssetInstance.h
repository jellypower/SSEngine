#pragma once
#include "SSGAL/Public/GPURenderAsset/GPUMaterialAssetInstanceBase.h"

class DX12PSOWrapper;

class DX12GPUMaterialAssetInstance : public GPUMaterialAssetInstanceBase
{
public:
	DX12GPUMaterialAssetInstance(MaterialAsset* ownerMaterial);
	virtual ~DX12GPUMaterialAssetInstance() override;

	bool IsValid() const override;

private:
	DX12PSOWrapper* _PSOWrapper = nullptr;
};
