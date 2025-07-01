#pragma once
#include <d3d12.h>

#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"

class ModelAsset;
class DX12GALMaterialAssetWrapper;
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

	virtual bool GenerateMeshGALAsset(MeshAsset* InMeshAsset) override;
	virtual bool GenerateMaterialGALAsset(MaterialAsset* InMaterialAsset) override;
	virtual void GenerateRenderInstanceMetadata(IRenderInstance* InRenderInstance) override;

	virtual void ResourceBarrier(GALRenderTarget* InRenderTarget, EResourceStateType From, EResourceStateType To) override;
	virtual void SetRenderTarget(GALRenderTarget* InRenderTarget) override;
	virtual void ClearRenderTarget(GALRenderTarget* InRenderTarget) override;

	virtual void Draw(IRenderInstance* InRenderInstance) override;


private:
	void TEMP_DrawStaticMesh(
		ModelAsset* InModelAsset,
		DX12GALRIMetadata_SM* DX12RenderInstanceMetaData,
		const XMMATRIX& DrawMat,
		const XMMATRIX& DrawRotMat);

public:
	ID3D12GraphicsCommandList* GetCurrentCmdList() const { return _CommandLists[_CurCommandListIdx]; }
	const SS::PooledList<ID3D12GraphicsCommandList*>& GetCommandLists() const { return _CommandLists; }

protected:
	virtual void ResetRenderState() override;

private:
	void ResetCommandList();


private:
	SS::PooledList<ID3D12CommandAllocator*> _CommandAllocators;
	SS::PooledList <ID3D12GraphicsCommandList*> _CommandLists;
	int32 _CurCommandListIdx = 0;

};
