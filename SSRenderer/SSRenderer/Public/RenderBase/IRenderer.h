#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

#include "SObject/Public/SObjHashCode.h"

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"

class IRISkinnedMesh;
struct RenderLightDirectionalDesc;
class IRenderLightDirectional;
class IRenderLight;
class IRenderer;
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
	virtual IRenderWorld* CreateRenderWorld(const utf16* InWorldName = nullptr) = 0;
	virtual IRIMesh* CreateRIStaticMesh() = 0;
	virtual IRISkinnedMesh* CreateRISkinnedMesh() = 0;
	virtual IRenderCamera* CreateRenderCamera() = 0;

	virtual IRenderLightDirectional* CreateDirectionalLight(const RenderLightDirectionalDesc& InDesc) = 0;

public:
	virtual SObjHashCode GetPixelPickedObjectID() const = 0;
	virtual Vector2f GetViewportSize() const = 0;
	virtual const IRenderCamera* GetMainRenderCamera() const = 0;

public:
	virtual void SetMainRenderCamera(IRenderCamera* InCamera) = 0;
	virtual void RequestPixelPicking(int32 X, int32 Y) = 0;

public:
	virtual void StartUp() = 0; // 엔진 처음 시작할 때 1번
	virtual void PerFrame() = 0; // 매 프레임
	virtual void CleanUp() = 0; // 엔진 제거할 때 1번

public:
	virtual void ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(void (*InCallback)()) = 0;

};
