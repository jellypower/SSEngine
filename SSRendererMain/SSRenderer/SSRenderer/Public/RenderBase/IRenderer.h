#pragma once
#include <SSEngineDefault/Public/INoncopyable.h>

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"

class IAssetManager;
class IRenderWorld;
class IRenderCamera;
class GALRenderDevice;
class GALRenderDeviceContext;

class IRenderer : public INoncopyable
{
protected:
	GALRenderDeviceContext* _MainDeviceContext = nullptr;
	GALRenderDevice* _GALRenderDevice = nullptr;

public:
	GALRenderDevice* GetRenderDevice() const { return _GALRenderDevice; }

public:
	virtual IAssetManager* GetAssetManager() const = 0;
	virtual IAssetManagerMutable* GetMutableAssetManager() = 0;

public:
	virtual void StartUp() = 0;
	virtual void PerFrame() = 0;
	virtual void CleanUp() = 0;

	virtual IRenderWorld* CreateRenderWorld() = 0;

	virtual void SetRenderCamera(IRenderCamera* InCamera) = 0;
};
