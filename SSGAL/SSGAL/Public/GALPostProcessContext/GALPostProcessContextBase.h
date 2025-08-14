#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class IRenderCamera;
class GALRenderDevice;

class GALPostProcessContextBase : public INoncopyable
{
public:
	virtual void SyncGALPPCParam() = 0;

protected:
	virtual GALRenderDevice* GetOwner() const = 0;
};
