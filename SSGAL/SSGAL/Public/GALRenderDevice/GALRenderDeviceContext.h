#pragma once

#include "SSGAL/Public/SSGALInlineSettings.h" // TODO: 나중에 pch로 다 빼버릴 수도 있음

class IMeshAsset;
class IRICubeMap;
class GALRWMetaData;
class GALPostProcessContextBase;
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
class SSTransientMemAllocator;

enum class ERenderDeviceTaskPhase
{
	None = -1,

	TaskWaiting,

	DrawShadow,
	DrawMesh,
	PostProcess, // Including DeferredShadowing
	DrawDebug,

	TaskDenial,

	Count
};

class GALRenderDeviceContext : public INoncopyable
{
protected:
	GALRenderDevice* _OwnerRenderDevice = nullptr;
	GALResourceUpdater* _ResourceUpdater[GAL_NESTED_FRAME_CNT] = { nullptr, }; // TODO: 이거도 GAL_NESTED_FRAME_CNT 만큼 만들고
	SSTransientMemAllocator* _TransientCBAllocator[GAL_NESTED_FRAME_CNT] = { nullptr, }; // TODO: 이거도 GAL_NESTED_FRAME_CNT 만큼 만들어야 함

public:
	virtual bool IsValid() const = 0;
	virtual ERenderDeviceTaskPhase GetTaskPhase() = 0;
	virtual GALRWMetaData* GetCurRenderWorldGALMetaData() const = 0;

	GALRenderDevice* GetOwnerRenderDevice() const { return _OwnerRenderDevice; }
	GALResourceUpdater* GetResourceUpdater(int FrameMod) const { return _ResourceUpdater[FrameMod]; }
	SSTransientMemAllocator* GetTransientCBAllocator(int FrameMod) const { return _TransientCBAllocator[FrameMod]; }

public:
	virtual void FinalizeDeviceContext() = 0;

	virtual void BeginRender() = 0;
	virtual void WaitForCommandExecuteFinish() = 0;
	virtual void EndRender() = 0;
	virtual void Present(GALRenderTarget* SwapChainToPresent) = 0;


public:
	virtual void GenerateGALRI(IRenderInstance* InRenderInstance) const = 0;
	virtual void SyncGALRI(IRenderInstance* RIToSync, const IRenderCamera* CameraToSync) const = 0;

public:
	virtual bool GenerateMeshGALAsset(IMeshAssetMutable* InMeshAsset) = 0;
	virtual bool GenerateTextureGALAsset(ITextureAssetMutable* InTextureAsset) = 0;
	virtual bool GenerateMaterialGALAsset(IMaterialAssetMutable* InMaterialAsset) = 0;

	
	virtual void AddRenderLightToDraw(IRenderLight* InLight) = 0;
	virtual void CommitAddedRenderLights() = 0;


	virtual void SetRenderCamera(IRenderCamera* InCamera) = 0;

	virtual void ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From, EResourceStateType To) = 0;
	virtual void SetPSOAndRootSignature(const PipelineDesc& PSODesc) = 0;
	virtual void SetRenderTarget(int32 NumRenderTargets, GALRenderTarget** InRenderTargets, GALRenderTarget* InDepthStencilView) = 0;
	virtual void ClearRenderTarget(GALRenderTarget* InRenderTarget, const Vector4f& ClearColor) = 0;

	virtual void CopyRenderTarget(GALCPUReadableTexture* CopyDest, GALRenderTarget* CopySrc) = 0;
	virtual void CopyRenderTarget(GALRenderTarget* CopyDest, GALRenderTarget* CopySrc) = 0;




	// ERenderDeviceTaskPhase::DrawShadow
	virtual void BeginDrawShadowMap(IRenderLight* InLightToDrawShadowMap) = 0; // RenderTarget과 PSO를 포함한 상태를 변화시킨다
	virtual void DrawShadow(IRenderInstance* InRenderInstance) = 0;
	virtual void EndDrawShadowMap() = 0;
	// ~ERenderDeviceTaskPhase::DrawShadow



	// ERenderDeviceTaskPhase::DrawMesh
	virtual void BeginDrawMesh() = 0;
	virtual void DrawMesh(IRenderInstance* InRenderInstance) = 0;
	virtual void EndDrawMesh() = 0;
	// ~ERenderDeviceTaskPhase::DrawMesh



	// ERenderDeviceTaskPhase::PostProcess
	virtual void BeginPostProcessing() = 0;
	virtual void DrawSkyMap(IRICubeMap* CubeMapToDraw) = 0;
	virtual void ExecutePostProcessing(GALPostProcessContextBase* PostProcessContext) = 0;
	virtual void EndPostProcessing() = 0;
	// ~ERenderDeviceTaskPhase::PostProcess


	// ERenderDeviceTaskPhase::DrawDebug
	virtual void BeginDrawDebug() = 0;
	virtual void DrawDebugWire(
		const IMeshAsset* InMesh,
		const XMMATRIX& TransformMatrix,
		const XMMATRIX& RotMatrix,
		const Vector4f& InColor,
		bool bUseDepth = false) = 0;
	virtual void EndDrawDebug() = 0;
	// ERenderDeviceTaskPhase::~DrawDebug

protected:
	virtual void ResetRenderState() = 0;
	virtual void FenceFrame() = 0;
	virtual void WaitForNestedGPUJob() = 0;
};
