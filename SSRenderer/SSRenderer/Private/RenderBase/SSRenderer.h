#pragma once
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

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

class SSRenderer : public IRenderer
{
private:
	CommonRenderAssetSet* _CommonRenderAssetSet = nullptr;
	AssetManagerBase* _AssetManager = nullptr;

	SS::PooledList<IRenderInstance*> _RenderInstancesToDraw;

	SS::PooledList<IMaterialAssetMutable*> _GALStateChangedMaterialAsset;
	SS::PooledList<IMeshAssetMutable*> _GALStateChangedMeshAsset;
	SS::PooledList<ITextureAssetMutable*> _GALStateChangedTextureAsset;

	SS::PooledList<void(*)()> _OneTimeCallback_BeforeGALRenderDeviceEndRender;

private:
	IRenderCamera* _MainRenderCamera = nullptr;

	bool _bPixelPickingReserved = false;
	Vector2i32 _PixelPickingCoord;
	GALRenderTarget* _DSVRenderTarget = nullptr;
	GALRenderTarget* _PixelPickerRenderTarget = nullptr;
	GALRenderTarget* _ShadowMapRenderTarget = nullptr;
	GALCPUReadableTexture* _PixelPickerCPUReadableTex = nullptr;
	SObjHashCode _PickedObjectHash;

public:
	SSRenderer(GALRenderDevice* InRenderDevice);
	virtual ~SSRenderer();

public:
	ICommonRenderAssetSet* GetCommonRenderAssetSet() const override;
	IAssetManager* GetAssetManager() const override;
	IAssetManagerMutable* GetMutableAssetManager() override;
	

public:
	virtual IRenderWorld* CreateRenderWorld(const utf16* InWorldName = nullptr) override;
	virtual IRIMesh* CreateRIStaticMesh() override;
	virtual IRenderCamera* CreateRenderCamera() override;

public:
	virtual SObjHashCode GetPixelPickedObjectID() const override;
	virtual Vector2f GetViewportSize() const override;

public:
	virtual void SetRenderCamera(IRenderCamera* InCamera) override;
	virtual void RequestPixelPicking(int32 X, int32 Y) override;

public:
	virtual void StartUp() override;
	virtual void PerFrame() override;
	virtual void CleanUp() override;

public:
	void ReserveOneTimeCallback_BeforeGALRenderDeviceEndRender(void(* InCallback)()) override;

public:
	void AddGALStateChangedAsset(IAssetBase* AssetToChange);




private:
	void InstantiatePendingGALAssets(GALRenderDeviceContext* Executor);

	void ScrapRenderInstsances(SS::PooledList<IRenderInstance*>& OutRenderInstancesToDraw, IRenderCamera* InCamera);

	void Before_GALRenderDevice_EndRender();
};

