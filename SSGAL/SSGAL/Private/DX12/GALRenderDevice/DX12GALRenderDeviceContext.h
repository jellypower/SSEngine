#pragma once
// #include <d3d12.h>

#include "Private/PCommon/GALWrapper/PSOWrapper.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"

class DX12TransientConstantBufferAllocator;
class IRICubeMap;
class IRISkinnedMesh;
class GALRIShadowMapMetadata;
class DX12GALRWMetaData;
class IRenderWorld;
class IMaterialAssetMutable;
class IRIMesh;
class DX12GALDefaultPBRMaterialAsset;
class DX12GALRIMetadata_SM;
class DX12GALMeshAssetWrapper;
class DX12GALResourceUpdater;
class DX12GALRenderDevice;

class DX12GALRenderDeviceContext : public GALRenderDeviceContext
{
public:
	DX12GALRenderDeviceContext(DX12GALRenderDevice* InRenderDevice, int32 SwapChainFrameCnt);
	virtual ~DX12GALRenderDeviceContext();

public:
	virtual bool IsValid() const override;
	virtual ERenderDeviceTaskPhase GetTaskPhase() override;
	virtual GALRWMetaData* GetCurRenderWorldGALMetaData() const override;

	virtual void BeginRender() override;
	virtual void EndRender() override;

	virtual bool GenerateMeshGALAsset(IMeshAssetMutable* InMeshAsset) override;
	virtual bool GenerateTextureGALAsset(ITextureAssetMutable* InTextureAsset) override;
	virtual bool GenerateMaterialGALAsset(IMaterialAssetMutable* InMaterialAsset) override;

	virtual void GenerateRenderInstanceMetadata(IRenderInstance* InRenderInstance) override;

	virtual void AddRenderLightToDraw(IRenderLight* InLight) override;
	virtual void CommitAddedRenderLights() override;

	virtual void SetRenderCamera(IRenderCamera* InCamera) override;

	virtual void ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From, EResourceStateType To) override;
	virtual void SetPSOAndRootSignature(const PipelineDesc& InPSODesc) override;
	virtual void SetRenderTarget(int32 NumRenderTargets, GALRenderTarget** InRenderTargets, GALRenderTarget* InDepthStencilView) override;
	virtual void ClearRenderTarget(GALRenderTarget* InRenderTarget, const Vector4f& ClearColor) override;

	virtual void CopyRenderTarget(GALCPUReadableTexture* CopyDest, GALRenderTarget* CopySrc) override;
	virtual void CopyRenderTarget(GALRenderTarget* CopyDest, GALRenderTarget* CopySrc) override;





	// ERenderDeviceTaskPhase::DrawShadow
	virtual void BeginDrawShadowMap(IRenderLight* InLightToDrawShadowMap) override; // SetPSOAndRootSignature, SetRenderTarget
	virtual void DrawShadow(IRenderInstance* InRenderInstance) override;
	virtual void EndDrawShadowMap() override;
	// ERenderDeviceTaskPhase::~DrawShadow



	// ERenderDeviceTaskPhase::DrawMesh
	virtual void BeginDrawMesh() override;
	virtual void DrawMesh(IRenderInstance* InRenderInstance) override;
	virtual void EndDrawMesh() override;
	// ERenderDeviceTaskPhase::~DrawMesh


	// ERenderDeviceTaskPhase::PostProcess
	virtual void BeginPostProcessing() override;
	virtual void DrawSkyMap(IRICubeMap* CubeMapToDraw) override;
	virtual void ExecutePostProcessing(GALPostProcessContextBase* PostProcessContext) override;
	virtual void EndPostProcessing() override;
	// ERenderDeviceTaskPhase::~PostProcess


private:
	void DrawStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat);
	void DrawSkinnedMesh(IRISkinnedMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat);

	void DrawShadowStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat);
	void DrawShadowSkinnedMesh(IRISkinnedMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat);


public:
	ID3D12GraphicsCommandList* GetCurrentDrawWorkerCmdList() const { return _DrawWorkerCommandLists[_CurCommandListIdx]; }
	const SS::PooledList<ID3D12GraphicsCommandList*>& GetDrawWorkerCommandLists() const { return _DrawWorkerCommandLists; }

	ID3D12GraphicsCommandList* GetCurrentPostProcessCmdList() const;
	ID3D12CommandAllocator* GetCurrentPostProcessCmdAllocator() const;

protected:
	virtual void ResetRenderState() override;

protected:
	const SS::PooledList<GALRenderTarget*>& GetThisFrameBoundRenderTargets() const { return _BoundRenderTargets; }
	GALRenderTarget* GetThisFrameBoundDSV() const { return _BoundDSV; }

private:
	void ResetCommandList();


private:
	ERenderDeviceTaskPhase _TaskPhase = ERenderDeviceTaskPhase::TaskDenial;

	SS::PooledList<ID3D12CommandAllocator*> _DrawWorkerCommandAllocators;
	SS::PooledList <ID3D12GraphicsCommandList*> _DrawWorkerCommandLists; // TODO: SWAP_CHAIN_FRAME_COUNT * THREAD_CNT 개수만큼 만들기
	// TODO: Shadow용 CommandList 없애기

	SS::PooledList<ID3D12CommandAllocator*> _PostProcessCommandAllocators;
	SS::PooledList <ID3D12GraphicsCommandList*> _PostProcessCommandLists; // TODO: SWAP_CHAIN_FRAME_COUNT * THREAD_CNT 개수만큼 만들기

	int32 _CurCommandListIdx = 0;

	GALRenderTarget* _BoundDSV = nullptr;
	SS::PooledList<GALRenderTarget*> _BoundRenderTargets;
	SS::PooledList<IRenderLight*> _RenderLightsToDraw;

	IRenderCamera* _CurRenderCamera = nullptr;
	IRenderWorld* _CurRenderWorld = nullptr;
	DX12GALRWMetaData* _CurRenderWorldGALData = nullptr;

	GALRIShadowMapMetadata* _DrawingShadowMapMetadata = nullptr;
	PipelineDesc _LastSetPSO;

	SS::PooledList<ID3D12DescriptorHeap*, SS::InlineAllocator<10>> _UniqueDescHeapWorkTable;


};