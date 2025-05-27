#include "GALResourceUpdater.h"

GALResourceUpdater::GALResourceUpdater(GALRenderDevice* RenderDevice, GALRenderDeviceContext* InRenderDeviceContext)
	: _AncestorOwnerRenderDevice(RenderDevice), _OwnerDeviceContext(InRenderDeviceContext)
{
}
