#include "PSOPool.h"

PSOPool::PSOPool()
	: _PSOMap(5000)
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
		PSOWrapper* NewWrapper = InstantiatePSO(PipelineDesc);
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
