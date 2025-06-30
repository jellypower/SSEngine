#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class GALRenderDevice;
class GALRenderDeviceContext;

class GALResourceUpdater : INoncopyable
{
public:
	GALResourceUpdater(GALRenderDevice* RenderDevice, GALRenderDeviceContext* InRenderDeviceContext); // MODL: 분리

	virtual void ResetUpdateBuffer() = 0;
	
protected:
	GALRenderDevice* _AncestorOwnerRenderDevice = nullptr;
	GALRenderDeviceContext* _OwnerDeviceContext = nullptr;
};
