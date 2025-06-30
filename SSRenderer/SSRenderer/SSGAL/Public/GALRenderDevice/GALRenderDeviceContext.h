#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class MaterialAsset;
enum class EResourceStateType : uint8;
class GALRenderTarget;
class BasicRenderInstance;
class GALResourceUpdater;
class GALRenderDevice;
class MeshAsset;

class GALRenderDeviceContext : public INoncopyable
{
public:
	GALRenderDeviceContext(GALRenderDevice* InRenderDevice); // MODL: 분리
	virtual ~GALRenderDeviceContext(); // MODL: 분리

public:
	virtual bool IsValid() const = 0;
	GALRenderDevice* GetOwnerRenderDevice() const { return _OwnerRenderDevice; }

	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;


	virtual bool InstantiateMeshGPUAsset(MeshAsset* InMeshAsset) = 0;
	virtual bool InstantiateMaterialGPUAsset(MaterialAsset* InMaterialAsset) = 0;
	virtual void InstantiateRenderInstanceMetadata(BasicRenderInstance* InRenderInstance) = 0;

	virtual void ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From, EResourceStateType To) = 0;
	virtual void SetRenderTarget(GALRenderTarget* InRenderTarget) = 0;
	virtual void ClearRenderTarget(GALRenderTarget* InRenderTarget) = 0;


	virtual void Draw(BasicRenderInstance* InRenderInstance) = 0;

public:
	void SetCameraVPTransform(const XMMATRIX& InTransform) { _CameraVPTransform = InTransform; }
	void SetCameraPosition(const XMVECTOR& InPos) { _CameraPosition = InPos; }

protected:
	virtual void ResetRenderState();


protected:
	GALRenderDevice* _OwnerRenderDevice = nullptr;
	GALResourceUpdater* _ResourceUpdater = nullptr;

	XMMATRIX _CameraVPTransform;
	XMVECTOR _CameraPosition;
};
