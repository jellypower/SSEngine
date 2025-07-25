#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

struct PipelineDesc;
class IRenderLight;
class IRenderCamera;
struct BoundBox2f;
struct ViewportBox;
class IMaterialAssetMutable;
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
	virtual bool GenerateMaterialGALAsset(IMaterialAssetMutable* InMaterialAsset) = 0;
	virtual void GenerateRenderInstanceMetadata(IRenderInstance* InRenderInstance) = 0;

	
	virtual void AddRenderLightToDraw(IRenderLight* InLight) = 0;
	virtual void BeginDrawShadowMap(IRenderLight* InLightToDrawShadowMap) = 0; // RenderTarget과 PSO를 포함한 상태를 변화시킨다
	virtual void EndDrawShadowMap() = 0;

	virtual void SetRenderCamera(IRenderCamera* InCamera) = 0;

	virtual void ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From, EResourceStateType To) = 0;
	virtual void SetPSO(const PipelineDesc& PSODesc) = 0;
	virtual void SetRenderTarget(int32 NumRenderTargets, GALRenderTarget** InRenderTargets, GALRenderTarget* InDepthStencilView) = 0;
	virtual void ClearRenderTarget(GALRenderTarget* InRenderTarget) = 0;

	virtual void CopyRenderTarget(GALCPUReadableTexture* CopyDest, GALRenderTarget* CopySrc) = 0;


	virtual void Draw(IRenderInstance* InRenderInstance) = 0;
	virtual void DrawShadow(IRenderInstance* InRenderInstance) = 0;

protected:
	virtual void ResetRenderState() = 0;


protected:
	GALRenderDevice* _OwnerRenderDevice = nullptr;
	GALResourceUpdater* _ResourceUpdater = nullptr;
};
