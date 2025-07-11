#pragma once
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

class GALCPUReadableTexture;
class IRenderInstance;
class AssetManagerBase;
class IMeshAsset;
class IMeshAssetMutable;
class IModelAsset;
class IRenderCamera;
class GALRenderTarget;
struct AssetInstanceReferencer;
class RenderWorld;
class GALRenderDeviceContext;
class GALRenderDevice;

class SSRenderer : public IRenderer
{
private:
	AssetManagerBase* _AssetManager = nullptr;

	SS::PooledList<IRenderInstance*> _RenderInstancesToDraw;

	SS::PooledList<IMeshAssetMutable*> _GALStateChangedMeshAsset;

private:
	IRenderCamera* _CurRenderCamera = nullptr;

	bool _bPixelPickingReserved = false;
	Vector2i32 _PixelPickingCoord;
	GALRenderTarget* _PixelPickerRenderTarget = nullptr;
	GALCPUReadableTexture* _PixelPickerCPUReadableTex = nullptr;
	SObjHashCode _PickedObjectHash;

public:
	SSRenderer(GALRenderDevice* InRenderDevice);
	virtual ~SSRenderer();

public:
	IAssetManager* GetAssetManager() const override;
	IAssetManagerMutable* GetMutableAssetManager() override;

public:
	virtual IRenderWorld* CreateRenderWorld() override;
	virtual IRIMesh* CreateRIStaticMesh() override;
	virtual SObjHashCode GetPixelPickedObjectID() const override;

public:
	virtual void SetRenderCamera(IRenderCamera* InCamera) override;
	virtual void RequestPixelPicking(int32 X, int32 Y) override;

public:
	virtual void StartUp() override;
	virtual void PerFrame() override;
	virtual void CleanUp() override;



	void AddGALStateChangedAsset(IAssetBase* AssetToChange);




private:
	void InstantiatePendingGALAssets(GALRenderDeviceContext* Executor);

	void ScrapRenderInstsances(SS::PooledList<IRenderInstance*>& OutRenderInstancesToDraw, IRenderCamera* InCamera);


	void InitAssetManagers();
	void CleanupAssetMnagers();
	void CleanupRenderer();
};

