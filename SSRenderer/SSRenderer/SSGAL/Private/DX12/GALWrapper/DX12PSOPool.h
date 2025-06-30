#pragma once
#include "SSGAL/Private/PCommon/GALWrapper/PSOPool.h"

class DX12PSOPool : public PSOPool
{
public:
	virtual PSOWrapper* InstantiatePSO(const PipelineDesc& PipelineDesc) const override;
};
