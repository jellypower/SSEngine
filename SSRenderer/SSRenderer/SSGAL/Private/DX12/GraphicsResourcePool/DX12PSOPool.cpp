#include "DX12PSOPool.h"

#include "GraphicsResourceWrapper/DX12PSOWrapper.h"

PSOWrapper* DX12PSOPool::InstantiatePSO(const PipelineDesc& PipelineDesc) const
{
	DX12PSOWrapper* NewPSOWrapper = DBG_NEW DX12PSOWrapper(PipelineDesc);

	if(NewPSOWrapper->IsValid() == false)
	{
		DEBUG_BREAK();
		delete NewPSOWrapper;
		return nullptr;
	}

	return NewPSOWrapper;
}
