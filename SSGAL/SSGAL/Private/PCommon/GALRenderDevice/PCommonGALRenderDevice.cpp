#include "pch.h"


#include "PCommonGALRenderDevice.h"

#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "Private/PCommon/GALPrivateGlobals.h"

PCommonGALRenderDevice::PCommonGALRenderDevice(bool EnableDebugLayer)
{
	_EnableDebug = EnableDebugLayer;
}


void PCommonGALRenderDevice::BindRendererXXX(IRenderer* InOwnerRenderer)
{
	if (_OwnerRenderer != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_OwnerRenderer = InOwnerRenderer;
	_CachedCommonRenderAssetSet = InOwnerRenderer->GetCommonRenderAssetSet();
	g_CommonRenderAssetSet = _CachedCommonRenderAssetSet;
}
