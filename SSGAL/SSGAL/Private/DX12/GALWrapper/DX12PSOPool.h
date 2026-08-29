#pragma once
#include "SSGAL/Private/PCommon/GALWrapper/PSOPool.h"

class DX12GALRenderDevice;

class DX12PSOPool : public PSOPool
{
public:
	DX12PSOPool(DX12GALRenderDevice* InRenderDevice);
	virtual void Release() override;

protected:
	virtual PSOWrapper* CreatePSO(const PipelineDesc& PipelineDesc) override;
};
