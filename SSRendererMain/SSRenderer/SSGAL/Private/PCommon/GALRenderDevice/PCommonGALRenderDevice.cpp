#include "PCommonGALRenderDevice.h"

void PCommonGALRenderDevice::BindRendererXXX(SSRenderer* InOwnerRenderer)
{
	if (_OwnerRenderer != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_OwnerRenderer = InOwnerRenderer;
}
