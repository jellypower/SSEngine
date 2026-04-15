#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

#include "SObject/Public/SObjHashCode.h"

#include "SSRenderer/Public/RenderAsset/Mutable/IAssetManagerMutable.h"

class GALRWMetaData;
class GALRIMetadata;
struct DebugDrawMeshDesc;
class IRICubeMap;
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
class GALRenderTarget;



class IRenderer : public INoncopyable
{
protected:
	GALRenderDeviceContext* _MainDeviceContext = nullptr;
	GALRenderDevice* _GALRenderDevice = nullptr;
	GALRenderTarget* _MainViewportSwapChain = nullptr;

public:
	GALRenderDevice* GetRenderDevice() const { return _GALRenderDevice; }
	GALRenderDeviceContext* GetMainDeviceContext() const { return _MainDeviceContext; }
	GALRenderTarget* GetMainViewportSwapChain() const { return _MainViewportSwapChain; }

public:
	virtual ICommonRenderAssetSet* GetCommonRenderAssetSet() const = 0;
	virtual IAssetManager* GetAssetManager() const = 0;
	virtual IAssetManagerMutable* GetMutableAssetManager() = 0;

public:
	virtual IRenderWorld* CreateRenderWorld(const utf16* InWorldName = nullptr) = 0;
	virtual IRIMesh* CreateRIStaticMesh() = 0;
	virtual IRISkinnedMesh* CreateRISkinnedMesh() = 0;
	virtual IRenderCamera* CreateRenderCamera() = 0;
	virtual IRICubeMap* CreateRICubeMap() = 0;

	virtual IRenderLightDirectional* CreateDirectionalLight(const RenderLightDirectionalDesc& InDesc) = 0;

public:
	virtual SObjHashCode GetPixelPickedObjectID() const = 0;
	virtual Vector2f GetViewportSize() const = 0;
	virtual const IRenderCamera* GetMainRenderCamera() const = 0;

public:
	virtual void HandoverMainViewportSwapChain(GALRenderTarget* InMainViewportSwapChain) = 0;
	virtual void SetMainRenderCamera(IRenderCamera* InCamera) = 0;
	virtual void RequestPixelPicking(int32 X, int32 Y) = 0;

public:
	virtual void StartUp() = 0; // 엔진 처음 시작할 때 1번
	virtual void PerFrame() = 0; // 매 프레임

	virtual void FinalizeRendering() = 0; // 렌더러 없애기 전에 GPU 작업들 끝나기를 기다리기
	virtual void CleanUp() = 0; // 엔진 제거할 때 1번

public:
	virtual void ReserveDestory(GALRIMetadata* DestroyTaget, int32 TargetDestroyMod) = 0;
	virtual void ReserveDestroyGALRW(GALRWMetaData* DestroyTarget, int32 TargetDestroyMod) = 0;
	virtual void ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(void (*InCallback)()) = 0;


public:
	virtual void DrawWireFrame(const DebugDrawMeshDesc& Desc) = 0;
};
