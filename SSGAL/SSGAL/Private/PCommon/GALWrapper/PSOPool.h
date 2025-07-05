#pragma once
#include "PSOWrapper.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class GALRenderDevice;

class PSOPool : public INoncopyable
{
protected:
	SS::HashMap<PipelineDesc, PSOWrapper*> _PSOMap;
protected:
	GALRenderDevice* _OwnerDevice = nullptr;

public:
	PSOPool();
	virtual ~PSOPool();

	GALRenderDevice* GetOwnerDevice() const { return _OwnerDevice; }

	const PSOWrapper* FindOrAddPSO(const PipelineDesc& PipelineDesc);
	virtual void ReleaseAllPSO();

protected:
	virtual PSOWrapper* CreatePSO(const PipelineDesc& PipelineDesc) = 0;
};
