#pragma once
#include "SSGAL/Public/SSGALInlineSettings.h"

#include "SSRenderer/Public/DEBUG/DebugDrawDesc.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSVertexType.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

class GALRWMetaData;
class IRICubeMap;
class GALPPCDeferredShading;
class CommonRenderAssetSet;
class ITextureAssetMutable;
class GALCPUReadableTexture;
class IRenderInstance;
class AssetManagerBase;
class IMeshAssetMutable;
class IMaterialAssetMutable;
class IRenderCamera;
class GALRenderTarget;
struct AssetInstanceReferencer;
class RenderWorld;
class GALRenderDeviceContext;
class GALRenderDevice;

static constexpr int32 DEFERRED_DESTROY_MOD = GAL_NESTED_FRAME_CNT + 1;

class SSRenderer : public IRenderer
{
private:
	CommonRenderAssetSet* _CommonRenderAssetSet = nullptr;
	AssetManagerBase* _AssetManager = nullptr;

	SS::PooledList<IRenderInstance*> _RenderInstancesToDraw;
	SS::PooledList<IRenderLight*> _RenderLightsToDraw;
	IRICubeMap* _CubeMapToDraw = nullptr;

	SS::PooledList<IMaterialAssetMutable*> _GALStateChangedMaterialAsset;
	SS::PooledList<IMeshAssetMutable*> _GALStateChangedMeshAsset;
	SS::PooledList<ITextureAssetMutable*> _GALStateChangedTextureAsset;

	SS::PooledList<void(*)()> _OneTimeCallback_BeforeGALRDCEndRender;

	SS::PooledList<DebugDrawMeshDesc> _DebugDrawItemsWithoutDepth;
	SS::PooledList<DebugDrawMeshDesc> _DebugDrawItemsWithDepth;
	SS::PooledList<SimpleLineColorVertex> _DebugDrawLinesWithDepth;
	SS::PooledList<SimpleLineColorVertex> _DebugDrawLinesWithoutDepth;

	SS::PooledList<GALRIMetadata*> _DeferredDestroyTargets[DEFERRED_DESTROY_MOD];
	SS::PooledList<GALRWMetaData*> _DeferredDestoryGALRWs[DEFERRED_DESTROY_MOD];

private:
	IRenderCamera* _MainRenderCamera = nullptr;


	GALPPCDeferredShading* _DeferredShadingContext = nullptr;
	GALRenderTarget* _RTGBufferNormal = nullptr;
	GALRenderTarget* _RTGBufferAlbedo = nullptr;
	GALRenderTarget* _RTGBufferWorldPos = nullptr;
	GALRenderTarget* _RTGBufferMetallicRoughness = nullptr;
	GALRenderTarget* _RTGBufferEmissive = nullptr;
	GALRenderTarget* _RTPostProcessResult = nullptr;


	GALRenderTarget* _DSVRenderTarget = nullptr;


	bool _bPixelPickingReserved = false;
	Vector2i32 _PixelPickingCoord;
	GALRenderTarget* _PixelPickerRenderTarget = nullptr;
	GALCPUReadableTexture* _PixelPickerCPUReadableTex = nullptr;
	SObjHashCode _PickedObjectHash;

public:
	SSRenderer(GALRenderDevice* InRenderDevice);
	void Release() override;

public:
	ICommonRenderAssetSet* GetCommonRenderAssetSet() const override;
	IAssetManager* GetAssetManager() const override;
	IAssetManagerMutable* GetMutableAssetManager() override;
	

public:
	virtual IRenderWorld* CreateRenderWorld(const utf16* InWorldName = nullptr) override;
	virtual IRIMesh* CreateRIStaticMesh() override;
	virtual IRISkinnedMesh* CreateRISkinnedMesh() override;
	virtual IRenderCamera* CreateRenderCamera() override;
	virtual IRICubeMap* CreateRICubeMap() override;

	virtual IRenderLightDirectional* CreateDirectionalLight(const RenderLightDirectionalDesc& InDesc) override;

public:
	virtual SObjHashCode GetPixelPickedObjectID() const override;
	virtual Vector2f GetViewportSize() const override;
	virtual const IRenderCamera* GetMainRenderCamera() const override;

public:
	virtual void HandoverMainViewportSwapChain(GALRenderTarget* InMainViewportSwapChain) override;
	virtual void SetMainRenderCamera(IRenderCamera* InCamera) override;
	virtual void RequestPixelPicking(int32 X, int32 Y) override;

public:
	virtual void StartUp() override;
	virtual void PerFrame() override;
	virtual void FinalizeRendering() override;
	virtual void CleanUp() override;

public:
	virtual void ReserveDestory(GALRIMetadata* DestroyTaget, int32 TargetDestroyMod) override;
	virtual void ReserveDestroyGALRW(GALRWMetaData* DestroyTarget, int32 TargetDestroyMod) override;
	virtual void ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(void(* InCallback)()) override;


public:
	virtual void DrawWireFrame(const DebugDrawMeshDesc& Desc) override;
	virtual void DrawLine(const DebugDrawLineDesc& Desc) override;

public:
	void AddGALStateChangedAsset(IAssetBase* AssetToChange);




private:
	void ProcessReservedDestroy();
	void InstantiatePendingGALAssets(GALRenderDeviceContext* Executor);

	void ScrapRenderInstsances(
		SS::PooledList<IRenderInstance*>& OutRenderInstancesToDraw,
		SS::PooledList<IRenderLight*>& OutRenderLightsToDraw,
		IRICubeMap*& OutCubeMapToDraw,
		IRenderCamera* InCamera);
	

	void Before_EndRender();


	// FinalizeFunctions
private:
	void ValidateReleaseAllGALAssets();
	void FinalizeAllReservedDestroy();
};

