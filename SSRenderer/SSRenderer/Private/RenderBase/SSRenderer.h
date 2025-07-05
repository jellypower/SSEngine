#pragma once
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

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

	SS::PooledList<IMeshAssetMutable*> _InstanceStateChangedMesh;

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


	void AddModelInstanceReference(IModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer);
	void AddMeshInstanceReference(IMeshAsset* NewMeshAsset, const AssetInstanceReferencer& Referencer);

	void RemoveModelInstanceReference(IModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer);
	void RemoveMeshInstanceReference(IMeshAsset* MeshAssetToRemove, const AssetInstanceReferencer& Referencer);






private:
	void InstantiatePendingAssets(GALRenderDeviceContext* Executor);
	void DrawRenderWorld(GALRenderDeviceContext* Executor, IRenderCamera* InCamera);


	void InitAssetManagers();
	void CleanupAssetMnagers();
	void CleanupRenderer();
};

