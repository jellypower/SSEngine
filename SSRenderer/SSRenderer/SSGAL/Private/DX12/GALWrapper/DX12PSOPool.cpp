#include "DX12PSOPool.h"

#include "DX12PSOWrapper.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

DX12PSOPool::DX12PSOPool(DX12GALRenderDevice* InRenderDevice)
{
	_OwnerDevice = InRenderDevice;
}

PSOWrapper* DX12PSOPool::CreatePSO(const PipelineDesc& PipelineDesc)
{
	DX12PSOWrapper* NewPSOWrapper = DBG_NEW DX12PSOWrapper(PipelineDesc, this);

	if(NewPSOWrapper->IsValid() == false)
	{
		DEBUG_BREAK();
		delete NewPSOWrapper;
		return nullptr;
	}

	return NewPSOWrapper;
}
