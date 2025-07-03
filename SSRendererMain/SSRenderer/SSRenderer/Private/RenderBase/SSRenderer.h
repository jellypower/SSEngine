#pragma once
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

class IRenderCamera;
class GALRenderTarget;
struct AssetInstanceReferencer;
class ModelAsset;
class MeshAsset;
class RenderWorld;
class GALRenderDeviceContext;
class GALRenderDevice;
class ModelCombinationAssetManager;
class ModelAssetManager;
class MaterialAssetManager;
class MeshAssetManager;

class SSRenderer : public IRenderer
{
private:
	SS::PooledList<MeshAsset*> _InstanceStateChangedMesh;
	SS::PooledList<MeshAsset*> _InstanceStateChangedMaterial;

private:
	IRenderCamera* _CurRenderCamera = nullptr;
	GALRenderTarget* _PixelPickerRenderTarget = nullptr;

public:
	SSRenderer(GALRenderDevice* InRenderDevice);
	virtual ~SSRenderer();


public:
	virtual void StartUp() override;
	virtual void PerFrame() override;
	virtual void CleanUp() override;

	virtual IRenderWorld* CreateRenderWorld() override;
	virtual void SetRenderCamera(IRenderCamera* InCamera) override;


	void AddModelInstanceReference(ModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer);
	void AddMeshInstanceReference(MeshAsset* NewMeshAsset, const AssetInstanceReferencer& Referencer);

	void RemoveModelInstanceReference(ModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer);
	void RemoveMeshInstanceReference(MeshAsset* MeshAssetToRemove, const AssetInstanceReferencer& Referencer);






private:
	void InstantiatePendingAssets(GALRenderDeviceContext* Executor);
	void DrawRenderWorld(GALRenderDeviceContext* Executor, IRenderCamera* InCamera);


	void InitAssetManagers();
	void CleanupAssetMnagers();
	void CleanupRenderer();
};

