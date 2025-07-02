#pragma once
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

class SSRenderer : public INoncopyable
{
public:
	GALRenderDevice* _GALRenderDevice = nullptr;
	GALRenderDeviceContext* _MainDeviceContext = nullptr;

private:
	MaterialAssetManager* _materialAssetManager = nullptr;
	MeshAssetManager* _meshAssetManager = nullptr;
	ModelAssetManager* _ModelAssetManager = nullptr;
	ModelCombinationAssetManager* _ModelCombAssetManager = nullptr;


	SS::PooledList<MeshAsset*> _InstanceStateChangedMesh;
	SS::PooledList<MeshAsset*> _InstanceStateChangedMaterial;

private:
	IRenderCamera* _CurRenderCamera = nullptr;
	GALRenderTarget* _PixelPickerRenderTarget = nullptr;

public:
	SSRenderer();
	virtual ~SSRenderer();


	MaterialAssetManager* GetMaterialAssetManager() const { return _materialAssetManager; }
	MeshAssetManager* GetMeshAssetManager() const { return _meshAssetManager; }
	ModelAssetManager* GetModelAssetManager() const { return _ModelAssetManager; }
	ModelCombinationAssetManager* GetModelCombinationAssetManager() const { return _ModelCombAssetManager; }


	RenderWorld* CreateRenderWorld();

	// 여기서 Camera한테 World를 찾아서 SetRenderWorld해준다.
	void SetRenderCamera(IRenderCamera* InCamera);


	void AddModelInstanceReference(ModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer);
	void AddMeshInstanceReference(MeshAsset* NewMeshAsset, const AssetInstanceReferencer& Referencer);

	void RemoveModelInstanceReference(ModelAsset* NewModelAsset, const AssetInstanceReferencer& Referencer);
	void RemoveMeshInstanceReference(MeshAsset* MeshAssetToRemove, const AssetInstanceReferencer& Referencer);




	void StartUp();
	void PerFrame();
	void CleanUp();

private:
	void InstantiatePendingAssets(GALRenderDeviceContext* Executor);
	void DrawRenderWorld(GALRenderDeviceContext* Executor, IRenderCamera* InCamera);


	void InitAssetManagers();
	void CleanupAssetMnagers();
	void CleanupRenderer();
};

