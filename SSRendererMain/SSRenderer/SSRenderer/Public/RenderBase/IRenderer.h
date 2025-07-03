#pragma once
#include <SSEngineDefault/Public/INoncopyable.h>

class IRenderWorld;
class IRenderCamera;
class ModelCombinationAssetManager;
class ModelAssetManager;
class MeshAssetManager;
class MaterialAssetManager;
class GALRenderDevice;
class GALRenderDeviceContext;

class IRenderer : public INoncopyable
{
protected:
	GALRenderDeviceContext* _MainDeviceContext = nullptr;
	GALRenderDevice* _GALRenderDevice = nullptr;

	MaterialAssetManager* _materialAssetManager = nullptr;
	MeshAssetManager* _meshAssetManager = nullptr;
	ModelAssetManager* _ModelAssetManager = nullptr;
	ModelCombinationAssetManager* _ModelCombAssetManager = nullptr;

public:
	GALRenderDevice* GetRenderDevice() const { return _GALRenderDevice; }

	MaterialAssetManager* GetMaterialAssetManager() const { return _materialAssetManager; }
	MeshAssetManager* GetMeshAssetManager() const { return _meshAssetManager; }
	ModelAssetManager* GetModelAssetManager() const { return _ModelAssetManager; }
	ModelCombinationAssetManager* GetModelCombinationAssetManager() const { return _ModelCombAssetManager; }


public:
	virtual void StartUp() = 0;
	virtual void PerFrame() = 0;
	virtual void CleanUp() = 0;

	virtual IRenderWorld* CreateRenderWorld() = 0;

	virtual void SetRenderCamera(IRenderCamera* InCamera) = 0;
};
