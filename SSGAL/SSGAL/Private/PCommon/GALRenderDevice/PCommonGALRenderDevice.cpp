#include "pch.h"


#include "PCommonGALRenderDevice.h"

#include <SSRenderer/Public/RenderBase/IRenderer.h>

void PCommonGALRenderDevice::BindRendererXXX(IRenderer* InOwnerRenderer)
{
	if (_OwnerRenderer != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_OwnerRenderer = InOwnerRenderer;
	_CachedCommonRenderAssetSet = InOwnerRenderer->GetCommonRenderAssetSet();
}
