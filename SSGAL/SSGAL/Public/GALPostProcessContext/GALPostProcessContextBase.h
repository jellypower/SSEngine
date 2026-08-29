#pragma once

class IRenderCamera;
class GALRenderDevice;
class GALRenderDeviceContext;
class GALRenderTarget;
struct PipelineDesc;

class GALPostProcessContextBase : public ISSUnknown
{
public:
	virtual void SyncGALPPCParam() = 0;
	virtual void ExecutePostProcess(GALRenderDeviceContext* Executor) = 0;

protected:
	virtual GALRenderDevice* GetOwner() const = 0;
};
