#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

#include "SObject/Public/SObjHashCode.h"

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"

class ICommonRenderAssetSet;
class IRIMesh;
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
	virtual ICommonRenderAssetSet* GetCommonRenderAssetSet() const = 0;

	virtual IAssetManager* GetAssetManager() const = 0;
	virtual IAssetManagerMutable* GetMutableAssetManager() = 0;

public:
	virtual IRenderWorld* CreateRenderWorld() = 0;
	virtual IRIMesh* CreateRIStaticMesh() = 0;
	virtual SObjHashCode GetPixelPickedObjectID() const = 0;

public:
	virtual void SetRenderCamera(IRenderCamera* InCamera) = 0;
	virtual void RequestPixelPicking(int32 X, int32 Y) = 0;

public:
	virtual void StartUp() = 0;
	virtual void PerFrame() = 0;
	virtual void CleanUp() = 0;




};
