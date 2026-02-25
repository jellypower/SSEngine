#pragma once

class GALRenderDevice;
class GALRenderDeviceContext;

class GALResourceUpdater : INoncopyable
{
public:
	GALResourceUpdater(GALRenderDevice* RenderDevice, GALRenderDeviceContext* InRenderDeviceContext);

	virtual void ResetUpdateBuffer() = 0;
	
protected:
	GALRenderDevice* _AncestorOwnerRenderDevice = nullptr;
	GALRenderDeviceContext* _OwnerDeviceContext = nullptr;
};
