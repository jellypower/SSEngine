#pragma once
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

PipelineDesc ConstructPSOToDrawSkyMap();

PipelineDesc ConstructPSOToDrawDebugWire(
	GALRenderTarget* InDSV = nullptr);

PipelineDesc ConstructPSOToDrawDebugLineList(
	GALRenderTarget* InDSV = nullptr);

PipelineDesc ConstructPSOToDrawDebugLines(
	GALRenderTarget* InDSV = nullptr);