#pragma once
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

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
	GALRenderTarget* _PixelPickerRenderTarget = nullptr;

public:
	SSRenderer(GALRenderDevice* InRenderDevice);
	virtual ~SSRenderer();

public:
	IAssetManager* GetAssetManager() const override;
	IAssetManagerMutable* GetMutableAssetManager() override;

public:
	virtual IRIMesh* CreateRIStaticMesh() override;

public:
	virtual void StartUp() override;
	virtual void PerFrame() override;
	virtual void CleanUp() override;

	virtual IRenderWorld* CreateRenderWorld() override;
	virtual void SetRenderCamera(IRenderCamera* InCamera) override;


	void AddGALStateChangedAsset(IAssetBase* AssetToChange);




private:
	void InstantiatePendingGALAssets(GALRenderDeviceContext* Executor);

	void ScrapRenderInstsances(SS::PooledList<IRenderInstance*>& OutRenderInstancesToDraw, IRenderCamera* InCamera);


	void InitAssetManagers();
	void CleanupAssetMnagers();
	void CleanupRenderer();
};

