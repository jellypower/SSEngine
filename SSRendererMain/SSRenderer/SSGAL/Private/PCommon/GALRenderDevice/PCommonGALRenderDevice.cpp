#include "PCommonGALRenderDevice.h"

void PCommonGALRenderDevice::BindRendererXXX(IRenderer* InOwnerRenderer)
{
	if (_OwnerRenderer != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_OwnerRenderer = InOwnerRenderer;
}
