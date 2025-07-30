#pragma once
// #include <d3d12.h>

#include "Private/PCommon/GALWrapper/PSOWrapper.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"

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
	DX12GALRenderDeviceContext(DX12GALRenderDevice* InRenderDevice, int32 InitialCommandListCnt);
	virtual ~DX12GALRenderDeviceContext();

public:
	virtual bool IsValid() const override;

	virtual void BeginRender() override;
	virtual void EndRender() override;

	virtual bool GenerateMeshGALAsset(IMeshAssetMutable* InMeshAsset) override;
	virtual bool GenerateTextureGALAsset(ITextureAssetMutable* InTextureAsset) override;
	virtual bool GenerateMaterialGALAsset(IMaterialAssetMutable* InMaterialAsset) override;

	virtual void GenerateRenderInstanceMetadata(IRenderInstance* InRenderInstance) override;

	virtual void AddRenderLightToDraw(IRenderLight* InLight) override;
	virtual void CommitAddedRenderLights() override;

	virtual void BeginDrawShadowMap(IRenderLight* InLightToDrawShadowMap) override; // SetPSO, SetRenderTarget
	virtual void EndDrawShadowMap() override;

	virtual void SetRenderCamera(IRenderCamera* InCamera) override;

	virtual void ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From, EResourceStateType To) override;
	virtual void SetPSO(const PipelineDesc& InPSODesc) override;
	virtual void SetRenderTarget(int32 NumRenderTargets, GALRenderTarget** InRenderTargets, GALRenderTarget* InDepthStencilView) override;
	virtual void ClearRenderTarget(GALRenderTarget* InRenderTarget) override;

	virtual void CopyRenderTarget(GALCPUReadableTexture* CopyDest, GALRenderTarget* CopySrc) override;

	virtual void Draw(IRenderInstance* InRenderInstance) override;
	virtual void DrawShadow(IRenderInstance* InRenderInstance) override;
	

private:
	void DrawStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat);

	void DrawShadowStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat);


public:
	ID3D12GraphicsCommandList* GetCurrentDrawWorkerCmdList() const { return _DrawWorkerCommandLists[_CurCommandListIdx]; }
	const SS::PooledList<ID3D12GraphicsCommandList*>& GetDrawWorkerCommandLists() const { return _DrawWorkerCommandLists; }

protected:
	virtual void ResetRenderState() override;

protected:
	const SS::PooledList<GALRenderTarget*>& GetThisFrameBoundRenderTargets() const { return _BoundRenderTargets; }
	GALRenderTarget* GetThisFrameBoundDSV() const { return _BoundDSV; }

private:
	void ResetCommandList();


private:
	SS::PooledList<ID3D12CommandAllocator*> _CommandAllocators; // TODO: SWAP_CHAIN_FRAME_COUNT 개수만큼 만들기
	SS::PooledList <ID3D12GraphicsCommandList*> _DrawWorkerCommandLists; // TODO: SWAP_CHAIN_FRAME_COUNT 개수만큼 만들기
	int32 _CurCommandListIdx = 0;

	GALRenderTarget* _BoundDSV = nullptr;
	SS::PooledList<GALRenderTarget*> _BoundRenderTargets;
	SS::PooledList<IRenderLight*> _RenderLightsToDraw;

	IRenderCamera* _CurRenderCamera = nullptr;
	DX12GALRWMetaData* _CurRenderWorldGALData = nullptr;
	GALRIShadowMapMetadata* _DrawingShadowMapMetadata = nullptr;
	PipelineDesc _LastSetPSO;
};