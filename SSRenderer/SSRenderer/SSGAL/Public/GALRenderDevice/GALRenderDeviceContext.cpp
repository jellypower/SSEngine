#include "GALRenderDeviceContext.h"

#include "SSGAL/Public/GraphicsResourcePool/GALResourceUpdater.h"

GALRenderDeviceContext::GALRenderDeviceContext(GALRenderDevice* InRenderDevice)
{
	_OwnerRenderDevice = InRenderDevice;
}

GALRenderDeviceContext::~GALRenderDeviceContext()
{
	delete _ResourceUpdater;
}

void GALRenderDeviceContext::ResetRenderState()
{
	_ResourceUpdater->ResetUpdateBuffer();
}
