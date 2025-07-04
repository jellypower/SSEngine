#pragma once
#include <SSEngineDefault/Public/INoncopyable.h>

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

	IAssetManager* _AssetManager = nullptr;

public:
	GALRenderDevice* GetRenderDevice() const { return _GALRenderDevice; }
	IAssetManager* GetAssetManager() const { return _AssetManager;  }

public:
	virtual void StartUp() = 0;
	virtual void PerFrame() = 0;
	virtual void CleanUp() = 0;

	virtual IRenderWorld* CreateRenderWorld() = 0;

	virtual void SetRenderCamera(IRenderCamera* InCamera) = 0;
};
