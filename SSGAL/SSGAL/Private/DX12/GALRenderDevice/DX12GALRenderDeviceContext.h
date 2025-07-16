#pragma once
// #include <d3d12.h>

#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"

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

	virtual void SetRenderCamera(IRenderCamera* InCamera) override;

	virtual void ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From, EResourceStateType To) override;
	virtual void SetRenderTarget(int32 NumRenderTargets, GALRenderTarget** InRenderTargets, GALRenderTarget* InDepthStencilView) override;
	virtual void ClearRenderTarget(GALRenderTarget* InRenderTarget) override;

	virtual void CopyRenderTarget(GALCPUReadableTexture* CopyDest, GALRenderTarget* CopySrc) override;

	virtual void Draw(IRenderInstance* InRenderInstance) override;


private:
	void DrawStaticMesh(IRIMesh* RIToDraw, const XMMATRIX& DrawMat, const XMMATRIX& DrawRotMat);



public:
	ID3D12GraphicsCommandList* GetCurrentDrawWorkerCmdList() const { return _DrawWorkerCommandLists[_CurCommandListIdx]; }
	const SS::PooledList<ID3D12GraphicsCommandList*>& GetDrawWorkerCommandLists() const { return _DrawWorkerCommandLists; }

protected:
	virtual void ResetRenderState() override;

protected:
	int32 GetThisFrameBoundRenderTargetCnt() const { return _BoundRenderTargetCnt[_CurCommandListIdx]; }
	GALRenderTarget* const* GetThisFrameBoundRenderTargets() const { return _BoundRenderTargets[_CurCommandListIdx]; }
	GALRenderTarget* GetThisFrameBoundDSV() const { return _BoundDSV[_CurCommandListIdx]; }

private:
	void ResetCommandList();


private:
	SS::PooledList<ID3D12CommandAllocator*> _CommandAllocators; // TODO: SWAP_CHAIN_FRAME_COUNT 개수만큼 만들기
	SS::PooledList <ID3D12GraphicsCommandList*> _DrawWorkerCommandLists; // TODO: SWAP_CHAIN_FRAME_COUNT 개수만큼 만들기
	int32 _CurCommandListIdx = 0;

	int32 _BoundRenderTargetCnt[SWAP_CHAIN_FRAME_COUNT] = { 0, };
	GALRenderTarget* _BoundRenderTargets[SWAP_CHAIN_FRAME_COUNT][RT_NUM_MAX] = {nullptr,};
	GALRenderTarget* _BoundDSV[SWAP_CHAIN_FRAME_COUNT] = { nullptr, };

	DX12GALRWMetaData* _CurRenderWorldGALData = nullptr;
};
