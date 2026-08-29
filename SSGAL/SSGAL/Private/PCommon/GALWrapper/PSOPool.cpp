#include "pch.h"


#include "PSOPool.h"

#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"


constexpr int32 TEMP_PSOMAP_CAPACITY = 5000;

PSOPool::PSOPool()
	: _PSOMap(TEMP_PSOMAP_CAPACITY)
{
}

const PSOWrapper* PSOPool::FindOrAddPSO(const PipelineDesc& PipelineDesc)
{
	PSOWrapper* const* Result = _PSOMap.Find(PipelineDesc);

	if(Result == nullptr)
	{
		SCOPE_PROFILE(CreatePSO);
		PSOWrapper* NewWrapper = CreatePSO(PipelineDesc);
		if(NewWrapper != nullptr)
		{
			_PSOMap.Add(PipelineDesc, NewWrapper);
		}
		return NewWrapper;
	}

	return *Result;
}

void PSOPool::ReleaseAllPSO()
{
	for (const SS::pair<PipelineDesc, PSOWrapper*>& PSOPairItem : _PSOMap)
	{
		PSOPairItem.second->Release();
	}

	_PSOMap.Clear();
}
