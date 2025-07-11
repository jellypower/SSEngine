#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class GALCPUReadableTexture;
class IMeshAssetMutable;
class ITextureAssetMutable;
class IMaterialAsset;
enum class EResourceStateType : uint8;
class GALRenderTarget;
class IRenderInstance;
class GALResourceUpdater;
class GALRenderDevice;

class GALRenderDeviceContext : public INoncopyable
{
public:
	virtual bool IsValid() const = 0;
	GALRenderDevice* GetOwnerRenderDevice() const { return _OwnerRenderDevice; }

	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;


	virtual bool GenerateMeshGALAsset(IMeshAssetMutable* InMeshAsset) = 0;
	virtual bool GenerateTextureGALAsset(ITextureAssetMutable* InTextureAsset) = 0;
	virtual bool GenerateMaterialGALAsset(IMaterialAsset* InMaterialAsset) = 0;
	virtual void GenerateRenderInstanceMetadata(IRenderInstance* InRenderInstance) = 0;

	virtual void ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From, EResourceStateType To) = 0;
	virtual void SetRenderTarget(GALRenderTarget* InRenderTarget) = 0;
	virtual void ClearRenderTarget(GALRenderTarget* InRenderTarget) = 0;

	virtual void CopyRenderTarget(GALCPUReadableTexture* CopyDest, GALRenderTarget* CopySrc) = 0;


	virtual void Draw(IRenderInstance* InRenderInstance) = 0;
	virtual void DrawID(IRenderInstance* InRenderInstance) = 0;

protected:
	virtual void ResetRenderState() = 0;

public:
	void SetCameraVPTransform(const XMMATRIX& InTransform) { _CameraVPTransform = InTransform; }
	void SetCameraPosition(const XMVECTOR& InPos) { _CameraPosition = InPos; }


protected:
	GALRenderDevice* _OwnerRenderDevice = nullptr;
	GALResourceUpdater* _ResourceUpdater = nullptr;

	XMMATRIX _CameraVPTransform;
	XMVECTOR _CameraPosition;
};
