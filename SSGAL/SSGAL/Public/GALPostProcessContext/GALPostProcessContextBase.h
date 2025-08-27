#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class IRenderCamera;
class GALRenderDevice;
class GALRenderDeviceContext;
class GALRenderTarget;
struct PipelineDesc;

class GALPostProcessContextBase : public INoncopyable
{
public:
	virtual void SyncGALPPCParam() = 0;
	virtual void ExecutePostProcess(GALRenderDeviceContext* Executor) = 0;

protected:
	virtual GALRenderDevice* GetOwner() const = 0;
};
