#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "Public/GALWrapper/PipelineDesc.h"


class PSOPool;

class PSOWrapper : public INoncopyable
{
private:
	PSOPool* _OwnerPSOPool = nullptr;
	PipelineDesc _pipelineDesc;

public:
	PSOWrapper(const PipelineDesc& InPipelineDesc, PSOPool* InOwnerPSOPool);
	virtual ~PSOWrapper();

	virtual bool IsValid() const = 0;
	const PipelineDesc& GetPipelineDesc() const { return _pipelineDesc; }
};



PipelineDesc ConstructPSODescToDrawMesh(
	EMeshType InMeshType,
	EMaterialType InMtlType, 
	int32 NumRenderTarget,
	GALRenderTarget* const* InRenderTargets,
	GALRenderTarget* InDSV);

PipelineDesc ConstructPSODescToDrawShadow(
	EMeshType InMeshType
);

PipelineDesc ConstructPSOToDeferredShading();