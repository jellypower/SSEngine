#pragma once
#include "SSGAL/Public/GraphicsResourcePool/PSOPool.h"

class DX12PSOPool : public PSOPool
{
public:
	virtual PSOWrapper* InstantiatePSO(const PipelineDesc& PipelineDesc) const override;
};
