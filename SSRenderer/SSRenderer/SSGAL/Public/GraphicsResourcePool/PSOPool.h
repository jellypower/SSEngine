#pragma once
#include "GraphicsResourceWrapper/PSOWrapper.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class PSOPool : public INoncopyable
{
protected:
	SS::HashMap<PipelineDesc, PSOWrapper*> _PSOMap;


public:
	PSOPool();
	virtual ~PSOPool();

	const PSOWrapper* FindOrAddPSO(const PipelineDesc& PipelineDesc);
	virtual void ReleaseAllPSO();
protected:
	virtual PSOWrapper* InstantiatePSO(const PipelineDesc& PipelineDesc) const = 0;
};
