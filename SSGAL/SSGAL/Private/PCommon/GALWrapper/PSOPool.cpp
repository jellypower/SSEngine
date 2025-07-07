#include "pch.h"


#include "PSOPool.h"


constexpr int32 TEMP_PSOMAP_CAPACITY = 5000;

PSOPool::PSOPool()
	: _PSOMap(TEMP_PSOMAP_CAPACITY)
{
}

PSOPool::~PSOPool()
{
}

const PSOWrapper* PSOPool::FindOrAddPSO(const PipelineDesc& PipelineDesc)
{
	PSOWrapper* const* Result = _PSOMap.Find(PipelineDesc);

	if(Result == nullptr)
	{
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
		delete PSOPairItem.second;
	}

	_PSOMap.Clear();
}
